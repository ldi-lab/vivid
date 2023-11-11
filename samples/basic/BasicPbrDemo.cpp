#include <iostream>
#include "vivid/Application.h"
#include "vivid/core/Mesh.h"
#include "vivid/core/Camera.h"
#include "vivid/core/Shader.h"
#include "vivid/OrbitControls.h"
#include "vivid/utils/GlmUtils.h"
#include "vivid/utils/IOUtil.h"
#include "vivid/primitives/SphereGeometry.h"
#include "vivid/extras/ShaderImpl.h"
#include <glm/gtc/matrix_transform.hpp>

namespace vivid {
class BasicPbrDemo : public Application {
public:
    BasicPbrDemo() : Application(800, 600, "PBR demo") {
        glEnable(GL_DEPTH_TEST);

        SetWindowResizable(false);

        // Load shader
        shader_ = ShaderImpl::LoadShader("./shaders/BasicPbr.vert", "./shaders/BasicPbr.frag");

        // Create airplane
        auto sphereGeo = std::make_shared<SphereGeometry>(0.5f, 64, 64);
        sphere_ = std::make_shared<Mesh>(sphereGeo);

        // Camera
        Eigen::Vector3d lookAtTarget(0, 0, 0);
        camera_ = std::make_shared<Camera>(45.0);
        glm::mat4 view_mat = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(lookAtTarget.x(), lookAtTarget.y(), lookAtTarget.z()), glm::vec3(0, 1, 0));
        Eigen::Matrix4d Tcw = vivid::GlmUtils::glm2eigen<double>(view_mat);
        camera_->SetTransform(Transform(Tcw.inverse()));

        controls_ = std::make_shared<OrbitControls>(window_, camera_, lookAtTarget, UpDir::Y);
    }

    void Render() override {
        controls_->Update();

        // Render to the screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.75f, 0.9f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind shader
        shader_->Use();

        // Set camera position uniform
        Eigen::Vector3d camPos = camera_->GetTransform().Position();
        shader_->SetVec3("uCamPosW", glm::vec3(0, 0, 3));

        // Set light uniforms
        shader_->SetVec3("uLightPositions[0]", glm::vec3(2.0, 2.0, 3.0));
        shader_->SetVec3("uLightColors[0]", glm::vec3(150.0));
        shader_->SetInt("uLightCount", 1);

        // Set material uniforms
//        shader_->SetBool("uHasBaseColorMap", true);
        shader_->SetVec3("uBaseColor", baseColor_);
        shader_->SetFloat("uMetalness", metalness_);
        shader_->SetFloat("uRoughness", roughness_);

        // Switch diffuse/specular on/off for debug purpose
        shader_->SetFloat("uDiffuseCoef", showDiffuse_ ? 1.0f : 0.0f);
        shader_->SetFloat("uSpecularCoef", showSpecular_ ? 1.0f : 0.0f);

        // draw
        sphere_->Draw(camera_, shader_);

        // UI
        ui_.NewFrame();

        ImGui::SetNextWindowSize({300, 180});
        ImGui::Begin("", 0, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::ColorEdit3("BaseColor", (float*)&baseColor_);
        ImGui::SliderFloat("Metalness", &metalness_, 0.0, 1.0);
        ImGui::SliderFloat("Roughness", &roughness_, 0.0, 1.0);
        ImGui::BulletText("Diffuse: ");
        ImGui::SameLine();
        ImGui::SetCursorPosX(120);
        ImGui::ToggleButton("diffuse-switch", &showDiffuse_, 14.0f);
        ImGui::BulletText("Specular: ");
        ImGui::SameLine();
        ImGui::SetCursorPosX(120);
        ImGui::ToggleButton("specular-switch", &showSpecular_, 14.0f);
        ImGui::End();

        ui_.Render();

    }

private:
    ShaderPtr shader_;

    MeshPtr sphere_;

    CameraPtr camera_;

    std::shared_ptr<OrbitControls> controls_;

    // material properties
    glm::vec3 baseColor_ = glm::vec3(1);
    float metalness_ = 0.5f;
    float roughness_ = 0.5f;
    bool showDiffuse_ = true;
    bool showSpecular_ = true;

};
} // namespace vivid


int main() {
    vivid::BasicPbrDemo app;
    app.Run();

    return 0;
}