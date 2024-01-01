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
#include "vivid/extras/MaterialImpl.h"
#include <glm/gtc/matrix_transform.hpp>

namespace vivid {
class BasicPbrDemo : public Application {
public:
    BasicPbrDemo() : Application(800, 600, "PBR demo") {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        SetWindowResizable(false);

        // Load shader
        shader_ = ShaderImpl::GetPBRShader();

        // IBL textures
        auto brdfLutTexture = IOUtil::LoadTexture("./models/car/lut.png");
        auto envIrradianceTexture = IOUtil::LoadTexture("./models/car/waterfall-diffuse-RGBM.png");
        auto envSpecularTexture = IOUtil::LoadTexture("./models/car/waterfall-specular-RGBM.png");

        // Load car
        LoadExterior();
        LoadInterior();

        shader_->Use();
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, brdfLutTexture->GetHandle());
        shader_->SetInt("uBrdfLutMap", 5);

        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, envIrradianceTexture->GetHandle());
        shader_->SetInt("uEnvIrradianceMap", 6);

        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, envSpecularTexture->GetHandle());
        shader_->SetInt("uEnvSpecularMap", 7);

        shader_->SetBool("uEnableIBL", true);

        // Create airplane
        auto sphereGeo = std::make_shared<SphereGeometry>(0.3f, 64, 64);
        auto sphereMaterial = std::make_shared<PbrMaterial>(glm::vec3(1));
        sphere_ = std::make_shared<Mesh>(sphereGeo, sphereMaterial);
        sphere_->GetTransform().SetPosition({0, 1.0, 1.2});


        // Camera
        Eigen::Vector3d lookAtTarget(0, 0, 0);
        camera_ = std::make_shared<Camera>(45.0);
        glm::mat4 view_mat = glm::lookAt(glm::vec3(2, 0.5, 3), glm::vec3(lookAtTarget.x(), lookAtTarget.y(), lookAtTarget.z()), glm::vec3(0, 1, 0));
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
        shader_->SetVec3("uCamPosW", glm::vec3(camPos.x(), camPos.y(), camPos.z()));

        // Set light uniforms
        shader_->SetVec3("uLightPositions[0]", glm::vec3(0, 2.0, 2.0));
        shader_->SetVec3("uLightColors[0]", glm::vec3(180.0));
        shader_->SetInt("uLightCount", 1);

        // draw sphere
        auto sphereMaterial = std::dynamic_pointer_cast<PbrMaterial>(sphere_->GetMaterial());
        sphereMaterial->SetBaseColor(baseColor_);
        sphereMaterial->SetMetalness(metalness_);
        sphereMaterial->SetRoughness(roughness_);
        sphere_->Draw(camera_, shader_);

        // draw car
        carExt_->GetTransform().Rotate({0, 1, 0}, 0.01);
        carExtInner_->SetTransform(carExt_->GetTransform());
        carInterior_->SetTransform(carExt_->GetTransform());
        carInterior_->Draw(camera_, shader_);
        carExt_->Draw(camera_, shader_);
        carExtInner_->Draw(camera_, shader_);

        // UI
        ui_.NewFrame();

        ImGui::SetNextWindowSize({300, 180});
        ImGui::Begin("", 0, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::ColorEdit3("BaseColor", (float*)&baseColor_);
        ImGui::SliderFloat("Metalness", &metalness_, 0.0, 1.0);
        ImGui::SliderFloat("Roughness", &roughness_, 0.0, 1.0);

        ui_.Render();

    }

    void LoadExterior() {
        carExt_ = IOUtil::LoadJsonModel("./models/car/car-ext.json");
        carExtInner_ = IOUtil::LoadJsonModel("./models/car/car-ext-inner.json");
        auto material = std::make_shared<PbrMaterial>(glm::vec3(1));

        auto baseColorTexture = IOUtil::LoadTexture("./models/car/car-ext-color.jpg");
        material->SetBaseColorTexture(baseColorTexture);

        auto rmoTexture = IOUtil::LoadTexture("./models/car/car-ext-rmo.jpg");
        material->SetRmoTexture(rmoTexture);

        auto opacityTexture = IOUtil::LoadTexture("./models/car/car-ext-opacity.jpg");
        material->SetOpacityTexture(opacityTexture);

        auto emissiveTexture = IOUtil::LoadTexture("./models/car/car-ext-emissive.jpg");
        material->SetEmissiveTexture(emissiveTexture);

        carExt_->SetMaterial(material);
        carExtInner_->SetMaterial(material);
    }

    void LoadInterior() {
        carInterior_ = IOUtil::LoadJsonModel("./models/car/car-int.json");

        auto material = std::make_shared<PbrMaterial>(glm::vec3(1));
        auto baseColorTexture = IOUtil::LoadTexture("./models/car/car-int-color.jpg");
        material->SetBaseColorTexture(baseColorTexture);

        auto rmoTexture = IOUtil::LoadTexture("./models/car/car-int-rmo.jpg");
        material->SetRmoTexture(rmoTexture);

        auto opacityTexture = IOUtil::LoadTexture("./models/car/white.jpg");
        material->SetOpacityTexture(opacityTexture);

        auto emissiveTexture = IOUtil::LoadTexture("./models/car/black.jpg");
        material->SetEmissiveTexture(emissiveTexture);

        carInterior_->SetMaterial(material);
    }

private:
    ShaderPtr shader_;

    MeshPtr sphere_;

    MeshPtr carExt_;
    MeshPtr carExtInner_;
    MeshPtr carInterior_;

    CameraPtr camera_;

    std::shared_ptr<OrbitControls> controls_;

    // material properties
    glm::vec3 baseColor_ = glm::vec3(1);
    float metalness_ = 0.5f;
    float roughness_ = 0.5f;

};
} // namespace vivid


int main() {
    vivid::BasicPbrDemo app;
    app.Run();

    return 0;
}