#pragma once
#include "core/core.h"
#include "core/api.h"
#include <memory>
#include <vector>

namespace wne
{
    class WNE_API File
    {
    public:
        File(const char *path, const char *access);
        ~File();
        File(const File &) = delete;
        File &operator=(const File &) = delete;

        static std::shared_ptr<File> openBinaryToRead(const char *path);
        uint32 readData(uint32 amountOfBytes, std::vector<uint8> &dst);

        bool isFinished();
        void rewindTo(uint32 position);
        uint32 getCurrentPosition();

        inline bool isValid()
        {
            return file != nullptr;
        }

    protected:
        void *file = nullptr;
    };
};
