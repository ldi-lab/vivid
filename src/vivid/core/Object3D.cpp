#include "vivid/core/Object3D.h"

namespace vivid {

Object3D::Object3D() {
    name_ = "";
    parent_ = nullptr;
    children_.clear();
    transform_.SetIdentity();
    localTransform_.SetIdentity();
}


void Object3D::AddChild(const std::shared_ptr<Object3D> &child) {
    child->SetParent(this);
    children_.push_back(child);
}


void Object3D::LookAt(const Eigen::Vector3d &target, const Eigen::Vector3d &up) {
    Eigen::Vector3d z = (transform_.Position() - target).normalized();
    Eigen::Vector3d x = up.cross(z).normalized();
    Eigen::Vector3d y = z.cross(x).normalized();

    Eigen::Matrix3d R;
    R << x, y, z;

    transform_.SetRotation(R);
}



} // vivid