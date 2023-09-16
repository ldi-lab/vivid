#pragma once

#include <iostream>
#include <memory>
#include "vivid/core/Attribute.h"
#include "vivid/core/Geometry.h"
#include "vivid/primitives/PlaneGeometry.h"


namespace vivid {

class BoxGeometry : public Geometry {
public:
    BoxGeometry(float width = 1.f, float height = 1.f, float depth = 1.f,
                          int widthSegments = 1, int heightSegments = 1, int depthSegments = 1)
           : Geometry(), width_(width), height_(height), depth_(depth),
             widthSegments_(widthSegments), heightSegments_(heightSegments), depthSegments_(depthSegments)
    {
        std::vector<float> positions, normals, uvs;
        std::vector<unsigned int> indices;

        // left , right
        PlaneGeometry::BuildPlane(positions, normals, uvs, indices, depth, height, width, depthSegments, heightSegments, 2, 1, 0, -1, -1);
        PlaneGeometry::BuildPlane(positions, normals, uvs, indices, depth, height, -width, depthSegments, heightSegments, 2, 1, 0, 1, -1);
        // top, bottom
        PlaneGeometry::BuildPlane(positions, normals, uvs, indices, width, depth, height, widthSegments, depthSegments, 0, 2, 1, 1, 1);
        PlaneGeometry::BuildPlane(positions, normals, uvs, indices, width, depth, -height, widthSegments, depthSegments, 0, 2, 1, 1, -1);
        // front, back
        PlaneGeometry::BuildPlane(positions, normals, uvs, indices, width, height, depth, widthSegments, heightSegments, 0, 1, 2, 1, -1);
        PlaneGeometry::BuildPlane(positions, normals, uvs, indices, width, height, -depth, widthSegments, heightSegments, 0, 1, 2, -1, -1);

        AddAttribute(std::make_shared<Attribute>(AttributeType::Position, 3, false, &positions));
        AddAttribute(std::make_shared<Attribute>(AttributeType::Normal, 3, true, &normals));
        AddAttribute(std::make_shared<Attribute>(AttributeType::TexCoord0, 2, false, &uvs));
        SetIndex(indices);
    }

private:
    float width_, height_, depth_;
    int widthSegments_, heightSegments_, depthSegments_;

};

} //namespace vivid