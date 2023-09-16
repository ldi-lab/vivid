#pragma once

#include <iostream>
#include <memory>
#include <Eigen/Dense>
#include "vivid/core/Attribute.h"
#include "vivid/core/Geometry.h"

namespace vivid {

class SphereGeometry : public Geometry {
public:
    SphereGeometry(float radius = 0.5f, int widthSegments = 16, int heightSegments = 8,
                   float phiStart = 0, float phiLength = EIGEN_PI * 2,
                   float thetaStart = 0, float thetaLength = EIGEN_PI)
           : Geometry(), radius_(radius), widthSegments_(widthSegments), heightSegments_(heightSegments),
             phiStart_(phiStart), phiLength_(phiLength), thetaStart_(thetaStart), thetaLength_(thetaLength)
    {
        std::vector<float> positions, normals, uvs;
        std::vector<unsigned int> indices;

        std::vector<std::vector<unsigned int>> grids;
        grids.reserve(heightSegments_ + 1);
        std::vector<unsigned int> vRow;
        vRow.reserve(widthSegments_ + 1);

        float thetaEnd = thetaStart_ + thetaLength_;
        unsigned int index = 0;
        for (int iy = 0; iy <= heightSegments_; ++iy) {
            vRow.clear();
            float v = float(iy) / (float)heightSegments_;
            for (int ix = 0; ix <= widthSegments_; ++ix) {
                float u = float(ix) / (float)widthSegments_;
                float x = -radius_ * cos(phiStart_ + u * phiLength_) * sin(thetaStart_ + v * thetaLength_);
                float y = radius_ * cos(thetaStart_ + v * thetaLength_);
                float z = radius_ * sin(phiStart_ + u * phiLength_) * sin(thetaStart_ + v * thetaLength_);
                positions.push_back(x);
                positions.push_back(y);
                positions.push_back(z);

                Eigen::Vector3f normal(x, y, z);
                normal.normalize();
                normals.push_back(normal.x());
                normals.push_back(normal.y());
                normals.push_back(normal.z());

                uvs.push_back(u);
                uvs.push_back(1 - v);

                vRow.push_back(index++);
            }
            grids.push_back(vRow);
        }

        // indices
        for (int iy = 0; iy < heightSegments; iy++) {
            for (int ix = 0; ix < widthSegments; ix++) {
                unsigned int a = grids[iy][ix + 1];
                unsigned int b = grids[iy][ix];
                unsigned int c = grids[iy + 1][ix];
                unsigned int d = grids[iy + 1][ix + 1];
                if (iy != 0 || thetaStart_ > 0) {
                    indices.push_back(a);
                    indices.push_back(b);
                    indices.push_back(d);
                }
                if (iy != heightSegments - 1 || thetaEnd < EIGEN_PI) {
                    indices.push_back(b);
                    indices.push_back(c);
                    indices.push_back(d);
                }
            }
        }

        AddAttribute(std::make_shared<Attribute>(AttributeType::Position, 3, false, &positions));
        AddAttribute(std::make_shared<Attribute>(AttributeType::Normal, 3, true, &normals));
        AddAttribute(std::make_shared<Attribute>(AttributeType::TexCoord0, 2, false, &uvs));
        SetIndex(indices);

    }

private:
    float radius_;
    int widthSegments_;
    int heightSegments_;
    float phiStart_;
    float phiLength_;
    float thetaStart_;
    float thetaLength_;

};

} //namespace vivid