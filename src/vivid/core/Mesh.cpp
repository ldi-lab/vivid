#include <glad/glad.h>

#include <utility>
#include "vivid/core/Mesh.h"
#include "vivid/utils/GlmUtils.h"

namespace vivid {

Mesh::Mesh(std::shared_ptr<Geometry> geometry, int drawMode,
           int renderOrder)
           : Object3D(), geometry_(std::move(geometry)), drawMode_(drawMode), renderOrder_(renderOrder)
{
    textures_.clear();
}


void Mesh::AddTexture(const std::string &name, const std::shared_ptr<Texture>& texture) {
    textures_.emplace_back(name, texture);
}


void Mesh::Draw(std::shared_ptr<Camera> cam, std::shared_ptr<Shader> shader) {
    // Bind ?

    if (cam != nullptr) {
        // Set built-in uniforms
        const glm::mat4 modelMatrix = GetModelMatrix();
        const glm::mat4 viewMatrix = cam->GetViewMatrix();
        glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
        glm::mat4 mvp = cam->GetProjectionMatrix() * modelViewMatrix;
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));

        if (shader->HasUniform("modelMatrix")) {
            shader->SetMat4("modelMatrix", modelMatrix);
        }
        if (shader->HasUniform("viewMatrix")) {
            shader->SetMat3("viewMatrix", cam->GetViewMatrix());
        }
        if (shader->HasUniform("modelViewMatrix")) {
            shader->SetMat4("modelViewMatrix", modelViewMatrix);
        }
        if (shader->HasUniform("projectionMatrix")) {
            shader->SetMat4("projectionMatrix", cam->GetProjectionMatrix());
        }
        if (shader->HasUniform("MVP")) {
            shader->SetMat4("MVP", mvp);
        }
        if (shader->HasUniform("normalMatrix")) {
            shader->SetMat3("normalMatrix", normalMatrix);
        }
    }

    // Set textures
    int texUnit = 0;
    for (auto& pair : textures_) {
        glActiveTexture(GL_TEXTURE0 + texUnit);
        glBindTexture(GL_TEXTURE_2D, pair.second->GetHandle());
        shader->SetInt(pair.first, texUnit);
        texUnit++;
    }

    // Draw geometry
    geometry_->Draw(shader, drawMode_);

}


glm::mat4 Mesh::GetModelMatrix() const {
    return GlmUtils::eigen2glm(transform_.Matrix());
}



} // namespace vivid
