#include <iostream>
#include <fstream>
#include "vivid/Application.h"
#include "vivid/core/Mesh.h"
#include "vivid/core/Camera.h"
#include "vivid/core/Shader.h"
#include "vivid/OrbitControls.h"
#include "vivid/utils/GlmUtils.h"
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION  //necessary for stb_image.h
#include "vivid/utils/IOUtil.h"
#include "vivid/extras/FrameBuffer.h"
#include "vivid/extras/ShaderImpl.h"
#include "vivid/primitives/PlaneGeometry.h"


namespace vivid {
    class FrameBufferDemoApp : public Application {
    public:
        FrameBufferDemoApp() : Application(800, 600, "Load json demo") {
            glEnable(GL_DEPTH_TEST);

            // Make the window size constant
            SetWindowResizable(false);

            // Create texture
            std::cout << "create texture...\n";
            auto diffuseTexture = IOUtil::LoadTexture("./models/fox.jpg");

            // Load model from json file
            std::cout << "create model...\n";
            fox_ = IOUtil::LoadJsonModel("./models/fox.json");
            fox_->AddTexture("diffuseMap", diffuseTexture);

            // Load shader
            std::cout << "load shader...\n";
            shader_ = ShaderImpl::LoadShader("./shaders/SimpleShading.vert", "./shaders/SimpleShading.frag");

            // Camera
            std::cout << "create camera...\n";
            camera_ = std::make_shared<Camera>(60, 4.0f/3.0f, 0.1f, 100.0f);
            glm::mat4 view_mat = glm::lookAt(glm::vec3(7, 3, -2), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
            Eigen::Matrix4d Tcw = vivid::GlmUtils::glm2eigen<double>(view_mat);
            camera_->SetTransform(Transform(Tcw.inverse()));

            // Frame buffer
            frameBuffer_ = std::make_shared<FrameBuffer>(windowWidth_, windowHeight_, true, true);
            if (!frameBuffer_->Check()) {
                std::cerr << "frameBuffer error!\n";
                exit(-1);
            }

            // Quad
            auto quadGeometry = std::make_shared<PlaneGeometry>(2, 2, 1, 1);
            quad_ = std::make_shared<Mesh>(quadGeometry);
            quadShader_ = ShaderImpl::LoadShader("./shaders/Passthrough.vert", "./shaders/WobbyTexture.frag");

            controls_ = std::make_shared<OrbitControls>(window_, camera_, Eigen::Vector3d(0, 1, 0), UpDir::Y);
        }

        void Render() override {

            // Animate
            fox_->GetTransform().Rotate(Eigen::Vector3d(0, 1, 0), 0.005);

            // if you want to draw the wireframe, set the fill mode
            // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);      // GL_FILL, GL_LINE

            controls_->Update();

            // Render to our frame buffer
            frameBuffer_->Bind();
            glClearColor(0.75f, 0.9f, 0.9f, 1.0f);
            glViewport(0, 0, windowWidth_, windowHeight_);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader_->Use();

            fox_->Draw(camera_, shader_);

            // Render to screen
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, windowWidth_, windowHeight_);
            glClearColor(0.75f, 0.9f, 0.9f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            quadShader_->Use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, frameBuffer_->colorTextureHandle_);
            quadShader_->SetInt("myTexture", 0);

            auto time = (float)(glfwGetTime() * 10.0f);
            quadShader_->SetFloat("time", time);

            quad_->Draw(nullptr, quadShader_);

        }

    private:

        std::shared_ptr<Shader> shader_;
        std::shared_ptr<Mesh> fox_;

        CameraPtr camera_;

        std::shared_ptr<OrbitControls> controls_;

        // Quad
        std::shared_ptr<Mesh> quad_;
        std::shared_ptr<Shader> quadShader_;

        // Frame buffer
        std::shared_ptr<FrameBuffer> frameBuffer_;

    };

} // namespace vivid


int main() {
    vivid::FrameBufferDemoApp app;
    app.Run();

    return 0;
}