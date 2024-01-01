#include <glad/glad.h>
#include <utility>
#include "vivid/core/Mesh.h"
#include "vivid/utils/GlmUtils.h"

namespace vivid {

Mesh::Mesh(GeometryPtr geometry, MaterialPtr material, int renderOrder)
    : Object3D(), geometry_(std::move(geometry)), material_(std::move(material)), renderOrder_(renderOrder)
{}


void Mesh::Draw(const CameraPtr& cam, const ShaderPtr& shader, int drawMode, bool useMaterial) {
    // Bind ?
    shader->Use();

    if (cam != nullptr) {
        // Set built-in uniforms
        const glm::mat4 modelMatrix = GetModelMatrix();
        const glm::mat4 viewMatrix = cam->GetViewMatrix();
        glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
        glm::mat4 mvp = cam->GetProjectionMatrix() * modelViewMatrix;
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));
        glm::mat3 normalMatrixW = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

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
        if (shader->HasUniform("normalMatrixW")) {
            shader->SetMat3("normalMatrixW", normalMatrixW);
        }
    }

    if (material_ != nullptr && useMaterial) {
        material_->SetUniforms(shader);
    }

//    // Set textures
//    int texUnit = 0;
//    for (auto& pair : textures_) {
//        glActiveTexture(GL_TEXTURE0 + texUnit);
//        glBindTexture(GL_TEXTURE_2D, pair.second->GetHandle());
//        shader->SetInt(pair.first, texUnit);
//        texUnit++;
//    }

    // Draw geometry
    geometry_->Draw(shader, drawMode);

}


glm::mat4 Mesh::GetModelMatrix() const {
    return GlmUtils::eigen2glm(transform_.Matrix());
}



} // namespace vivid
