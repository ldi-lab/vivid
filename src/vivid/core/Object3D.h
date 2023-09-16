#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include "vivid/core/Transform.h"

namespace vivid {

class Object3D {
public:
    Object3D();

    inline void SetName(const std::string& name) { name_ = name; }
    inline std::string GetName() { return name_; }

    inline void SetParent(Object3D* parent) { parent_ = parent; }
    inline Object3D* GetParent() { return parent_; }

    inline void SetTransform(const Transform& tf) { transform_ = tf; }
    inline Transform& GetTransform() { return transform_; }

    void AddChild(const std::shared_ptr<Object3D> &child);
    inline const std::vector<std::shared_ptr<Object3D> > &GetChildren() { return children_; }

    void LookAt(const Eigen::Vector3d& target, const Eigen::Vector3d& up = Eigen::Vector3d::UnitY());

protected:
    // automatically assigned, unique within process.
    int id_;

    std::string name_;

    Object3D* parent_;

    std::vector<std::shared_ptr<Object3D>> children_;

    // Transform in world frame (T_world_this)
    Transform transform_;

    // Transform in local frame (T_parent_this)
    Transform localTransform_;

    // Rendering properties
    bool isVisible_ = true;
    int renderOrder_ = 0;
    bool castShadow_ = false;
    bool receiveShadow_ = false;
    bool frustumCulled_ = true;

};

} // namespace vivid