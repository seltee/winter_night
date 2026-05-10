#include "features/loaders/fbx/fbxHeader.h"
#include <cstring>
#include <iostream>

using namespace wne;

FBXHeader::FBXHeader(bool isValidFlag)
{
    this->isValidFlag = isValidFlag;
}

std::unique_ptr<FBXHeader> FBXHeader::readHeader(std::shared_ptr<File> file)
{
    std::vector<uint8> data;
    int readBytes = file->readData(21, data);
    if (readBytes == 0)
        return nullptr;
    if (std::strcmp((const char *)data.data(), "Kaydara FBX Binary  ") != 0)
        return nullptr;

    readBytes = file->readData(2, data);
    if (readBytes == 0)
        return nullptr;
    if (data[0] != 0x1A || data[1] != 0x00)
        return nullptr;

    readBytes = file->readData(4, data);
    if (readBytes == 0)
        return nullptr;

    uint32 version = *((uint32 *)data.data());

    if (version != 7400)
    {
        if (version < 7400)
        {
            std::cout << "Warning! FBX version is less than 7.4, errors of reading may happen" << std::endl;
        }
        else
        {
            std::cout << "Versions of FBX higher than 7.4 are unsupported, failed to load" << std::endl;
            return nullptr;
        }
    }

    return std::make_unique<FBXHeader>(true);
}