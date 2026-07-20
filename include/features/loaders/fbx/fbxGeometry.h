#pragma once
#include "fbxNode.h"
#include "core/math.h"
#include <vector>

namespace wne
{
    class FBXDeformer;

    class FBXGeometry
    {
    public:
        struct RetargetVertexIndex
        {
            int32 from;
            int32 to;
        };

        FBXGeometry(FBXNode &node);
        void getData(std::vector<VertexTextured> &vertexTexturedData, std::vector<uint32> &indicesData);

        inline uint64 getId()
        {
            return id;
        }

        inline void addDeformer(FBXDeformer *deformer)
        {
            deformers.push_back(deformer);
        }

        inline const std::vector<FBXDeformer *> &getDeformers()
        {
            return deformers;
        }

        inline bool isEmpty()
        {
            return vertices.size() == 0;
        }

        inline const std::vector<RetargetVertexIndex> &getRetargetVertexList()
        {
            return retargetVertexList;
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
        uint32 getIndexByTrait(std::vector<VertexTextured> &vertexTexturedData, Vector3 &vertex, Vector2 &uv, Vector3 &normal, int oldIndex);

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
        std::vector<RetargetVertexIndex> retargetVertexList;

        std::vector<FBXDeformer *> deformers;
    };
};
