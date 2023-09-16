#pragma once

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "vivid/core/Object3D.h"
#include "vivid/core/Geometry.h"
#include "vivid/core/Camera.h"
#include "vivid/core/Texture.h"


namespace vivid {

class Mesh : public Object3D {
public:
    Mesh(std::shared_ptr<Geometry> geometry,
             int drawMode = GL_TRIANGLES,
             int renderOrder = 0);

    void AddTexture(const std::string& name, const std::shared_ptr<Texture>& texture);

    void Draw(std::shared_ptr<Camera> cam, std::shared_ptr<Shader> shader);

    glm::mat4 GetModelMatrix() const;

private:
    std::shared_ptr<Geometry> geometry_;
    int drawMode_;
    int renderOrder_;

    std::vector<std::pair<std::string, std::shared_ptr<Texture> > > textures_;
};

using MeshPtr = std::shared_ptr<Mesh>;

} // namespace vivid