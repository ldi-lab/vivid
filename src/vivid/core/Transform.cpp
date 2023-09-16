#include "vivid/core/Transform.h"

namespace vivid {

Transform::Transform() {
    SetIdentity();
}

Transform::Transform(const Eigen::Matrix4d &T) {
    SetMatrix(T);
}

void Transform::SetIdentity() {
    mat_.setIdentity();
    q_.setIdentity();
}

void Transform::Set(const Transform &tf) {
    SetMatrix( tf.Matrix() );
}

void Transform::SetMatrix(const Eigen::Matrix4d &T) {
    mat_ = T;
    UpdateQuat();
}

void Transform::SetRotation(const Eigen::Matrix3d &R) {
    mat_.topLeftCorner(3, 3) = R;
    UpdateQuat();
}

void Transform::SetPosition(const Eigen::Vector3d &p) {
    mat_.topRightCorner(3, 1) = p;
}

Eigen::Matrix4d Transform::Matrix() const {
    return mat_;
}

Eigen::Matrix3d Transform::Rotation() const {
    return mat_.topLeftCorner(3, 3);
}

Eigen::Vector3d Transform::Position() const {
    return mat_.topRightCorner(3, 1);
}

Eigen::Quaterniond Transform::Quaternion() const {
    return q_;
}

Transform Transform::Inverse() const {
    return Transform(mat_.inverse());
}

Transform Transform::operator*(const Transform &otherTf) const {
    return Transform(mat_ * otherTf.Matrix());
}

void Transform::UpdateQuat() {
    const Eigen::Matrix3d& R = mat_.topLeftCorner(3, 3);
    q_ = Eigen::Quaterniond(R);
}


void Transform::Rotate(const Eigen::Vector3d &axis, double angle) {
    Eigen::Matrix3d dR = Eigen::AngleAxisd(angle, axis).toRotationMatrix();
    Eigen::Matrix3d newR = Rotation() * dR;
    SetRotation(newR);
}




} // namespace vivid