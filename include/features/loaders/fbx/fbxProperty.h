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

        inline bool isFloat() const
        {
            return type == 'F';
        }

        inline bool isDouble() const
        {
            return type == 'D';
        }

        inline bool isLong() const
        {
            return type == 'L';
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
            if (type == 'I')
                return *((int *)(data.data()));

            return 0.0f;
        }

        inline uint64 asLong() const
        {
            if (type == 'L')
                return *((uint64 *)(data.data()));
            return 0;
        }

        uint8 type;
        std::vector<uint8> data;
    };
};
