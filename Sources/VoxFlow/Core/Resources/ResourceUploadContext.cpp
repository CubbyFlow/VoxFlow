// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
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
ResourceUploadContext::ResourceUploadContext(LogicalDevice* logicalDevice)
    : _logicalDevice(logicalDevice)
{
    _stagingBufferContext = new StagingBufferContext(
        logicalDevice, logicalDevice->getDeviceDefaultResourceMemoryPool());
}

ResourceUploadContext ::~ResourceUploadContext()
{
    if (_stagingBufferContext != nullptr)
    {
        delete _stagingBufferContext;
    }
}

void ResourceUploadContext::addPendingUpload(UploadPhase uploadPhase,
                                             RenderResource* uploadDst,
                                             UploadData&& uploadData)
{
    StagingBuffer* stagingBuffer =
        _stagingBufferContext->getOrCreateStagingBuffer(uploadData._size);

    // TODO(snowapril) : staging buffer offset
    const uint32_t stagingBufferOffset = 0;

    uint8_t* mappedData = stagingBuffer->map();
    memcpy(mappedData + stagingBufferOffset, uploadData._data,
           uploadData._size);
    stagingBuffer->unmap();

    if (uploadPhase == UploadPhase::Immediate)
    {
        // TODO(snowapril) : allocate one-time submit command buffer and upload
    }
    else
    {
        _pendingUploadDatas[static_cast<uint32_t>(uploadPhase)].emplace_back(
            stagingBuffer, uploadDst, stagingBufferOffset,
            std::move(uploadData));
    }
}

void ResourceUploadContext::processPendingUploads(
    UploadPhase uploadPhase, CommandStream* cmdStream)
{
    std::vector<PendingUploadInfo>& pendingUploadInfos =
        _pendingUploadDatas[static_cast<uint32_t>(uploadPhase)];
    for (PendingUploadInfo& uploadInfo : pendingUploadInfos)
    {
        const RenderResourceType resourceType =
            uploadInfo._dstResource->getResourceType();
        switch (resourceType)
        {
            case RenderResourceType::Buffer:
                cmdStream->addJob(CommandJobType::UploadBuffer,
                                  static_cast<Buffer*>(uploadInfo._dstResource),
                                  uploadInfo._srcBuffer,
                                  uploadInfo._uploadData._dstOffset,
                                  uploadInfo._stagingBufferOffset,
                                  uploadInfo._uploadData._size);
                break;
            case RenderResourceType::Texture:
                cmdStream->addJob(
                    CommandJobType::UploadTexture,
                    static_cast<Texture*>(uploadInfo._dstResource),
                    uploadInfo._srcBuffer, uploadInfo._uploadData._dstOffset,
                    uploadInfo._stagingBufferOffset,
                    uploadInfo._uploadData._size);
                break;
            default:
                break;
        }
    }
    pendingUploadInfos.clear();
}

}  // namespace VoxFlow