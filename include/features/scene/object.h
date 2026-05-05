#pragma once
#include <string>

namespace wne
{
    class Object
    {
    public:
        void setName(const std::string &newName);

    protected:
        std::string name = "Object";
    };
}