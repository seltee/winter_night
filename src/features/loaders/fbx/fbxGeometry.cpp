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
        FBXNode *normals = elementNormalsNode->findNode("Normals");
        if (normals && normals->hasProperties())
            provideNormals(normals->getArrayDouble(0), normals->getElementCount(0));
    }
}

void FBXGeometry::getData(std::vector<VertexTextured> &vertexTexturedData, std::vector<uint32> &indicesData)
{
    std::vector<Point> listIndicesAccumulator;
    std::vector<Point> listIndices;

    for (uint32 i = 0; i < polygonIndicies.size(); i++)
    {
        int32 index = polygonIndicies[i];
        int32 uvIndex = (i < UVIndicies.size()) ? UVIndicies[i] : -1;

        if (index < 0)
        {
            index = ~index;
            listIndicesAccumulator.push_back({i, index, uvIndex});
            auto list = breakPoints(listIndicesAccumulator);
            listIndices.insert(listIndices.end(), list.begin(), list.end());
            listIndicesAccumulator.clear();
        }
        else
        {
            listIndicesAccumulator.push_back({i, index, uvIndex});
        }
    }

    for (uint32 i = 0; i < listIndices.size(); i++)
    {
        uint32 index = (uint32)listIndices[i].index;
        int32 uvIndex = listIndices[i].uvIndex;
        Vector3 vertex = vertices[index];
        Vector2 uv = (uvIndex >= 0) ? UVs[uvIndex] : Vector2(0, 0);

        uint32 newIndex = getIndexByTrait(vertexTexturedData, vertex, uv);
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

uint32 FBXGeometry::getIndexByTrait(std::vector<VertexTextured> &vertexTexturedData, Vector3 &vertex, Vector2 &uv)
{
    for (uint32 i = 0; i < vertexTexturedData.size(); i++)
    {
        if (vertexTexturedData[i].pos.x == vertex.x &&
            vertexTexturedData[i].pos.y == vertex.y &&
            vertexTexturedData[i].pos.z == vertex.z &&
            vertexTexturedData[i].uv.x == uv.x &&
            vertexTexturedData[i].uv.y == uv.y)
        {
            return i;
        }
    }
    vertexTexturedData.push_back({vertex, uv});
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
    polygonIndicies.resize(countOfIndicies);
    memcpy(polygonIndicies.data(), list, countOfIndicies * sizeof(int32));
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
    UVIndicies.resize(countOfIndicies);
    memcpy(UVIndicies.data(), list, countOfIndicies * sizeof(int32));
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