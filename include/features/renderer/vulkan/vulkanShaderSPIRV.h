#pragma once
#include "shaderc/shaderc.h"

namespace wne
{
    class VulkanShaderSPIRV
    {
    public:
        VulkanShaderSPIRV(const char *code, const char *path, bool isVertexCode);
        ~VulkanShaderSPIRV();
        bool attemptCompile();

    protected:
        void logShaderCode(const char *code);

        shaderc_compiler_t compiler;
        shaderc_compile_options_t options;

        const char *code;
        const char *path;
        bool isVertexCode;
    };
};
