// Author : snowapril

#ifndef VOXEL_FLOW_RESOURCE_UPLOAD_CONTEXT_HPP
#define VOXEL_FLOW_RESOURCE_UPLOAD_CONTEXT_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <cstdint>
#include <array>
#include <vector>

namespace VoxFlow
{
class RenderResource;
class CommandStream;
class StagingBuffer;
class RenderDevice;
class StagingBufferContext;

enum class UploadPhase
{
    PreUpdate = 0,
    PreRender = 1,
    Immediate = 2,
    Undefined = 3,
    Count = Undefined
};

struct UploadData
{
    // Note(snowapril) : can be destructed after pending call
    const void* _data = nullptr;
    uint32_t _size = 0;
    uint32_t _dstOffset = 0;
};

class ResourceUploadContext : private NonCopyable
{
 public:
    ResourceUploadContext(RenderDevice* renderDevice);
    ~ResourceUploadContext();

 public:
    void addPendingUpload(UploadPhase uploadPhase, RenderResource* uploadDst,
                          UploadData&& uploadData);

    void processPendingUploads(UploadPhase uploadPhase,
                               CommandStream* cmdStream);

 private:
    struct PendingUploadInfo
    {
        StagingBuffer* _srcBuffer = nullptr;
        RenderResource* _dstResource = nullptr;
        uint32_t _stagingBufferOffset = 0;
        UploadData _uploadData = {};
    };

    void uploadResource(PendingUploadInfo&& uploadInfo,
                        CommandStream* cmdStream);

    void release();
 private:

    RenderDevice* _renderDevice = nullptr;
    std::vector<std::unique_ptr<StagingBufferContext>> _stagingBufferContexts;
    std::array<std::vector<PendingUploadInfo>,
               static_cast<uint32_t>(UploadPhase::Count)>
        _pendingUploadDatas;
};

}  // namespace VoxFlow

#endif