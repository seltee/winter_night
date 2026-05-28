#include "utils/primitives.h"
#include <iostream>

using namespace wne;

std::shared_ptr<Model> Primitives::createPlane(float halfSize)
{
    return createPlane(halfSize, Vector3::up());
}

std::shared_ptr<Model> Primitives::createPlane(float halfSize, const Vector3 &normal)
{
    Vector3 up = fabs(normal.y) < 0.999f
                     ? Vector3{0, 1, 0}
                     : Vector3{1, 0, 0};

    Vector3 xAxis = normalize(cross(up, normal));
    Vector3 zAxis = normalize(cross(normal, xAxis));

    const std::vector<wne::VertexTextured> vertices = {
        {xAxis * -halfSize + zAxis * -halfSize, {0.0f, 1.0f}, normal},
        {xAxis * halfSize + zAxis * -halfSize, {1.0f, 1.0f}, normal},
        {xAxis * halfSize + zAxis * halfSize, {1.0f, 0.0f}, normal},
        {xAxis * -halfSize + zAxis * halfSize, {0.0f, 0.0f}, normal}};

    const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

    return Model::createFromData(vertices, indices);
}

std::shared_ptr<Model> Primitives::createBox(float size)
{
    return createBox(Vector3(size, size, size));
}

std::shared_ptr<Model> Primitives::createBox(const Vector3 &size)
{
    float x = size.x;
    float y = size.y;
    float z = size.z;

    const std::vector<wne::VertexTextured> vertices = {
        {{x, y, -z}, {1.0f, 0.0f}, {1.0f, 0, 0}},
        {{x, y, z}, {1.0f, 1.0f}, {1.0f, 0, 0}},
        {{x, -y, z}, {0.0f, 1.0f}, {1.0f, 0, 0}},
        {{x, -y, -z}, {0.0f, 0.0f}, {1.0f, 0, 0}},

        {{-x, y, -z}, {0.0f, 0.0f}, {-1.0f, 0, 0}},
        {{-x, y, z}, {0.0f, 1.0f}, {-1.0f, 0, 0}},
        {{-x, -y, z}, {1.0f, 1.0f}, {-1.0f, 0, 0}},
        {{-x, -y, -z}, {1.0f, 0.0f}, {-1.0f, 0, 0}},

        {{-x, -y, -z}, {0.0f, 0.0f}, {0, 0, -1.0f}},
        {{-x, y, -z}, {0.0f, 1.0f}, {0, 0, -1.0f}},
        {{x, y, -z}, {1.0f, 1.0f}, {0, 0, -1.0f}},
        {{x, -y, -z}, {1.0f, 0.0f}, {0, 0, -1.0f}},

        {{-x, -y, z}, {1.0f, 0.0f}, {0, 0, 1.0f}},
        {{-x, y, z}, {1.0f, 1.0f}, {0, 0, 1.0f}},
        {{x, y, z}, {0.0f, 1.0f}, {0, 0, 1.0f}},
        {{x, -y, z}, {0.0f, 0.0f}, {0, 0, 1.0f}},

        {{-x, y, -z}, {1.0f, 0.0f}, {0, 1.0f, 0}},
        {{-x, y, z}, {1.0f, 1.0f}, {0, 1.0f, 0}},
        {{x, y, z}, {0.0f, 1.0f}, {0, 1.0f, 0}},
        {{x, y, -z}, {0.0f, 0.0f}, {0, 1.0f, 0}},

        {{-x, -y, -z}, {1.0f, 0.0f}, {0, -1.0f, 0}},
        {{-x, -y, z}, {1.0f, 1.0f}, {0, -1.0f, 0}},
        {{x, -y, z}, {0.0f, 1.0f}, {0, -1.0f, 0}},
        {{x, -y, -z}, {0.0f, 0.0f}, {0, -1.0f, 0}}};

    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0,
        4, 6, 5, 7, 6, 4,
        8, 9, 10, 10, 11, 8,
        12, 14, 13, 15, 14, 12,
        16, 17, 18, 18, 19, 16,
        20, 22, 21, 23, 22, 20};

    return Model::createFromData(vertices, indices);
}

std::shared_ptr<Model> Primitives::createSphere(float radius, unsigned int rings, unsigned int segments)
{
    std::vector<wne::VertexTextured> vertices;
    std::vector<uint16_t> indices;

    for (unsigned int y = 0; y <= rings; ++y)
    {
        for (unsigned int x = 0; x <= segments; ++x)
        {
            float xSegment = (float)x / (float)segments;
            float ySegment = (float)y / (float)rings;
            float xPos = radius * cos(xSegment * PI2) * sin(ySegment * PI);
            float yPos = radius * cos(ySegment * PI);
            float zPos = radius * sin(xSegment * PI2) * sin(ySegment * PI);

            Vector3 normal = normalize(Vector3(xPos, yPos, zPos));
            vertices.push_back({{xPos, yPos, zPos}, {xSegment, ySegment}, {normal.x, normal.y, normal.z}});
        }
    }

    for (unsigned int y = 0; y < rings; ++y)
    {
        for (unsigned int x = 0; x < segments; ++x)
        {
            unsigned int first = (y * (segments + 1)) + x;
            unsigned int second = first + segments + 1;

            indices.emplace_back(first);
            indices.emplace_back(first + 1);
            indices.emplace_back(second);
                
            indices.emplace_back(second);
            indices.emplace_back(first + 1);
            indices.emplace_back(second + 1);
        }
    }

    return Model::createFromData(vertices, indices);
}