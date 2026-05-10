#pragma once
#include "features/data/file.h"
#include <memory>

namespace wne
{
    class FBXHeader
    {
    public:
        FBXHeader(bool isValidFlag);
        FBXHeader(const FBXHeader &) = delete;
        FBXHeader &operator=(const FBXHeader &) = delete;

        static std::unique_ptr<FBXHeader> readHeader(std::shared_ptr<File> file);

        inline bool isValid()
        {
            return isValidFlag;
        }

    protected:
        bool isValidFlag = false;
    };
};
