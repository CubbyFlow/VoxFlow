// Author : snowapril

#ifndef VOXEL_FLOW_SHADER_UTIL_HPP
#define VOXEL_FLOW_SHADER_UTIL_HPP

#include <glslang_c_shader_types.h>
#include <volk/volk.h>
#include <string_view>
#include <vector>

namespace VoxFlow
{
// TODO(snowapril): `CompileShader` needs to be moved into Instance or
// LogicalDevice as it only success when they are already constructed
class GlslangUtil
{
 public:
    GlslangUtil() = delete;

    /**
     * @return glslang stage flag from given filename extension
     */
    [[nodiscard]] static glslang_stage_t GlslangStageFromFilename(
        const std::string_view filename);

    /**
     * @return glslang stage flag that match to given vulkan stage flag
     */
    [[nodiscard]] static glslang_stage_t VulkanStageToGlslangStage(
        VkShaderStageFlagBits vkStage);

    /**
     * @return vulkan stage flag that match to given glslang stage flag
     */
    [[nodiscard]] static VkShaderStageFlagBits GlslangStageToVulkanStage(
        glslang_stage_t glslangStage);

    /**
     * Read given filename as glsl shader file and return its texts to dst
     * argument.
     * @param filename filename to read as glsl shader file
     * @param dst destination of shader file text read operation
     * @return whether file open is successful or not
     */
    [[nodiscard]] static bool ReadShaderFile(const char* filename,
                                             std::vector<char>* dst);

    /**
     * Compile given shader text with glslangValidator into SPIR-V binary
     * @param stage compilation stage
     * @param shaderSource glsl shader source text data
     * @param pSpirvBinary compiled spir-v binary data is passed to given vector
     * pointer
     * @return whether spir-v compilation is successful or not
     */
    [[nodiscard]] static bool CompileShader(
        glslang_stage_t stage, const char* shaderSource,
        std::vector<unsigned int>* pSpirvBinary);
};

enum class ShaderFileType : uint8_t
{
    Glsl = 0,
    Spirv = 1,
    Undefined = 2,
    Count = Undefined
};

enum class ShaderStage : uint8_t
{
    Vertex = 0,
    TessControl = 1,
    TessEvaludation = 2,
    Geometry = 3,
    Fragment = 4,
    Compute = 5,
    Mesh = 6,
    Undefined = 7,
    Count = Undefined
};

struct ShaderPathInfo
{
    ShaderFileType fileType;
    ShaderStage shaderStage;
    std::string path;
};

class ShaderUtil
{
 public:
    [[nodiscard]] static VkShaderStageFlagBits ConvertToShaderStageFlag(
        const ShaderStage stage);

    [[nodiscard]] static std::string ConvertToShaderFileExtension(
        const ShaderStage stage);

    [[nodiscard]] static bool ReadSpirvBinary(const char* filename,
                                              std::vector<uint32_t>* dst);
};
}  // namespace VoxFlow

#endif