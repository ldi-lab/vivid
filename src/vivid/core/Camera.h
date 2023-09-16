#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "vivid/core/Object3D.h"
#include "vivid/core/Transform.h"

namespace vivid {

enum class CameraType : int {
    Perspective = 0,
    Orthographic = 1
};

class Camera : public Object3D {
public:
    Camera(float fov = 60.f, float ratio = 4.f/3.f, float near = 0.1f, float far = 100.f);

    Camera(float left, float right, float bottom, float top, float near, float far);

    void CalcProjectionMatrix();

    inline glm::mat4 GetProjectionMatrix() { return projMat_; }

    glm::mat4 GetViewMatrix();

    bool IsPerspective() const {
        return type_ == CameraType::Perspective;
    }

    bool IsOrthographic() const {
        return type_ == CameraType::Orthographic;
    }

private:
    int width_;
    int height_;

    // Perspective parameters
    float fov_;
    float ratio_;
    float near_;
    float far_;

    // Orthographic parameters
    float left_, right_, bottom_, top_;

    // Projection matrix
    glm::mat4 projMat_;

    // Camera type
    CameraType type_;
};

using CameraPtr = std::shared_ptr<Camera>;

} // namespace vivid