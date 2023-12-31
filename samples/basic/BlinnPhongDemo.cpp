#include <iostream>
#include "vivid/Application.h"
#include "vivid/core/Mesh.h"
#include "vivid/core/Camera.h"
#include "vivid/core/Shader.h"
#include "vivid/OrbitControls.h"
#include "vivid/utils/GlmUtils.h"
#include "vivid/primitives/PlaneGeometry.h"
#include "vivid/primitives/BoxGeometry.h"
#include "vivid/primitives/SphereGeometry.h"
#include "vivid/extras/ShaderImpl.h"
#include "vivid/extras/MaterialImpl.h"
#include <glm/gtc/matrix_transform.hpp>

namespace vivid {
    class ColoredBlinnPhongDemoApp : public Application {
    public:
        ColoredBlinnPhongDemoApp() : Application(800, 600, "cube demo") {
            // Load shader
            shader_ = ShaderImpl::GetBlinnPhongShader();

            glm::vec3 diffuseColor(0.2, 0.7, 0.8);
            glm::vec3 specularColor(1, 0, 0);
            float shininess = 5;
            auto material = std::make_shared<PhongMaterial>(diffuseColor, specularColor, shininess);

            // Create plane
            auto planeGeometry = std::make_shared<PlaneGeometry>(2, 2, 3, 3);
            plane_ = std::make_shared<Mesh>(planeGeometry, material);

            // Create box
            auto boxGeometry = std::make_shared<BoxGeometry>(1, 1, 1, 2, 2, 2);
            box_ = std::make_shared<Mesh>(boxGeometry, material);
            box_->GetTransform().SetPosition(Eigen::Vector3d(0, 2, 0));

            // Create sphere
            auto sphereGeometry = std::make_shared<SphereGeometry>(0.5);
            sphere_ = std::make_shared<Mesh>(sphereGeometry, material);
            sphere_->GetTransform().SetPosition(Eigen::Vector3d(0, 0, 2));


            // Camera
            camera_ = std::make_shared<Camera>();
            camera_->GetTransform().SetPosition(Eigen::Vector3d(6, 2, 0));
            camera_->LookAt(Eigen::Vector3d(0, 0, 0), Eigen::Vector3d(0, 1, 0));

            controls_ = std::make_shared<OrbitControls>(window_, camera_, Eigen::Vector3d(0, 0, 0), UpDir::Y);
        }

        void Render() override {
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            controls_->Update();

            // Bind shader
            shader_->Use();

            // set environment uniforms
            shader_->SetVec3("uLight.positionC", glm::vec3(1, 1, 1));
            shader_->SetVec3("uLight.color", glm::vec3(1.0, 1.0, 1.0));
            shader_->SetInt("uLight.type", 0);

            shader_->SetVec3("uAmbientColor", glm::vec3(0.8, 0.8, 0.8));

            // Draw mesh
            plane_->GetTransform().Rotate(Eigen::Vector3d(0, 1, 0), 0.005);
            box_->GetTransform().Rotate(Eigen::Vector3d(0, 1, 0), 0.005);
            sphere_->GetTransform().Rotate(Eigen::Vector3d(0, 1, 0), 0.01);
            plane_->Draw(camera_, shader_);
            box_->Draw(camera_, shader_);
            sphere_->Draw(camera_, shader_);
        }

    private:

        std::shared_ptr<Shader> shader_;
        std::shared_ptr<Mesh> plane_;
        std::shared_ptr<Mesh> box_;
        std::shared_ptr<Mesh> sphere_;

        CameraPtr camera_;

        std::shared_ptr<OrbitControls> controls_;

    };
} // namespace vivid


int main() {
    vivid::ColoredBlinnPhongDemoApp app;
    app.Run();

    return 0;
}