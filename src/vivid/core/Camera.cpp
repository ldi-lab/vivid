#include <glm/gtc/matrix_transform.hpp>
#include "vivid/core/Camera.h"
#include "vivid/utils/GlmUtils.h"

namespace vivid {

Camera::Camera(float fov, float ratio, float near, float far)
    :Object3D(), fov_(fov), ratio_(ratio), near_(near), far_(far), type_(CameraType::Perspective)
{
    CalcProjectionMatrix();
}

Camera::Camera(float left, float right, float bottom, float top, float near, float far)
    :Object3D(), left_(left), right_(right), bottom_(bottom), top_(top), near_(near), far_(far), type_(CameraType::Orthographic)
{
    CalcProjectionMatrix();
}



void Camera::CalcProjectionMatrix() {
    if (IsPerspective()) {
        projMat_ = glm::perspective(glm::radians(fov_), ratio_, near_, far_);
    } else if (IsOrthographic()) {
        projMat_ = glm::ortho(left_, right_, bottom_, top_, near_, far_);
    }
}


glm::mat4 Camera::GetViewMatrix() {
    Eigen::Matrix4d Tcw = transform_.Matrix().inverse();
    return GlmUtils::eigen2glm(Tcw);
}


} // namespace vivid