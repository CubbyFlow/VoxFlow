// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/RenderDevice.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandJobSystem.hpp>
#include <VoxFlow/Core/Resources/Buffer.hpp>
#include <VoxFlow/Core/Resources/RenderResource.hpp>
#include <VoxFlow/Core/Resources/ResourceUploadContext.hpp>
#include <VoxFlow/Core/Resources/StagingBuffer.hpp>
#include <VoxFlow/Core/Resources/StagingBufferContext.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>

namespace VoxFlow
{
ResourceUploadContext::ResourceUploadContext(RenderDevice* renderDevice)
    : _renderDevice(renderDevice)
{
    for (uint32_t i = 0; i < static_cast<uint32_t>(LogicalDeviceType::Count);
         ++i)
    {
        LogicalDevice* logicalDevice =
            _renderDevice->getLogicalDevice(static_cast<LogicalDeviceType>(i));
        _stagingBufferContexts.emplace_back(
            std::make_unique<StagingBufferContext>(
                logicalDevice,
                logicalDevice->getDeviceDefaultResourceMemoryPool()));
    }
}

ResourceUploadContext ::~ResourceUploadContext()
{
    release();
}

void ResourceUploadContext::addPendingUpload(UploadPhase uploadPhase,
                                             RenderResource* uploadDst,
                                             UploadData&& uploadData)
{
    const LogicalDeviceType deviceType = uploadDst->getDeviceType();

    auto [stagingBuffer, stagingBufferOffset] =
        _stagingBufferContexts[static_cast<uint32_t>(deviceType)]
            ->getOrCreateStagingBuffer(uploadData._size);

    uint8_t* mappedData = stagingBuffer->map();
    memcpy(mappedData + stagingBufferOffset, uploadData._data,
           uploadData._size);
    stagingBuffer->unmap();

    PendingUploadInfo uploadInfo = { ._srcBuffer = stagingBuffer,
                                     ._dstResource = uploadDst,
                                     ._stagingBufferOffset =
                                         stagingBufferOffset,
                                     ._uploadData = std::move(uploadData) };

    if (uploadPhase == UploadPhase::Immediate)
    {
        CommandStreamKey immediateStreamKey = {
            ._cmdStreamName = IMMEDIATE_UPLOAD_STREAM_NAME,
            ._cmdStreamUsage = CommandStreamUsage::Transfer
        };

        CommandStream* cmdStream = _renderDevice->getLogicalDevice(deviceType)
                                       ->getCommandJobSystem()
                                       ->getCommandStream(immediateStreamKey);
        uploadResource(std::move(uploadInfo), cmdStream);
        cmdStream->flush(nullptr, nullptr, true);
    }
    else
    {
        _pendingUploadDatas[static_cast<uint32_t>(uploadPhase)].emplace_back(
            std::move(uploadInfo)
            );
    }
}

void ResourceUploadContext::processPendingUploads(
    UploadPhase uploadPhase, CommandStream* cmdStream)
{
    std::vector<PendingUploadInfo>& pendingUploadInfos =
        _pendingUploadDatas[static_cast<uint32_t>(uploadPhase)];
    for (PendingUploadInfo& uploadInfo : pendingUploadInfos)
    {
        uploadResource(std::move(uploadInfo), cmdStream);
    }
    pendingUploadInfos.clear();
}

void ResourceUploadContext::uploadResource(PendingUploadInfo&& uploadInfo,
                                           CommandStream* cmdStream)
{
    const RenderResourceType resourceType =
        uploadInfo._dstResource->getResourceType();
    switch (resourceType)
    {
        case RenderResourceType::Buffer:
            cmdStream->addJob(
                CommandJobType::UploadBuffer,
                static_cast<Buffer*>(uploadInfo._dstResource),
                uploadInfo._srcBuffer, uploadInfo._uploadData._dstOffset,
                uploadInfo._stagingBufferOffset, uploadInfo._uploadData._size);
            break;
        case RenderResourceType::Texture:
            cmdStream->addJob(
                CommandJobType::UploadTexture,
                static_cast<Texture*>(uploadInfo._dstResource),
                uploadInfo._srcBuffer, uploadInfo._uploadData._dstOffset,
                uploadInfo._stagingBufferOffset, uploadInfo._uploadData._size);
            break;
        default:
            break;
    }
}

void ResourceUploadContext::release()
{
    _stagingBufferContexts.clear();
}

}  // namespace VoxFlow