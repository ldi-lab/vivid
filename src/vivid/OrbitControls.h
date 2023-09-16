#pragma once

#include <iostream>
#include <GLFW/glfw3.h>
#include "vivid/core/Camera.h"

namespace vivid {

enum class OrbitControlState {
    IDLE = 0,
    ROTATE,
    PAN
};

enum class UpDir : int {
    X = 0,
    Y = 1,
    Z = 2
};


class OrbitControls {
public:
    explicit OrbitControls(GLFWwindow* window, CameraPtr cam, const Eigen::Vector3d& target = Eigen::Vector3d::Zero(), UpDir = UpDir::Z);

    void Update();

private:
    static void OnMouseButton(GLFWwindow *window, int button, int action, int mods);

    static void OnMouseMove(GLFWwindow* window, double xpos, double ypos);

    static void OnMouseScroll(GLFWwindow* window, double, double yoffset);

private:
    GLFWwindow* window_;
    CameraPtr cam_;

    Eigen::Vector3d targetPosition_;

    UpDir upDir_;

};

} // namespace vivid