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
class CommandJobSystem;
class StagingBuffer;
class LogicalDevice;
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
    ResourceUploadContext(LogicalDevice* logicalDevice);
    ~ResourceUploadContext();

 public:
    void addPendingUpload(UploadPhase uploadPhase, RenderResource* uploadDst,
                          UploadData&& uploadData);

    void processPendingUploads(UploadPhase uploadPhase,
                               CommandJobSystem* commandJobSystem);

 private:
    struct PendingUploadInfo
    {
        StagingBuffer* _srcBuffer = nullptr;
        RenderResource* _dstResource = nullptr;
        uint32_t _stagingBufferOffset = 0;
        UploadData _uploadData = {};
    };

    LogicalDevice* _logicalDevice = nullptr;
    StagingBufferContext* _stagingBufferContext = nullptr;
    std::array<std::vector<PendingUploadInfo>,
               static_cast<uint32_t>(UploadPhase::Count)>
        _pendingUploadDatas;
};

}  // namespace VoxFlow

#endif