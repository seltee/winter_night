#pragma once
#include "string"

namespace wne
{
    class VulkanShaderMaker
    {
    public:
        void updateShaderCode();

        inline const char *getVertexMainShaderCode()
        {
            if (isDirty)
                updateShaderCode();
            return vertexMainShaderCode.c_str();
        }

        inline const char *getVertexDepthShaderCode()
        {
            if (isDirty)
                updateShaderCode();
            return vertexDepthShaderCode.c_str();
        }

        inline const char *getFragmentMainShaderCode()
        {
            if (isDirty)
                updateShaderCode();
            return fragmentMainShaderCode.c_str();
        }

        inline const char *getFragmentDepthShaderCode()
        {
            if (isDirty)
                updateShaderCode();
            return fragmentDepthShaderCode.c_str();
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
    };
};