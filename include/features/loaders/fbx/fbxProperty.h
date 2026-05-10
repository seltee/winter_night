#pragma once
#include "core/core.h"
#include <vector>

namespace wne
{
    class FBXProperty
    {
    public:
        FBXProperty(uint8 type, std::vector<uint8> &&data);

        inline bool isString() const
        {
            return type == 'S';
        }

        inline bool idDouble() const
        {
            return type == 'D';
        }

        inline const char *asString() const
        {
            
            return (const char *)data.data();
        }

        inline float asFloat() const
        {
            if (type == 'D')
                return (float)*((double *)(data.data()));
            if (type == 'F')
                return *((float *)(data.data()));
            return 0.0f;
        }

        uint8 type;
        std::vector<uint8> data;
    };
};
