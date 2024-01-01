#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "vivid/core/Object3D.h"
#include "vivid/core/Geometry.h"
#include "vivid/core/Camera.h"
#include "vivid/core/Material.h"
#include "vivid/core/Shader.h"


namespace vivid {

class Mesh : public Object3D {
public:
    Mesh(GeometryPtr geometry,
         MaterialPtr material,
         int renderOrder = 0);

    void Draw(const CameraPtr& cam, const ShaderPtr& shader,
              int drawMode = GL_TRIANGLES, bool useMaterial = true);

    void SetMaterial(const MaterialPtr& material) {
        material_ = material;
    }

    MaterialPtr GetMaterial() const {
        return material_;
    }

    glm::mat4 GetModelMatrix() const;

private:
    GeometryPtr geometry_;
    MaterialPtr material_;
    int renderOrder_;
};

using MeshPtr = std::shared_ptr<Mesh>;

} // namespace vivid