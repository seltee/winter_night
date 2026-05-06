#pragma once
#include <string>
#include "core/api.h"

namespace wne
{
    class WNE_API Object
    {
    public:
        void setName(const std::string &newName);

    protected:
        std::string name = "Object";
    };
}