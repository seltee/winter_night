#pragma once
#include "features/data/base3d.h"
#include "features/data/file.h"
#include "features/loaders/fbx/fbxProperty.h"
#include <memory>
#include <iostream>

namespace wne
{
    class FBXNode
    {
    public:
        struct Header
        {
            uint32 endOffset;
            uint32 numProperties;
            uint32 propertyListLen;
        };

        FBXNode(File &file);
        FBXNode(const FBXNode &) = delete;
        FBXNode &operator=(const FBXNode &) = delete;

        static std::vector<std::unique_ptr<FBXNode>> readTree(File &file);

        std::vector<FBXNode *> getChildrenByName(const char *name);
        FBXNode *findNode(const char *name);
        bool checkIfNeeded();

        inline uint8 getTypeSize(uint8 type)
        {
            switch (type)
            {
            case 'S':
                return 1;
            case 'I':
                return 4;
            case 'C':
                return 1;
            case 'R':
                return 1;
            case 'D':
                return 8;
            case 'L':
                return 8;
            case 'i':
                return 4;
            case 'd':
                return 8;
            case 'f':
                return 4;
            case 'l':
                return 8;
            }
            return 1;
        }

        inline const std::vector<FBXProperty> &getProperties()
        {
            return properties;
        }

        inline std::vector<std::unique_ptr<FBXNode>> &getChildren()
        {
            return children;
        }

        inline const char *getString(uint32 index)
        {
            if (index < properties.size() && properties[index].type == 'S')
                return (const char *)properties[index].data.data();
            std::cout << "Unable to get index " << index << " on " << name << " as string" << std::endl;
            return 0;
        }

        inline uint64 getLong(uint32 index)
        {
            if (index < properties.size() && properties[index].type == 'L')
                return *((uint64 *)properties[index].data.data());
            std::cout << "Unable to get index " << index << " on " << name << " as long" << std::endl;
            return 0;
        }

        inline double *getArrayDouble(uint32 index)
        {
            if (index < properties.size() && properties[index].type == 'd')
                return ((double *)properties[index].data.data());
            std::cout << "Unable to get index " << index << " on " << name << " as double array (" << properties[index].type << ")" << std::endl;
            return nullptr;
        }

        inline int32 *getArrayIntegers(uint32 index)
        {
            if (index < properties.size() && properties[index].type == 'i')
                return ((int32 *)properties[index].data.data());
            std::cout << "Unable to get index " << index << " on " << name << " as int array" << std::endl;
            return nullptr;
        }

        inline uint64 getElementCount(uint32 index)
        {
            if (index < properties.size())
                return properties[index].data.size() / getTypeSize(properties[index].type);
            std::cout << "Index out of bounds " << index << " on " << name << std::endl;
            return 0;
        }

        inline bool hasProperties()
        {
            return properties.size() > 0;
        }

        inline bool hasChildren()
        {
            return children.size() > 0;
        }

        inline const char *getName()
        {
            return name;
        }

        inline bool isName(const char *name)
        {
            return strcmp(this->name, name) == 0;
        }

        inline bool isValid()
        {
            return isValidFlag;
        }

    protected:
        bool isValidFlag = false;
        uint32 endOffset;
        uint32 numProperties;
        uint32 propertyListLen;
        const char *name = nullptr;

        std::vector<FBXProperty> properties;
        std::vector<std::unique_ptr<FBXNode>> children;

        bool readHeader(File &file);
        bool readValues(File &file);
        bool readSubNodes(File &file);
    };
};
