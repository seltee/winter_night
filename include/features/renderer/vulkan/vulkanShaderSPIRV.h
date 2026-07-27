#pragma once
#include "shaderc/shaderc.h"
#include "core/core.h"

namespace wne
{
    class VulkanShaderSPIRV
    {
    public:
        VulkanShaderSPIRV(const char *code, const char *path, bool isVertexCode);
        ~VulkanShaderSPIRV();
        bool attemptCompile();

        inline size_t getCompCodeLength()
        {
            return compCodeLength;
        }

        inline const uint8 *getCompCodeBytes()
        {
            return compCodeBytes;
        }

    protected:
        void logShaderCode(const char *code);

        shaderc_compiler_t compiler = nullptr;
        shaderc_compile_options_t options = nullptr;
        shaderc_compilation_result_t result = nullptr;

        size_t compCodeLength = 0;
        const uint8 *compCodeBytes = nullptr;

        const char *code;
        const char *path;
        bool isVertexCode;
    };
};
