#pragma once

#include <iostream>
#include <memory>
#include <Eigen/Dense>
#include "vivid/core/Attribute.h"
#include "vivid/core/Geometry.h"

namespace vivid {

class BoneGeometry : public Geometry {
public:
    BoneGeometry(float length, int radialSegments, const Eigen::Vector3f& direction)
            : length_(length), radialSegments_(radialSegments), direction_(direction)
    {
        std::vector<float> positions, normals, uvs;
        std::vector<unsigned int> indices;

        const float a = 0.1f * length;
        const float b = 0.2f * length;

        // === if the direction is +z ===
        // head vertex
        positions.push_back(0);
        positions.push_back(0);
        positions.push_back(0);
        normals.push_back(0);
        normals.push_back(0);
        normals.push_back(-1);
        uvs.push_back(0);
        uvs.push_back(0);

        // middle vertices
        for (int i = 0; i <= radialSegments; i++) {
            float theta = float(i) / float(radialSegments) * EIGEN_PI * 2;
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);
            // position
            positions.push_back(a * sinTheta);
            positions.push_back(a * cosTheta);
            positions.push_back(b);
            // normal
            normals.push_back(sinTheta);
            normals.push_back(cosTheta);
            normals.push_back(0);
            // uv
            uvs.push_back(float(i) / float(radialSegments));
            uvs.push_back(0);
        }

        // tail vertex
        positions.push_back(0);
        positions.push_back(0);
        positions.push_back(length);
        normals.push_back(0);
        normals.push_back(0);
        normals.push_back(1);
        uvs.push_back(1);
        uvs.push_back(1);

        // Now rotate the vertices


        // indices
        const unsigned int lastIndex = positions.size() / 3 - 1;
        for (int i = 0; i < radialSegments; i++) {
            indices.push_back(0);
            indices.push_back(i + 1);
            indices.push_back(i + 2);
            indices.push_back(lastIndex);
            indices.push_back(i+1);
            indices.push_back(i+2);
        }

        AddAttribute(std::make_shared<Attribute>(AttributeType::Position, 3, false, &positions));
        AddAttribute(std::make_shared<Attribute>(AttributeType::Normal, 3, true, &normals));
        AddAttribute(std::make_shared<Attribute>(AttributeType::TexCoord0, 2, false, &uvs));
        SetIndex(indices);
    }

private:
    float length_;
    int radialSegments_;
    Eigen::Vector3f direction_;

};

} //namespace vivid