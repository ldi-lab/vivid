#pragma once

#include <Eigen/Dense>

namespace vivid {

class Transform {
public:
    Transform();

    explicit Transform(const Eigen::Matrix4d &T);

    void SetIdentity();

    void Set(const Transform& tf);

    void SetMatrix(const Eigen::Matrix4d &T);
    void SetRotation(const Eigen::Matrix3d &R);
    void SetPosition(const Eigen::Vector3d& p);

    Eigen::Matrix4d Matrix() const;
    Eigen::Matrix3d Rotation() const;
    Eigen::Vector3d Position() const;
    Eigen::Quaterniond Quaternion() const;

    Transform Inverse() const;

    Transform operator* (const Transform& otherTf) const;

    void Rotate(const Eigen::Vector3d &axis, double angle);

private:
    void UpdateQuat();

    Eigen::Matrix4d mat_;
    Eigen::Quaterniond q_;
};

} // namespace vivid