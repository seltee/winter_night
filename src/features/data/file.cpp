#include "features/data/file.h"
#include <stdio.h>

using namespace wne;

File::File(const char *path, const char *access)
{
    FILE *newFile = fopen(path, access);
    if (newFile)
        file = newFile;
}

File::~File()
{
    if (file)
    {
        fclose((FILE *)file);
        file = nullptr;
    }
}

std::shared_ptr<File> File::openBinaryToRead(const char *path)
{
    std::shared_ptr<File> file = std::make_shared<File>(path, "rb");
    if (!file)
        return nullptr;
    return file;
}

uint32 File::readData(uint32 amountOfBytes, std::vector<uint8> &dst)
{
    if (!file)
        return 0;

    dst.resize(amountOfBytes);
    return (uint32)fread(dst.data(), 1, amountOfBytes, (FILE *)file);
}

bool File::isFinished()
{
    if (!file)
        return true;
    return feof((FILE *)file);
}

void File::rewindTo(uint32 position)
{
    if (!file)
        return;
    fseek((FILE *)file, position, SEEK_SET);
}

uint32 File::getCurrentPosition()
{
    return ftell((FILE *)file);
}