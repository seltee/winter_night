#pragma once
#include "fbxNode.h"
#include "core/math.h"
#include <vector>

namespace wne
{
    class FBXGeometry
    {
    public:
        FBXGeometry(FBXNode &node);
        void getData(std::vector<VertexTextured> &vertexTexturedData, std::vector<uint32> &indicesData);

        inline uint64 getId()
        {
            return id;
        }

    protected:
        struct Point
        {
            uint32 number;
            int32 index;
            int32 uvIndex;
            int32 normalIndex;
        };

        std::vector<Point> breakPoints(std::vector<Point> points);
        // adds vertex if doesn't exist
        uint32 getIndexByTrait(std::vector<VertexTextured> &vertexTexturedData, Vector3 &vertex, Vector2 &uv, Vector3 &normal);

        void provideVertices(double *list, uint64 countOfDoubles);
        void providePolygonIndices(int32 *list, uint64 countOfIndicies);
        void provideUVsData(double *list, uint64 countOfDoubles);
        void provideUVIndices(int32 *list, uint64 countOfIndicies);
        void provideNormals(double *list, uint64 countOfDoubles);
        void provideNormalIndices(int32 *list, uint64 countOfIndicies);

        uint64 id;

        std::vector<Vector3> vertices;
        std::vector<int32> polygonIndices;
        std::vector<Vector3> normals;
        std::vector<uint32> normalIndices;
        std::vector<Vector2> UVs;
        std::vector<uint32> UVIndices;
    };
};
