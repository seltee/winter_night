#include "features/renderer/vulkan/vulkanShaderSPIRV.h"
#include <cstring>
#include "features/logger/logger.h"

using namespace wne;

VulkanShaderSPIRV::VulkanShaderSPIRV(const char *code, const char *path, bool isVertexCode)
{
    this->code = code;
    this->path = path;
    this->isVertexCode = isVertexCode;

    compiler = shaderc_compiler_initialize();
    options = shaderc_compile_options_initialize();
}

VulkanShaderSPIRV::~VulkanShaderSPIRV()
{
    if (compiler)
        shaderc_compiler_release(compiler);
    if (options)
        shaderc_compile_options_release(options);
    if (result)
        shaderc_result_release(result);
}

bool VulkanShaderSPIRV::attemptCompile()
{
    result =
        shaderc_compile_into_spv(
            compiler,
            code,
            strlen(code),
            isVertexCode ? shaderc_vertex_shader : shaderc_fragment_shader,
            path,
            "main",
            options);

    if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success)
    {
        Logger::log << "Vulkan GLSL vertex main code compilation failed " << shaderc_result_get_error_message(result) << endl;
        Logger::log << "Shader code:" << endl;
        logShaderCode(code);
        return false;
    }

    compCodeLength = shaderc_result_get_length(result);
    compCodeBytes = reinterpret_cast<const uint8 *>(shaderc_result_get_bytes(result));

    return true;
}

void VulkanShaderSPIRV::logShaderCode(const char *code)
{
    char buffer[512];
    buffer[0] = 0;
    int i = 0, p = 0, line = 1;
    while (code[p] != 0)
    {
        if (code[p] == '\n')
        {
            Logger::log << line << " " << buffer << endl;
            i = 0;
            buffer[i] = 0;
            line++;
        }
        else
        {
            buffer[i] = code[p];
            i++;
            buffer[i] = 0;
        }
        p++;
    }
}
