#include <features/loaders/fbx/fbxProperty.h>

using namespace wne;

FBXProperty::FBXProperty(uint8 type, std::vector<uint8> &&data)
{
    this->type = type;
    this->data = std::move(data);
}