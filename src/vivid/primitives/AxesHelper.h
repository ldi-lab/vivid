#pragma once

#include <iostream>
#include <memory>
#include "vivid/core/Attribute.h"
#include "vivid/core/Geometry.h"


namespace vivid {

class AxesHelper : public Geometry {
public:
    explicit AxesHelper(float size = 1.0f)
        : Geometry(), size_(size)
    {
        std::vector<float> positions = {
                0, 0, 0,   size, 0, 0,
                0, 0, 0,   0, size, 0,
                0, 0, 0,   0, 0, size
        };
        std::vector<float> colors = {
                1, 0, 0,   1, 0.6f, 0,
                0, 1, 0,   0.6f, 1, 0,
                0, 0, 1,   0, 0.6f, 1
        };

        AddAttribute(std::make_shared<Attribute>(AttributeType::Position, 3, false, &positions));
        AddAttribute(std::make_shared<Attribute>(AttributeType::Color, 3, false, &colors));
    }


private:
    float size_;

};

} // namespace vivid



