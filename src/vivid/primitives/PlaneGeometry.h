#pragma once

#include <iostream>
#include <memory>
#include "vivid/core/Attribute.h"
#include "vivid/core/Geometry.h"

namespace vivid {

class PlaneGeometry : public Geometry {
public:
    PlaneGeometry(float width = 1.f, float height = 1.f, int widthSegments = 1, int heightSegments = 1)
        : Geometry(), width_(width), height_(height), widthSegments_(widthSegments), heightSegments_(heightSegments)
    {
        std::vector<float> positions, normals, uvs;
        std::vector<unsigned int> indices;
        BuildPlane(positions, normals, uvs, indices, width, height, 0, widthSegments, heightSegments);
        AddAttribute(std::make_shared<Attribute>(AttributeType::Position, 3, false, &positions));
        AddAttribute(std::make_shared<Attribute>(AttributeType::Normal, 3, true, &normals));
        AddAttribute(std::make_shared<Attribute>(AttributeType::TexCoord0, 2, false, &uvs));
        SetIndex(indices);
    }

    static void BuildPlane(std::vector<float> &positions, std::vector<float> &normals, std::vector<float> &uvs, std::vector<unsigned int> &indices,
                                     float width, float height, float depth, int wSegs, int hSegs,
                                     int u = 0, int v = 1, int w = 2, int uDir = 1, int vDir = -1)
    {
        const float segW = width / float(wSegs);   // segment width
        const float segH = height / float(hSegs);  // segment height

        const int io = static_cast<int>(positions.size()) / 3;
        float pos[3], normal[3];

        for (int iy = 0; iy <= hSegs; iy++) {
            const float y = float(iy) * segH - height / 2;
            for (int ix = 0; ix <= wSegs; ix++) {
                const float x = float(ix) * segW - width / 2;
                pos[u] = x * float(uDir);
                pos[v] = y * float(vDir);
                pos[w] = depth / 2;
                normal[u] = 0;
                normal[v] = 0;
                normal[w] = depth > 0 ? 1 : -1;
                positions.push_back(pos[0]);
                positions.push_back(pos[1]);
                positions.push_back(pos[2]);
                normals.push_back(normal[0]);
                normals.push_back(normal[1]);
                normals.push_back(normal[2]);
                uvs.push_back(float(ix) / float(wSegs));
                uvs.push_back(1.0f - float(iy) / float(hSegs));

                // indices
                if (iy < hSegs && ix < wSegs) {
                    const int a = io + ix + (wSegs + 1) * iy;
                    const int b = io + ix + (wSegs + 1) * (iy + 1);
                    const int c = io + (ix + 1) + (wSegs + 1) * (iy + 1);
                    const int d = io + (ix + 1) + (wSegs + 1) * iy;
                    indices.push_back(a);
                    indices.push_back(b);
                    indices.push_back(d);
                    indices.push_back(b);
                    indices.push_back(c);
                    indices.push_back(d);
                }
            }
        }
    }

private:
    float width_;
    float height_;
    int widthSegments_;
    int heightSegments_;
};

} //namespace vivid