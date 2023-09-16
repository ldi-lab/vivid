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
#include "vivid/extras/FrameBuffer.h"
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

            lightDepthShader_ = ShaderImpl::LoadShader("./shaders/DepthShader.vert", "./shaders/DepthShader.frag");

            // Create plane
            auto planeGeometry = std::make_shared<PlaneGeometry>(2, 2, 3, 3);
            plane_ = std::make_shared<Mesh>(planeGeometry);

            // Create box
            auto boxGeometry = std::make_shared<BoxGeometry>(1, 1, 1, 2, 2, 2);
            box_ = std::make_shared<Mesh>(boxGeometry);
            box_->GetTransform().SetPosition(Eigen::Vector3d(0, 2, 0));

            // Create sphere
            auto sphereGeometry = std::make_shared<SphereGeometry>(0.5);
            sphere_ = std::make_shared<Mesh>(sphereGeometry);
            sphere_->GetTransform().SetPosition(Eigen::Vector3d(0, 0, 2));

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
            camera_ = std::make_shared<Camera>();
            glm::mat4 view_mat = glm::lookAt(glm::vec3(6, 2, 0), glm::vec3(lookAtTarget.x(), lookAtTarget.y(), lookAtTarget.z()), glm::vec3(0, 1, 0));
            Eigen::Matrix4d Tcw = vivid::GlmUtils::glm2eigen<double>(view_mat);
            camera_->SetTransform(Transform(Tcw.inverse()));

            // light camera
            lightCamera_ = std::make_shared<Camera>(-8.0, 8.0, -8.0, 8.0, 0.1, 20.0);
            lightCamera_->GetTransform().SetPosition({0.1, 10.0, 0.1});
            lightCamera_->LookAt({0, 0, 0});

            // Frame buffer
            frameBuffer_ = std::make_shared<FrameBuffer>(1024, 1024);
            frameBuffer_->CreateDepthTexture();
            frameBuffer_->SetupDrawables();

            controls_ = std::make_shared<OrbitControls>(window_, camera_, lookAtTarget);
        }

        void Render() override {
            controls_->Update();

            // Animate geometries
            plane_->GetTransform().Rotate(Eigen::Vector3d(0, 1, 0), 0.005);
            box_->GetTransform().Rotate(Eigen::Vector3d(0, 1, 0), 0.005);
            sphere_->GetTransform().Rotate(Eigen::Vector3d(0, 1, 0), 0.01);

            // Render to our framebuffer
            frameBuffer_->Bind();
            glViewport(0, 0, 1024, 1024);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Animate light direction
            double t = glfwGetTime();
            Eigen::Vector3d lightTarget(3*sin(t), 0, 3*cos(t));
            lightCamera_->LookAt(lightTarget);

            lightDepthShader_->Use();

            plane_->Draw(lightCamera_, lightDepthShader_);
            box_->Draw(lightCamera_, lightDepthShader_);
            sphere_->Draw(lightCamera_, lightDepthShader_);
            ground_->Draw(lightCamera_, lightDepthShader_);


            // Render to the screen
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, windowWidth_, windowHeight_);
            glClearColor(0.75f, 0.9f, 0.9f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Bind shader
            shader_->Use();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, frameBuffer_->depthTextureHandle_);
            shader_->SetInt("shadowMap", 0);

            glm::mat4 lightCamPV = lightCamera_->GetProjectionMatrix() *lightCamera_->GetViewMatrix();
            shader_->SetMat4("shadowCameraPV", lightCamPV);

            // Draw mesh

            plane_->Draw(camera_, shader_);
            box_->Draw(camera_, shader_);
            sphere_->Draw(camera_, shader_);

            ground_->Draw(camera_, shader_);

            // Render the depth texture to the plane
            quadShader_->Use();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, frameBuffer_->depthTextureHandle_);
            quadShader_->SetInt("myTexture", 0);

            glViewport(0, 0, 256, 256);
            quad_->Draw(nullptr, quadShader_);

        }

    private:

        std::shared_ptr<Shader> shader_;
        std::shared_ptr<Mesh> plane_;
        std::shared_ptr<Mesh> box_;
        std::shared_ptr<Mesh> sphere_;
        std::shared_ptr<Mesh> ground_;

        CameraPtr camera_;

        CameraPtr  lightCamera_;

        // Fow shadow map
        std::shared_ptr<Shader> lightDepthShader_;
        std::shared_ptr<FrameBuffer> frameBuffer_;

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