#pragma once
#include "core/core.h"
#include <string>
#include <vector>

namespace wne
{
    class VulkanShaderMaker
    {
    public:
        void updateShaderCode();
        void updateNames();
        void updateSpirVCache();

        inline const char *getVertexMainShaderText()
        {
            if (isDirty)
                updateShaderCode();
            return vertexMainShaderCode.c_str();
        }

        inline const char *getVertexDepthShaderText()
        {
            if (isDirty)
                updateShaderCode();
            return vertexDepthShaderCode.c_str();
        }

        inline const char *getFragmentMainShaderText()
        {
            if (isDirty)
                updateShaderCode();
            return fragmentMainShaderCode.c_str();
        }

        inline const char *getFragmentDepthShaderText()
        {
            if (isDirty)
                updateShaderCode();
            return fragmentDepthShaderCode.c_str();
        }

        inline const std::vector<int8> &getVertexMainShaderByteCode()
        {
            if (isDirty)
                updateShaderCode();
            return vertexMainShaderCompiled;
        }

        inline const std::vector<int8> &getVertexDepthShaderByteCode()
        {
            if (isDirty)
                updateShaderCode();
            return vertexDepthShaderCompiled;
        }

        inline const std::vector<int8> &getFragmentMainShaderByteCode()
        {
            if (isDirty)
                updateShaderCode();
            return fragmentMainShaderCompiled;
        }

        inline const std::vector<int8> &getFragmentDepthShaderByteCode()
        {
            if (isDirty)
                updateShaderCode();
            return fragmentDepthShaderCompiled;
        }

        inline void setLightsState(bool enabled)
        {
            isDirty = true;
            stateLightsEnabled = enabled;
        }

        inline bool getLightsState()
        {
            return stateLightsEnabled;
        }

        inline void setBonesState(bool enabled)
        {
            isDirty = true;
            stateBonesEnabled = enabled;
        }

        inline bool getBonesState()
        {
            return stateBonesEnabled;
        }

        inline void setMaskedState(bool enabled)
        {
            isDirty = true;
            stateMaskedEnabled = enabled;
        }

        inline bool getMaskedState()
        {
            return stateMaskedEnabled;
        }

    protected:
        bool isDirty = true;

        bool stateLightsEnabled = false;
        bool stateBonesEnabled = false;
        bool stateMaskedEnabled = false;

        std::string vertexMainShaderCode;
        std::string vertexDepthShaderCode;
        std::string fragmentMainShaderCode;
        std::string fragmentDepthShaderCode;

        std::string vertexMainShaderName;
        std::string vertexDepthShaderName;
        std::string fragmentMainShaderName;
        std::string fragmentDepthShaderName;

        std::string vertexMainShaderCachePath;
        std::string vertexDepthShaderCachePath;
        std::string fragmentMainShaderCachePath;
        std::string fragmentDepthShaderCachePath;

        std::vector<int8> vertexMainShaderCompiled;
        std::vector<int8> vertexDepthShaderCompiled;
        std::vector<int8> fragmentMainShaderCompiled;
        std::vector<int8> fragmentDepthShaderCompiled;
    };
};