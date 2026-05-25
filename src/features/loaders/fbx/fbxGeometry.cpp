#include "features/loaders/fbx/fbxGeometry.h"
#include <iostream>

using namespace wne;

FBXGeometry::FBXGeometry(FBXNode &node)
{
    FBXNode *verticesNode = node.findNode("Vertices");
    FBXNode *polygonVertexIndexNode = node.findNode("PolygonVertexIndex");
    FBXNode *elementUVsNode = node.findNode("LayerElementUV");
    FBXNode *elementNormalsNode = node.findNode("LayerElementNormal");

    id = node.getLong(0);

    if (verticesNode && verticesNode->hasProperties())
        provideVertices(verticesNode->getArrayDouble(0), verticesNode->getElementCount(0));

    if (polygonVertexIndexNode && polygonVertexIndexNode->hasProperties())
        providePolygonIndices(polygonVertexIndexNode->getArrayIntegers(0), polygonVertexIndexNode->getElementCount(0));

    if (elementUVsNode && elementUVsNode->hasProperties())
    {
        FBXNode *UVsNode = elementUVsNode->findNode("UV");
        FBXNode *UVIndicesNode = elementUVsNode->findNode("UVIndex");

        if (UVsNode && UVsNode->hasProperties())
            provideUVsData(UVsNode->getArrayDouble(0), UVsNode->getElementCount(0));

        if (UVIndicesNode && UVIndicesNode->hasProperties())
            provideUVIndices(UVIndicesNode->getArrayIntegers(0), UVIndicesNode->getElementCount(0));
    }

    if (elementNormalsNode)
    {
        FBXNode *normalsNode = elementNormalsNode->findNode("Normals");
        FBXNode *normalIndicesNode = elementNormalsNode->findNode("NormalsIndex");

        if (normalsNode && normalsNode->hasProperties())
            provideNormals(normalsNode->getArrayDouble(0), normalsNode->getElementCount(0));

        if (normalIndicesNode && normalIndicesNode->hasProperties())
            provideNormalIndices(normalIndicesNode->getArrayIntegers(0), normalIndicesNode->getElementCount(0));
    }
}

void FBXGeometry::getData(std::vector<VertexTextured> &vertexTexturedData, std::vector<uint32> &indicesData)
{
    std::vector<Point> listIndicesAccumulator;
    std::vector<Point> listIndices;

    for (uint32 i = 0; i < polygonIndices.size(); i++)
    {
        int32 index = polygonIndices[i];
        int32 uvIndex = (i < UVIndices.size()) ? UVIndices[i] : -1;
        int32 normalIndex = (i < normalIndices.size()) ? normalIndices[i] : -1;

        if (index < 0)
        {
            index = ~index;
            listIndicesAccumulator.push_back({i, index, uvIndex, normalIndex});
            auto list = breakPoints(listIndicesAccumulator);
            listIndices.insert(listIndices.end(), list.begin(), list.end());
            listIndicesAccumulator.clear();
        }
        else
        {
            listIndicesAccumulator.push_back({i, index, uvIndex, normalIndex});
        }
    }

    for (uint32 i = 0; i < listIndices.size(); i++)
    {
        uint32 index = (uint32)listIndices[i].index;
        int32 uvIndex = listIndices[i].uvIndex;
        int32 normalIndex = (uint32)listIndices[i].normalIndex;

        Vector3 vertex = vertices[index];
        Vector2 uv = (uvIndex >= 0) ? UVs[uvIndex] : Vector2(0, 0);
        Vector3 normal = (normalIndex >= 0) ? normals[normalIndex] : Vector3(0, 0, 1.0f);

        uint32 newIndex = getIndexByTrait(vertexTexturedData, vertex, uv, normal);
        indicesData.push_back(newIndex);
    }
}

std::vector<FBXGeometry::Point> FBXGeometry::breakPoints(std::vector<Point> points)
{
    std::vector<FBXGeometry::Point> out;
    for (uint32 i = 0; i < points.size(); i++)
    {
        if (i < 3)
        {
            out.emplace_back(points[i]);
        }
        else
        {
            out.emplace_back(points[0]);
            out.emplace_back(points[i - 1]);
            out.emplace_back(points[i]);
        }
    }
    return out;
}

uint32 FBXGeometry::getIndexByTrait(std::vector<VertexTextured> &vertexTexturedData, Vector3 &vertex, Vector2 &uv, Vector3 &normal)
{
    for (uint32 i = 0; i < vertexTexturedData.size(); i++)
    {
        if (vertexTexturedData[i].pos.x == vertex.x &&
            vertexTexturedData[i].pos.y == vertex.y &&
            vertexTexturedData[i].pos.z == vertex.z &&
            vertexTexturedData[i].uv.x == uv.x &&
            vertexTexturedData[i].uv.y == uv.y &&
            vertexTexturedData[i].normal.x == normal.x &&
            vertexTexturedData[i].normal.y == normal.y &&
            vertexTexturedData[i].normal.z == normal.z)
        {
            return i;
        }
    }
    vertexTexturedData.push_back({vertex, uv, normal});
    return vertexTexturedData.size() - 1;
}

void FBXGeometry::provideVertices(double *list, uint64 countOfDoubles)
{
    vertices.clear();

    uint32 vertexAmount = countOfDoubles / 3;
    for (uint32 i = 0; i < vertexAmount; i++)
    {
        uint32 s = i * 3;
        vertices.push_back({(float)list[s], (float)list[s + 1], (float)list[s + 2]});
    }
}

void FBXGeometry::providePolygonIndices(int32 *list, uint64 countOfIndicies)
{
    polygonIndices.resize(countOfIndicies);
    memcpy(polygonIndices.data(), list, countOfIndicies * sizeof(int32));
}

void FBXGeometry::provideUVsData(double *list, uint64 countOfDoubles)
{
    UVs.clear();

    uint32 indexAmount = countOfDoubles / 2;
    for (uint32 i = 0; i < indexAmount; i++)
    {
        uint32 s = i * 2;
        UVs.push_back({(float)list[s], (float)list[s + 1]});
    }
}

void FBXGeometry::provideUVIndices(int32 *list, uint64 countOfIndicies)
{
    UVIndices.resize(countOfIndicies);
    memcpy(UVIndices.data(), list, countOfIndicies * sizeof(int32));
}

void FBXGeometry::provideNormals(double *list, uint64 countOfDoubles)
{
    normals.clear();

    uint32 normalsAmount = countOfDoubles / 3;
    for (uint32 i = 0; i < normalsAmount; i++)
    {
        uint32 s = i * 3;
        normals.push_back(normalize(Vector3((float)list[s], (float)list[s + 1], (float)list[s + 2])));
    }
}

void FBXGeometry::provideNormalIndices(int32 *list, uint64 countOfIndicies)
{
    normalIndices.resize(countOfIndicies);
    memcpy(normalIndices.data(), list, countOfIndicies * sizeof(int32));
}