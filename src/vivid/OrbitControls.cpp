#include "OrbitControls.h"
#include <imgui/imgui.h>

#include <utility>

namespace vivid {

double scrollOffsets_ = 0;
Eigen::Vector2d panDeltas_(0, 0);
Eigen::Vector2d rotateDeltas_(0, 0);
OrbitControlState orbitControlState_ = OrbitControlState::IDLE;
double xPos_ = 0;
double yPos_ = 0;

OrbitControls::OrbitControls(GLFWwindow* window, CameraPtr cam, const Eigen::Vector3d& target, UpDir upDir)
    : window_(window), cam_(std::move(cam)), upDir_(upDir)
{
    // Initial values
    targetPosition_ = target;

    // Setup callbacks
    glfwSetMouseButtonCallback(window, OrbitControls::OnMouseButton);
    glfwSetCursorPosCallback(window, OrbitControls::OnMouseMove);
    glfwSetScrollCallback(window, OrbitControls::OnMouseScroll);
}


void OrbitControls::Update() {
    double xCursor, yCursor;
    glfwGetCursorPos(window_, &xCursor, &yCursor);

    // Zoom
    if (scrollOffsets_ != 0) {
        Transform camTf = cam_->GetTransform();
        Eigen::Vector3d vz = (camTf.Rotation() * Eigen::Vector3d(0, 0, 1)).normalized();
        camTf.SetPosition(camTf.Position() + vz * scrollOffsets_ * 0.1);
        cam_->SetTransform(camTf);
        scrollOffsets_ = 0;
    }

    // Pan
    if (orbitControlState_ == OrbitControlState::PAN) {
        double dx = xCursor - xPos_;
        double dy = yCursor - yPos_;
        xPos_ = xCursor;
        yPos_ = yCursor;
        Transform camTf = cam_->GetTransform();
        Eigen::Vector3d vx = (camTf.Rotation() * Eigen::Vector3d(1, 0, 0)).normalized();
        Eigen::Vector3d vy = (camTf.Rotation() * Eigen::Vector3d(0, 1, 0)).normalized();
        Eigen::Vector3d deltaPosition = -vx * dx * 0.01 + vy * dy * 0.01;
        camTf.SetPosition(camTf.Position() + deltaPosition);
        cam_->SetTransform(camTf);
        targetPosition_ += deltaPosition;
    }

    // Rotate
    if (orbitControlState_ == OrbitControlState::ROTATE) {
        double dx = xCursor - xPos_;
        double dy = yCursor - yPos_;
        xPos_ = xCursor;
        yPos_ = yCursor;
        double deltaTheta = -dx * 0.01;
        double deltaPhi = -dy * 0.01;

        const int upIdx = static_cast<int>(upDir_);
        Eigen::Vector3d offset = cam_->GetTransform().Position() - targetPosition_;
        double radius = offset.norm();
        double theta = atan2(offset[(upIdx+2)%3], offset[(upIdx+1)%3]);
        double phi = acos(offset[upIdx] / radius);

        // Avoid phi = 0 or pi, in case that lookAt() fails
        double newPhi = std::min(std::max(0.001, phi + deltaPhi), double(EIGEN_PI) - 0.001);
        double newTheta = theta + deltaTheta;

        Eigen::Vector3d newOffset;
        double sinPhiRadius = sin(newPhi) * radius;
        newOffset[(upIdx+2)%3] = sinPhiRadius * sin(newTheta);
        newOffset[upIdx] = cos(newPhi) * radius;
        newOffset[(upIdx+1)%3] = sinPhiRadius * cos(newTheta);

        Eigen::Vector3d upVector(0, 0, 0);
        upVector[upIdx] = 1;
        cam_->GetTransform().SetPosition(newOffset + targetPosition_);
        cam_->LookAt(targetPosition_, upVector);
    }


}


void OrbitControls::OnMouseButton(GLFWwindow *window, int button, int action, int mods) {
    // (1) ALWAYS forward mouse data to ImGui! This is automatic with default backends. With your own backend:
    ImGuiIO& io = ImGui::GetIO();
    bool down = action == GLFW_PRESS;
    io.AddMouseButtonEvent(button, down);
    if (io.WantCaptureMouse) {
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            if (orbitControlState_ == OrbitControlState::IDLE) {
                orbitControlState_ = OrbitControlState::ROTATE;
                glfwGetCursorPos(window, &xPos_, &yPos_);
            }
        } else if (action == GLFW_RELEASE) {
            if (orbitControlState_ == OrbitControlState::ROTATE) {
                orbitControlState_ = OrbitControlState::IDLE;
            }
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            if (orbitControlState_ == OrbitControlState::IDLE) {
                orbitControlState_ = OrbitControlState::PAN;
                glfwGetCursorPos(window, &xPos_, &yPos_);
            }
        } else if (action == GLFW_RELEASE) {
            if (orbitControlState_ == OrbitControlState::PAN) {
                orbitControlState_ = OrbitControlState::IDLE;
            }
        }
    }

}


void OrbitControls::OnMouseMove(GLFWwindow *window, double xpos, double ypos) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(xpos, ypos);
    if (io.WantCaptureMouse) {
        return;
    }
}


void OrbitControls::OnMouseScroll(GLFWwindow *window, double, double yoffset) {
    scrollOffsets_ += yoffset;
}



} // namespace vivid