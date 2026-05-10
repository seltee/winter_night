#include "features/loaders/fbx/fbxNode.h"
#include "features/loaders/stb_image.h"

using namespace wne;

FBXNode::FBXNode(File &file)
{
    if (!readHeader(file))
    {
        // std::cout << "Can't read" << std::endl;
        return;
    }
    if (!checkIfNeeded())
    {
        // no need to read if it will be skipped anyway
        file.rewindTo(endOffset);
        isValidFlag = true;
        return;
    }
    if (!readValues(file))
    {
        file.rewindTo(endOffset);
        return;
    }
    if (!readSubNodes(file))
    {
        file.rewindTo(endOffset);
        return;
    }

    file.rewindTo(endOffset);
    isValidFlag = true;
}

std::vector<std::unique_ptr<FBXNode>> FBXNode::readTree(File &file)
{
    std::vector<std::unique_ptr<FBXNode>> nodes;

    while (true)
    {
        std::unique_ptr<FBXNode> node = std::make_unique<FBXNode>(file);
        if (node->isValid())
        {
            if (node->checkIfNeeded())
            {
                nodes.emplace_back(std::move(node));
            }
        }
        else
        {
            break;
        }
    }

    return nodes;
}

std::vector<FBXNode *> FBXNode::getChildrenByName(const char *name)
{
    std::vector<FBXNode *> outList;
    for (auto &it : children)
    {
        if (it->isName(name))
        {
            outList.push_back(it.get());
        }
    }
    return outList;
}

FBXNode *FBXNode::findNode(const char *name)
{
    for (auto &it : children)
    {
        if (it->isName(name))
            return it.get();
    }
    return nullptr;
}

bool FBXNode::checkIfNeeded()
{
    return !(
        (strcmp(name, "FBXHeaderExtension") == 0) ||
        (strcmp(name, "Takes") == 0) ||
        (strcmp(name, "Document") == 0) ||
        (strcmp(name, "Documents") == 0) ||
        (strcmp(name, "References") == 0) ||
        (strcmp(name, "Definitions") == 0) ||
        (strcmp(name, "Video") == 0) ||
        (strcmp(name, "Material") == 0) ||
        (strcmp(name, "Texture") == 0));
}

bool FBXNode::readHeader(File &file)
{
    // HEADER
    if (file.isFinished())
        return false;
    std::vector<uint8> data;
    int count = file.readData(sizeof(Header), data);
    if (count == 0)
        return false;

    Header header;
    memcpy(&header, data.data(), sizeof(Header));

    count = file.readData(1, data);
    if (count == 0)
        return false;

    uint8 nameLength = data[0];
    if (nameLength == 0)
        return false;

    endOffset = header.endOffset;
    numProperties = header.numProperties;
    propertyListLen = header.propertyListLen;

    count = file.readData(nameLength, data);
    if (count == 0)
        return false;

    data.push_back(0);
    name = strdup((const char *)data.data());
    return true;
}

bool FBXNode::readValues(File &file)
{
    if (propertyListLen == 0)
        return true;

    std::vector<uint8> putData;

    uint32 p = 0, rawLength, arrayLength, encoding, compressedLength;

    std::vector<uint8> propertiesData;
    uint32 count = file.readData(propertyListLen, propertiesData);
    if (count == 0 || count != propertyListLen)
        return false;

    for (uint32 i = 0; i < numProperties; i++)
    {
        switch (propertiesData[p])
        {
        case 'S':
            rawLength = *((uint32 *)&propertiesData[p + 1]);
            p += 5;
            if (rawLength)
            {
                putData.resize(rawLength);
                memcpy(putData.data(), &propertiesData[p], rawLength);
                putData.push_back(0);

                properties.emplace_back('S', std::move(putData));
                p += rawLength;

                // std::cout << "String " << (const char *)putData.data() << std::endl;
            }
            break;

        case 'I':
            putData.resize(4);
            memcpy(putData.data(), &propertiesData[p + 1], 4);
            p += 5;
            properties.emplace_back('I', std::move(putData));
            break;

        case 'C':
            putData.resize(1);
            memcpy(putData.data(), &propertiesData[p + 1], 1);
            p += 2;
            properties.emplace_back('C', std::move(putData));
            break;

        case 'R':
            // todo do something with binary blobs
            rawLength = *((uint32 *)&propertiesData[p + 1]);
            p += 5 + rawLength;
            putData.resize(0);
            properties.emplace_back('C', std::move(putData));
            break;

        case 'D':
            putData.resize(8);
            memcpy(putData.data(), &propertiesData[p + 1], 8);
            p += 9;
            properties.emplace_back('D', std::move(putData));
            break;

        case 'L':
            putData.resize(8);
            memcpy(putData.data(), &propertiesData[p + 1], 8);
            p += 9;
            properties.emplace_back('L', std::move(putData));
            break;

        case 'd':
            arrayLength = *((uint32 *)&propertiesData[p + 1]);
            encoding = *((uint32 *)&propertiesData[p + 5]);
            compressedLength = *((uint32 *)&propertiesData[p + 9]);
            p += 13;
            rawLength = arrayLength * sizeof(double); // end size
            putData.resize(rawLength);

            if (encoding)
                stbi_zlib_decode_buffer((char *)putData.data(), rawLength, (char *)&propertiesData.data()[p], compressedLength);
            else
                memcpy(putData.data(), &propertiesData[p], rawLength);

            properties.emplace_back('d', std::move(putData));
            break;

        case 'f':
            arrayLength = *((uint32 *)&propertiesData[p + 1]);
            encoding = *((uint32 *)&propertiesData[p + 5]);
            compressedLength = *((uint32 *)&propertiesData[p + 9]);
            p += 13;
            rawLength = arrayLength * sizeof(float); // end size
            putData.resize(rawLength);

            if (encoding)
                stbi_zlib_decode_buffer((char *)putData.data(), rawLength, (char *)&propertiesData.data()[p], compressedLength);
            else
                memcpy(putData.data(), &propertiesData[p], rawLength);

            properties.emplace_back('f', std::move(putData));
            break;

        case 'i':
            arrayLength = *((uint32 *)&propertiesData[p + 1]);
            encoding = *((uint32 *)&propertiesData[p + 5]);
            compressedLength = *((uint32 *)&propertiesData[p + 9]);
            p += 13;
            rawLength = arrayLength * sizeof(int32); // end size
            putData.resize(rawLength);

            if (encoding)
                stbi_zlib_decode_buffer((char *)putData.data(), rawLength, (char *)&propertiesData.data()[p], compressedLength);
            else
                memcpy(putData.data(), &propertiesData[p], rawLength);

            properties.emplace_back('i', std::move(putData));
            break;

        case 'l':
            arrayLength = *((uint32 *)&propertiesData[p + 1]);
            encoding = *((uint32 *)&propertiesData[p + 5]);
            compressedLength = *((uint32 *)&propertiesData[p + 9]);
            p += 13;
            rawLength = arrayLength * sizeof(uint64); // end size
            putData.resize(rawLength);

            if (encoding)
                stbi_zlib_decode_buffer((char *)putData.data(), rawLength, (char *)&propertiesData.data()[p], compressedLength);
            else
                memcpy(putData.data(), &propertiesData[p], rawLength);

            properties.emplace_back('l', std::move(putData));
            break;

        default:
            std::cout << "Unknown type " << propertiesData[p] << std::endl;
            return false;
        }
    }

    return true;
}

bool FBXNode::readSubNodes(File &file)
{

    while (file.getCurrentPosition() < endOffset)
    {
        auto subNode = std::make_unique<FBXNode>(file);
        if (subNode->isValid())
        {
            children.emplace_back(std::move(subNode));
        }
    }
    return true;
}