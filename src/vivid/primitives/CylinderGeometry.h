#pragma once

#include <iostream>
#include <memory>
#include <Eigen/Dense>
#include "vivid/core/Attribute.h"
#include "vivid/core/Geometry.h"

namespace vivid {

class CylinderGeometry : public Geometry {
public:
    CylinderGeometry(float radiusTop, float radiusBottom, float height,
                     int radialSegments, int heightSegments, bool openEnded = false,
                     float thetaStart = 0, float thetaLength = 2 * EIGEN_PI)
            : Geometry(), radiusTop_(radiusTop), radiusBottom_(radiusBottom), height_(height),
            radialSegments_(radialSegments), heightSegments_(heightSegments),
            openEnded_(openEnded), thetaStart_(thetaStart), thetaLength_(thetaLength)
    {
        std::vector<float> positions, normals, uvs;
        std::vector<unsigned int> indices;

        GenerateTorso(positions, normals, uvs, indices);
        if (!openEnded) {
            GenerateCap(positions, normals, uvs, indices, true);
            GenerateCap(positions, normals, uvs, indices, false);
        }

        AddAttribute(std::make_shared<Attribute>(AttributeType::Position, 3, false, &positions));
        AddAttribute(std::make_shared<Attribute>(AttributeType::Normal, 3, true, &normals));
        AddAttribute(std::make_shared<Attribute>(AttributeType::TexCoord0, 2, false, &uvs));
        SetIndex(indices);

    }


    void GenerateTorso(std::vector<float>& positions, std::vector<float>& normals, std::vector<float>& uvs, std::vector<unsigned int>& indices) {
        std::vector<std::vector<unsigned int>> grids;
        grids.reserve(heightSegments_ + 1);
        std::vector<unsigned int> vRow;
        vRow.reserve(radialSegments_ + 1);
        unsigned int index = 0;

        // this will be used calculate the normal
        const float slope = (radiusBottom_ - radiusTop_) / height_;
        const float halfHeight = height_ / 2;
        for (int j = 0; j <= heightSegments_; ++j) {
            vRow.clear();

            const float v = float(j) / float(heightSegments_);
            // calculate the radius of current rao
            const float radius = v * (radiusBottom_ - radiusTop_) + radiusTop_;

            for (int i = 0; i <= radialSegments_; ++i) {
                const float u = float(i) / float(radialSegments_);
                const float theta = u * thetaLength_ + thetaStart_;
                float sinTheta = std::sin(theta);
                float cosTheta = std::cos(theta);
                // vertex position
                positions.push_back(radius * sinTheta);
                positions.push_back(- v * height_ + halfHeight);
                positions.push_back(radius * cosTheta);
                // normal
                Eigen::Vector3f normal(sinTheta, slope, cosTheta);
                normal.normalize();
                normals.push_back(normal.x());
                normals.push_back(normal.y());
                normals.push_back(normal.z());
                // uv
                uvs.push_back(u);
                uvs.push_back(1-v);
                // index row
                vRow.push_back(index++);
            }
            grids.push_back(vRow);
        }

        // generate indices
        for ( int x = 0; x < radialSegments_; x ++ ) {
            for ( int y = 0; y < heightSegments_; y ++ ) {
                // we use the index array to access the correct indices
                auto a = grids[ y ][ x ];
                auto b = grids[ y + 1 ][ x ];
                auto c = grids[ y + 1 ][ x + 1 ];
                auto d = grids[ y ][ x + 1 ];

                // faces
                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(d);
                indices.push_back(b);
                indices.push_back(c);
                indices.push_back(d);
            }
        }
    }


    void GenerateCap(std::vector<float>& positions, std::vector<float>& normals,
                     std::vector<float>& uvs, std::vector<unsigned int>& indices, bool isTop) {
        const float halfHeight = height_ / 2;
        const float radius =  isTop ? radiusTop_ : radiusBottom_;
        const float sign = isTop ? 1 : - 1;

        // save the index of the first center index
        const unsigned int centerIndexStart = positions.size() / 3;
        unsigned int index = centerIndexStart;

        // first we generate the center vertex data of the cap.
        // because the geometry needs one set of uvs per face,
        // we must generate a center vertex per face/segment
        for (int i = 0; i <= radialSegments_; ++i) {
            // position
            positions.push_back(0);
            positions.push_back(halfHeight * sign);
            positions.push_back(0);
            // normal
            normals.push_back(0);
            normals.push_back(sign);
            normals.push_back(0);
            // uv
            uvs.push_back(0.5f);
            uvs.push_back(0.5f);
            // index
            index++;
        }

        // save the index of the last center index
        const unsigned int centerIndexEnd = index;

        // now we generate the surrounding vertices, normals, uvs
        for (int x = 0; x <= radialSegments_; ++x) {
            const float u = float(x) / float(radialSegments_);
            const float theta = u * thetaLength_ + thetaStart_;
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);
            // vertex position
            positions.push_back(radius * sinTheta);
            positions.push_back(halfHeight * sign);
            positions.push_back(radius * cosTheta);
            // normal
            normals.push_back(0);
            normals.push_back(sign);
            normals.push_back(0);
            // uv
            uvs.push_back(0.5f + 0.5f * cosTheta);
            uvs.push_back(0.5f + 0.5f * sinTheta * sign);
            // index
            index++;
        }

        // generate indices
        for ( int x = 0; x < radialSegments_; x ++ ) {
            const unsigned int c = centerIndexStart + x;
            const unsigned int i = centerIndexEnd + x;
            if (isTop) {
                // face top
                indices.push_back(i);
                indices.push_back(i + 1);
                indices.push_back(c);
            } else {
                // face bottom
                indices.push_back(i + 1);
                indices.push_back(i);
                indices.push_back(c);
            }
        }
    }


private:
    float radiusTop_;
    float radiusBottom_;
    float height_;
    int radialSegments_;
    int heightSegments_;
    bool openEnded_;
    float thetaStart_;
    float thetaLength_;
};

} //namespace vivid