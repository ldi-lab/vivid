#pragma once

#include <iostream>
#include <memory>
#include "vivid/core/Attribute.h"
#include "vivid/core/Geometry.h"


namespace vivid {

class GridHelper : public Geometry {
public:
    explicit GridHelper(float gridCellSize = 1, int divisions = 10, int majorUnitFrequency = 2,
                        const glm::vec3 &majorColor = glm::vec3(0.5), const glm::vec3 &minorColor = glm::vec3(0.7))
            : Geometry(), gridCellSize_(gridCellSize), divisions_(divisions),
              majorUnitFrequency_(majorUnitFrequency), majorColor_(majorColor), minorColor_(minorColor)
    {
        std::vector<float> positions, colors;
        positions.reserve((divisions+1) * 4 * 3);
        colors.reserve((divisions+1) * 4 * 3);
        float halfSize = gridCellSize * float(divisions) / 2.0f;
        for (int i = 0; i <= divisions; ++i) {
            float p = -halfSize + float(i) * gridCellSize;
            positions.push_back(p);
            positions.push_back(0);
            positions.push_back(-halfSize);
            positions.push_back(p);
            positions.push_back(0);
            positions.push_back(halfSize);
            positions.push_back(-halfSize);
            positions.push_back(0);
            positions.push_back(p);
            positions.push_back(halfSize);
            positions.push_back(0);
            positions.push_back(p);

            auto& color = (i % majorUnitFrequency == 0) ? majorColor_ : minorColor_;

            for (int k = 0; k < 4; k++) {
                colors.push_back(color.x);
                colors.push_back(color.y);
                colors.push_back(color.z);
            }
        }

        AddAttribute(std::make_shared<Attribute>(AttributeType::Position, 3, false, &positions));
        AddAttribute(std::make_shared<Attribute>(AttributeType::Color, 3, false, &colors));
    }

private:
    float gridCellSize_;
    int divisions_;
    int majorUnitFrequency_;
    glm::vec3 majorColor_;
    glm::vec3 minorColor_;

};

} // namespace vivid



