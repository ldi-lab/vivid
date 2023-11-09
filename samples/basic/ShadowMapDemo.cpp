#include <iostream>
#include "vivid/Application.h"
#include "vivid/core/Mesh.h"
#include "vivid/core/Camera.h"
#include "vivid/core/Shader.h"
#include "vivid/OrbitControls.h"
#include "vivid/utils/GlmUtils.h"
#include "vivid/utils/IOUtil.h"
#include "vivid/primitives/PlaneGeometry.h"
#include "vivid/primitives/BoxGeometry.h"
#include "vivid/primitives/SphereGeometry.h"
#include "vivid/extras/Shadow.h"
#include "vivid/extras/ShaderImpl.h"
#include <glm/gtc/matrix_transform.hpp>

namespace vivid {
    class PrimitivesDemoApp : public Application {
    public:
        PrimitivesDemoApp() : Application(1280, 960, "cube demo") {
            glEnable(GL_DEPTH_TEST);

            SetWindowResizable(false);

            // Load shader
            shader_ = ShaderImpl::LoadShader("./shaders/ShadowMapping.vert", "./shaders/ShadowMapping.frag");

            // Create airplane
            airplane_ = IOUtil::LoadJsonModel("./models/airplane.json");
            auto diffuseTexture = IOUtil::LoadTexture("./models/airplane.jpg");
            airplane_->AddTexture("diffuseMap", diffuseTexture);

            // Create ground
            auto groundGeometry = std::make_shared<PlaneGeometry>(10, 10, 4, 4);
            ground_ = std::make_shared<Mesh>(groundGeometry);
            ground_->GetTransform().Rotate(Eigen::Vector3d(1, 0, 0), EIGEN_PI/2);
            ground_->GetTransform().SetPosition(Eigen::Vector3d(0, -2, 0));

            // Quad
            auto quadGeometry = std::make_shared<PlaneGeometry>(2, 2, 1, 1);
            quad_ = std::make_shared<Mesh>(quadGeometry);

            quadShader_ = ShaderImpl::LoadShader("./shaders/Passthrough.vert", "./shaders/SimpleTexture.frag");


            // Camera
            Eigen::Vector3d lookAtTarget(0, 0, 0);
            camera_ = std::make_shared<Camera>(45.0);
            glm::mat4 view_mat = glm::lookAt(glm::vec3(5, 4, 10), glm::vec3(lookAtTarget.x(), lookAtTarget.y(), lookAtTarget.z()), glm::vec3(0, 1, 0));
            Eigen::Matrix4d Tcw = vivid::GlmUtils::glm2eigen<double>(view_mat);
            camera_->SetTransform(Transform(Tcw.inverse()));

            // light camera
            lightCamera_ = std::make_shared<Camera>(-3.0, 3.0, -3.0, 3.0, 1.0, 20.0);
            lightCamera_->GetTransform().SetPosition({3, 10.0, 3});
            lightCamera_->LookAt({0, 0, 0});

            // Frame buffer
            shadow_ = std::make_shared<Shadow>(lightCamera_, 1024, 1024);

            controls_ = std::make_shared<OrbitControls>(window_, camera_, lookAtTarget, UpDir::Y);
        }

        void Render() override {
            controls_->Update();

            // Animate the airplane
            double t = glfwGetTime();
            Eigen::Vector3d pos = airplane_->GetTransform().Position();
            pos.z() = std::sin(t * 0.5);
            airplane_->GetTransform().SetPosition(pos);
            Eigen::Matrix3d R = Eigen::AngleAxisd(sin(t * 0.5) * 0.1, Eigen::Vector3d(1, 0, 0)).toRotationMatrix();
            airplane_->GetTransform().SetRotation(R);

            // Render depth map
            std::vector<MeshPtr> castMeshes = { airplane_ };
            shadow_->RenderDepthMap(castMeshes);

            // Render to the screen
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, windowWidth_, windowHeight_);
            glClearColor(0.75f, 0.9f, 0.9f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Bind shader
            shader_->Use();

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, shadow_->GetDepthMapHandle());
            shader_->SetInt("shadowMap", 2);

            glm::mat4 lightCamPV = lightCamera_->GetProjectionMatrix() *lightCamera_->GetViewMatrix();
            shader_->SetMat4("shadowCameraPV", lightCamPV);

            // Draw mesh
            shader_->SetBool("hasDiffuseMap", true);
            airplane_->Draw(camera_, shader_);

            shader_->SetBool("hasDiffuseMap", false);
            shader_->SetVec3("diffuseColor", glm::vec3(0.3, 0.4, 0.5));
            ground_->Draw(camera_, shader_);

            // Render the depth texture to the plane
            quadShader_->Use();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, shadow_->GetDepthMapHandle());
            quadShader_->SetInt("myTexture", 0);

            glViewport(0, 0, 256, 256);
            quad_->Draw(nullptr, quadShader_);

        }

    private:
        ShaderPtr shader_;

        MeshPtr airplane_;
        MeshPtr ground_;

        CameraPtr camera_;

        // Fow shadow map
        CameraPtr  lightCamera_;
        std::shared_ptr<Shadow> shadow_;

        // Quad
        std::shared_ptr<Mesh> quad_;
        std::shared_ptr<Shader> quadShader_;

        std::shared_ptr<OrbitControls> controls_;

    };
} // namespace vivid


int main() {
    vivid::PrimitivesDemoApp app;
    app.Run();

    return 0;
}