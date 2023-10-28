# Target name
set(target VoxFlowCore)
set(ROOT_DIR ${PROJECT_SOURCE_DIR})
set(LIBRARY_DIR ${PROJECT_SOURCE_DIR}/Dependencies/src)
set(PUBLIC_HDR_DIR ${PROJECT_SOURCE_DIR}/Includes)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../)

# Set public headers
set(PUBLIC_HDRS
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Devices/Context.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Devices/DeviceQueryContext.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Devices/Instance.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Devices/LogicalDevice.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Devices/PhysicalDevice.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Devices/Queue.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Devices/RenderDevice.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Devices/SwapChain.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/FrameGraph/BlackBoard.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/FrameGraph/DependencyGraph.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/FrameGraph/FrameGraph.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/FrameGraph/TypeTraits.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/FrameGraph/FrameGraph-Impl.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/FrameGraph/FrameGraphResources.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/FrameGraph/FrameGraphPass.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/FrameGraph/FrameGraphPass-Impl.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/FrameGraph/FrameGraphRenderPass.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/FrameGraph/Resource.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/FrameGraph/Resource-Impl.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/FrameGraph/ResourceHandle.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/FrameGraph/FrameGraphTexture.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Commands/CommandConfig.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Commands/CommandPool.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Commands/CommandJobSystem.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Commands/CommandJobSystem-Impl.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Commands/ResourceBarrierManager.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Descriptors/DescriptorSetAllocator.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Descriptors/DescriptorSetAllocatorPool.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Descriptors/DescriptorSetConfig.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Pipelines/PipelineLayoutDescriptor.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Pipelines/PipelineStateObject.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Pipelines/GlslangUtil.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Pipelines/ResourceBindingLayout.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Pipelines/BasePipeline.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Pipelines/ComputePipeline.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/RenderPass/FrameBuffer.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/RenderPass/RederTargetGroup.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/RenderPass/RenderTargetGroup.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/RenderPass/RenderPass.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/RenderPass/RenderPassParams.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/RenderPass/RenderPassCollector.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Renderer/SceneRenderer.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Renderer/SceneRenderPass.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Resources/ResourceTracker.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Resources/ResourceView.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Resources/StagingBuffer.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Resources/Texture.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Resources/Buffer.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Resources/Handle.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Resources/Sampler.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Resources/HandleAllocator.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Resources/RenderResource.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Resources/RenderResourceAllocator.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Resources/ResourceUploadContext.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Resources/RenderResourceGarbageCollector.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Resources/RenderResourceMemoryPool.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Resources/StagingBufferContext.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/BitwiseOperators.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/ChromeTracer.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/DebugUtil.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/HashUtil.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/DecisionMaker.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/DeviceInputSubscriber.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/Logger.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/Thread.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/MemoryAllocator.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/NonCopyable.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/FenceObject.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/FenceObject-Impl.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/RendererCommon.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/VertexFormat.hpp
)

# Set Sources
set(SRCS
    ${SRC_DIR}/Core/Devices/Context.cpp
    ${SRC_DIR}/Core/Devices/DeviceQueryContext.cpp
    ${SRC_DIR}/Core/Devices/Instance.cpp
    ${SRC_DIR}/Core/Devices/LogicalDevice.cpp
    ${SRC_DIR}/Core/Devices/PhysicalDevice.cpp
    ${SRC_DIR}/Core/Devices/Queue.cpp
    ${SRC_DIR}/Core/Devices/RenderDevice.cpp
    ${SRC_DIR}/Core/Devices/SwapChain.cpp
    ${SRC_DIR}/Core/FrameGraph/DependencyGraph.cpp
    ${SRC_DIR}/Core/FrameGraph/FrameGraph.cpp
    ${SRC_DIR}/Core/FrameGraph/FrameGraphPass.cpp
    ${SRC_DIR}/Core/FrameGraph/FrameGraphResources.cpp
    ${SRC_DIR}/Core/FrameGraph/Resource.cpp
    ${SRC_DIR}/Core/FrameGraph/ResourceHandle.cpp
    ${SRC_DIR}/Core/FrameGraph/FrameGraphTexture.cpp
    ${SRC_DIR}/Core/Graphics/Commands/CommandBuffer.cpp
    ${SRC_DIR}/Core/Graphics/Commands/CommandConfig.cpp
    ${SRC_DIR}/Core/Graphics/Commands/CommandPool.cpp
    ${SRC_DIR}/Core/Graphics/Commands/CommandJobSystem.cpp
    ${SRC_DIR}/Core/Graphics/Commands/ResourceBarrierManager.cpp
    ${SRC_DIR}/Core/Graphics/Descriptors/DescriptorSet.cpp
    ${SRC_DIR}/Core/Graphics/Descriptors/DescriptorSetAllocator.cpp
    ${SRC_DIR}/Core/Graphics/Descriptors/DescriptorSetAllocatorPool.cpp
    ${SRC_DIR}/Core/Graphics/Pipelines/PipelineLayout.cpp
    ${SRC_DIR}/Core/Graphics/Pipelines/PipelineLayoutDescriptor.cpp
    ${SRC_DIR}/Core/Graphics/Pipelines/GlslangUtil.cpp
    ${SRC_DIR}/Core/Graphics/Pipelines/BasePipeline.cpp
    ${SRC_DIR}/Core/Graphics/Pipelines/GraphicsPipeline.cpp
    ${SRC_DIR}/Core/Graphics/Pipelines/ComputePipeline.cpp
    ${SRC_DIR}/Core/Graphics/Pipelines/ShaderModule.cpp
    ${SRC_DIR}/Core/Graphics/RenderPass/FrameBuffer.cpp
    ${SRC_DIR}/Core/Graphics/RenderPass/RenderTargetGroup.cpp
    ${SRC_DIR}/Core/Graphics/RenderPass/RenderPass.cpp
    ${SRC_DIR}/Core/Graphics/RenderPass/RenderPassParams.cpp
    ${SRC_DIR}/Core/Graphics/RenderPass/RenderPassCollector.cpp
    ${SRC_DIR}/Core/Renderer/SceneRenderer.cpp
    ${SRC_DIR}/Core/Renderer/SceneRenderPass.cpp
    ${SRC_DIR}/Core/Resources/ResourceView.cpp
    ${SRC_DIR}/Core/Resources/RenderResource.cpp
    ${SRC_DIR}/Core/Resources/Texture.cpp
    ${SRC_DIR}/Core/Resources/Buffer.cpp
    ${SRC_DIR}/Core/Resources/Sampler.cpp
    ${SRC_DIR}/Core/Resources/RenderResourceAllocator.cpp
    ${SRC_DIR}/Core/Resources/ResourceUploadContext.cpp
    ${SRC_DIR}/Core/Resources/RenderResourceGarbageCollector.cpp
    ${SRC_DIR}/Core/Resources/RenderResourceMemoryPool.cpp
    ${SRC_DIR}/Core/Resources/StagingBufferContext.cpp
    ${SRC_DIR}/Core/Resources/StagingBuffer.cpp
    ${SRC_DIR}/Core/Utils/ChromeTracer.cpp
    ${SRC_DIR}/Core/Utils/DecisionMaker.cpp
    ${SRC_DIR}/Core/Utils/DeviceInputSubscriber.cpp
    ${SRC_DIR}/Core/Utils/DebugUtil.cpp
    ${SRC_DIR}/Core/Utils/FenceObject.cpp
    ${SRC_DIR}/Core/Utils/Thread.cpp
    ${SRC_DIR}/Core/Utils/MemoryAllocator.cpp
    ${SRC_DIR}/Core/Utils/VertexFormat.cpp
)

# Build library
add_library(${target} ${SRCS} ${BACKWARD_ENABLE})

# Enable backward-cpp stack-trace for this build
add_backward(${target})

# Project options
set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    PUBLIC_HEADER "${PUBLIC_HDRS}"
)

#Include directories
target_include_directories(${target}
    PUBLIC
    $<BUILD_INTERFACE:${PUBLIC_HDR_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
    PRIVATE
    ${PUBLIC_HDR_DIR}
    ${LIBRARY_DIR}/glfw/include
    ${LIBRARY_DIR}/volk
    ${LIBRARY_DIR}/glslang
    ${LIBRARY_DIR}/assimp/include
    ${LIBRARY_DIR}/vma/src
    ${LIBRARY_DIR}
)

# Compile options
target_compile_options(${target}
    PUBLIC
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)

# Link libraries
target_link_libraries(${target}
    PUBLIC
    ${DEFAULT_LINKER_OPTIONS}
	${DEFAULT_LIBRARIES}
    PRIVATE
    glfw
    volk
    glslang
    SPIRV
    assimp
    spirv-cross-core
    VulkanMemoryAllocator
)

# Compile definitions
target_compile_definitions(${target}
    PRIVATE
    RESOURCES_DIR="${RESOURCES_DIR}"
    ${DEFAULT_COMPILE_DEFINITIONS}
)

# Install
install(TARGETS  
        ${target} 
        RUNTIME  DESTINATION bin
        LIBRARY  DESTINATION lib
        ARCHIVE  DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)