#pragma once

#include <iostream>
#include <Eigen/Dense>

namespace vivid {

enum LightType : int {
    DIRECTIONAL = 0,
    POINT,
    SPOT
};

class Light {
public:
    Light(LightType type,
          const Eigen::Vector3d& position,
          const Eigen::Vector3d& color = Eigen::Vector3d::Ones(),
          double intensity = 1.0,
          double distance = 0.0,
          double angle = 0.0);

};

using LightPtr = std::shared_ptr<Light>;

} // namespace vivid