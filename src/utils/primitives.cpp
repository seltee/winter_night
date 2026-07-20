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
        {0, xAxis * -halfSize + zAxis * -halfSize, {1.0f, 1.0f}, normal},
        {1, xAxis * halfSize + zAxis * -halfSize, {0.0f, 1.0f}, normal},
        {2, xAxis * halfSize + zAxis * halfSize, {0.0f, 0.0f}, normal},
        {3, xAxis * -halfSize + zAxis * halfSize, {1.0f, 0.0f}, normal}};

    const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

    return Model::createFromData(vertices, indices);
}

std::shared_ptr<Model> Primitives::createBox(float size)
{
    return createBox(Vector3(size, size, size));
}

std::shared_ptr<Model> Primitives::createBox(const Vector3 &size)
{
    float x = size.x / 2.0f;
    float y = size.y / 2.0f;
    float z = size.z / 2.0f;

    const std::vector<wne::VertexTextured> vertices = {
        {0, {x, y, -z}, {1.0f, 0.0f}, {1.0f, 0, 0}},
        {1, {x, y, z}, {1.0f, 1.0f}, {1.0f, 0, 0}},
        {2, {x, -y, z}, {0.0f, 1.0f}, {1.0f, 0, 0}},
        {3, {x, -y, -z}, {0.0f, 0.0f}, {1.0f, 0, 0}},

        {4, {-x, y, -z}, {0.0f, 0.0f}, {-1.0f, 0, 0}},
        {5, {-x, y, z}, {0.0f, 1.0f}, {-1.0f, 0, 0}},
        {6, {-x, -y, z}, {1.0f, 1.0f}, {-1.0f, 0, 0}},
        {7, {-x, -y, -z}, {1.0f, 0.0f}, {-1.0f, 0, 0}},

        {8, {-x, -y, -z}, {0.0f, 0.0f}, {0, 0, -1.0f}},
        {9, {-x, y, -z}, {0.0f, 1.0f}, {0, 0, -1.0f}},
        {10, {x, y, -z}, {1.0f, 1.0f}, {0, 0, -1.0f}},
        {11, {x, -y, -z}, {1.0f, 0.0f}, {0, 0, -1.0f}},

        {12, {-x, -y, z}, {1.0f, 0.0f}, {0, 0, 1.0f}},
        {13, {-x, y, z}, {1.0f, 1.0f}, {0, 0, 1.0f}},
        {14, {x, y, z}, {0.0f, 1.0f}, {0, 0, 1.0f}},
        {15, {x, -y, z}, {0.0f, 0.0f}, {0, 0, 1.0f}},

        {16, {-x, y, -z}, {1.0f, 0.0f}, {0, 1.0f, 0}},
        {17, {-x, y, z}, {1.0f, 1.0f}, {0, 1.0f, 0}},
        {18, {x, y, z}, {0.0f, 1.0f}, {0, 1.0f, 0}},
        {19, {x, y, -z}, {0.0f, 0.0f}, {0, 1.0f, 0}},

        {20, {-x, -y, -z}, {1.0f, 0.0f}, {0, -1.0f, 0}},
        {21, {-x, -y, z}, {1.0f, 1.0f}, {0, -1.0f, 0}},
        {22, {x, -y, z}, {0.0f, 1.0f}, {0, -1.0f, 0}},
        {23, {x, -y, -z}, {0.0f, 0.0f}, {0, -1.0f, 0}}};

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
            vertices.push_back({static_cast<uint32>(vertices.size()), {xPos, yPos, zPos}, {xSegment, ySegment}, {normal.x, normal.y, normal.z}});
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