#include <iostream>
#include <fstream>
#include "vivid/Application.h"
#include "vivid/core/Mesh.h"
#include "vivid/core/Camera.h"
#include "vivid/core/Shader.h"
#include "vivid/OrbitControls.h"
#include "vivid/utils/GlmUtils.h"
#include "vivid/utils/json.hpp"
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION  //necessary for stb_image.h
#include "vivid/utils/stb_image.h"
#include "vivid/extras/FrameBuffer.h"
#include "vivid/primitives/PlaneGeometry.h"


namespace vivid {
    class FrameBufferDemoApp : public Application {
    public:
        FrameBufferDemoApp() : Application(800, 600, "Load json demo") {
            glEnable(GL_DEPTH_TEST);

            // Make the window size constant
            SetWindowResizable(false);

            // Load model from json file
            LoadModel("./models/fox.json");

            // Load texture from image file
            LoadTexture("./models/fox.jpg");

            // Create texture
            std::cout << "create texture...\n";
            auto diffuseTexture = std::make_shared<Texture>(imgData_, imgWidth_, imgHeight_, imgChannels_);

            // Attributes
            std::cout << "create texture...\n";
            auto positionAttr = std::make_shared<Attribute>(AttributeType::Position, 3, false, &positionData_);
            auto normalAttr = std::make_shared<Attribute>(AttributeType::Normal, 3, true, &normalData_);
            auto uvAttr = std::make_shared<Attribute>(AttributeType::TexCoord0, 2, false, &uvData_);

            // Geometry
            std::cout << "create geometry...\n";
            auto cubeGeometry = std::make_shared<Geometry>();
            cubeGeometry->AddAttribute(positionAttr);
            cubeGeometry->AddAttribute(normalAttr);
            cubeGeometry->AddAttribute(uvAttr);

            // Load shader
            std::cout << "load shader...\n";
            shader_ = std::make_shared<Shader>("SimpleShading.vert", "SimpleShading.frag");

            // Mesh
            std::cout << "create mesh...\n";
            cube_ = std::make_shared<Mesh>(cubeGeometry, GL_TRIANGLES , 0);
            cube_->AddTexture("diffuseMap", diffuseTexture);

            // Camera
            std::cout << "create camera...\n";
            camera_ = std::make_shared<Camera>(60, 4.0f/3.0f, 0.1f, 100.0f);
            glm::mat4 view_mat = glm::lookAt(glm::vec3(7, 3, -2), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
            Eigen::Matrix4d Tcw = vivid::GlmUtils::glm2eigen<double>(view_mat);
            camera_->SetTransform(Transform(Tcw.inverse()));

            // Frame buffer
            frameBuffer_ = std::make_shared<FrameBuffer>(windowWidth_, windowHeight_);
            frameBuffer_->CreateColorTexture();
            frameBuffer_->CreateDepthTexture();
            frameBuffer_->SetupDrawables();
            if (!frameBuffer_->Check()) {
                std::cerr << "frameBuffer error!\n";
                exit(-1);
            }

            // Quad
            auto quadGeometry = std::make_shared<PlaneGeometry>(2, 2, 1, 1);
            quad_ = std::make_shared<Mesh>(quadGeometry);
            quadShader_ = std::make_shared<Shader>("Passthrough.vert", "WobbyTexture.frag");

            controls_ = std::make_shared<OrbitControls>(window_, camera_, Eigen::Vector3d(0, 1, 0));
        }

        void Render() override {

            // Animate
            cube_->GetTransform().Rotate(Eigen::Vector3d(0, 1, 0), 0.005);

            // if you want to draw the wireframe, set the fill mode
            // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);      // GL_FILL, GL_LINE

            controls_->Update();

            // Render to our frame buffer
            frameBuffer_->Bind();
            glClearColor(0.75f, 0.9f, 0.9f, 1.0f);
            glViewport(0, 0, windowWidth_, windowHeight_);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader_->Use();

            cube_->Draw(camera_, shader_);


            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, windowWidth_, windowHeight_);
            glClearColor(0.75f, 0.9f, 0.9f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            quadShader_->Use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, frameBuffer_->colorTextureHandle_);
            quadShader_->SetInt("myTexture", 0);

            float time = (float)(glfwGetTime() * 10.0f);
            quadShader_->SetFloat("time", time);

            quad_->Draw(nullptr, quadShader_);

        }

        void LoadModel(const std::string& modelPath) {
            std::ifstream ifs(modelPath);
            if (!ifs.is_open()) {
                std::cerr << "failed to open file: " << modelPath << std::endl;
                exit(-1);
            }
            auto js = nlohmann::json::parse(ifs);
            ifs.close();

            positionData_ = js["position"].get<std::vector<float> >();
            normalData_ = js["normal"].get<std::vector<float> >();
            uvData_ = js["uv"].get<std::vector<float> >();
            std::cout << "position: " << positionData_.size() << ", normal: " << normalData_.size()
                      << ", uv: " << uvData_.size() << "\n";

            // Fix UV.y
            for (size_t i = 1; i < uvData_.size(); i += 2) {
                uvData_[i] = 1.0f - uvData_[i];
            }
        }

        void LoadTexture(const std::string& imgPath) {
            imgData_ = stbi_load(imgPath.c_str(), &imgWidth_, &imgHeight_, &imgChannels_, 0);
            std::cout << "img: " << imgWidth_ << " x " << imgHeight_ << " x " << imgChannels_ << "\n";
        }

    private:

        std::shared_ptr<Shader> shader_;
        std::shared_ptr<Mesh> cube_;

        CameraPtr camera_;

        std::shared_ptr<OrbitControls> controls_;

        // Model data
        std::vector<float> positionData_;
        std::vector<float> normalData_;
        std::vector<float> uvData_;

        //  Texture data
        unsigned char* imgData_;
        int imgWidth_;
        int imgHeight_;
        int imgChannels_;

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