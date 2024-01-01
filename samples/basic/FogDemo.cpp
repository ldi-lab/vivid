#include <iostream>
#include <fstream>
#include "vivid/Application.h"
#include "vivid/core/Mesh.h"
#include "vivid/core/Camera.h"
#include "vivid/core/Shader.h"
#include "vivid/OrbitControls.h"
#include "vivid/utils/GlmUtils.h"
#include "vivid/utils/json.hpp"
#include "vivid/utils/IOUtil.h"
#include "vivid/extras/ShaderImpl.h"
#include "vivid/extras/MaterialImpl.h"
#include <glm/gtc/matrix_transform.hpp>



namespace vivid {
class FogDemoApp : public Application {
public:
    FogDemoApp() : Application(800, 600, "Fog demo") {
        // Load model from json file
        LoadModel("./models/forest.json");

        // Load texture from image file
        imgData_ = IOUtil::LoadImage("./models/forest.jpg", imgWidth_, imgHeight_, imgChannels_);

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
        auto forestGeo = std::make_shared<Geometry>(&positionData_, &uvData_, &normalData_);

        // Load shader
        std::cout << "load shader...\n";
        shader_ = ShaderImpl::LoadShader("./shaders/Fog.vert", "./shaders/Fog.frag");

        // Mesh
        std::cout << "create mesh...\n";
        auto material = std::make_shared<BasicColorMaterial>(glm::vec3(1, 1, 1), diffuseTexture);
        forest_ = std::make_shared<Mesh>(forestGeo, material);

        // Camera
        std::cout << "create camera...\n";
        camera_ = std::make_shared<Camera>();
        glm::mat4 view_mat = glm::lookAt(glm::vec3(0, 6, 8), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        Eigen::Matrix4d Tcw = vivid::GlmUtils::glm2eigen<double>(view_mat);
        camera_->SetTransform(Transform(Tcw.inverse()));

        // Set background color
        bgColor = glm::vec3(0.75f, 0.9f, 0.9f);

        controls_ = std::make_shared<OrbitControls>(window_, camera_, Eigen::Vector3d(0,0,0), UpDir::Y);
    }

    void Render() override {
        glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // if you want to draw the wireframe, set the fill mode
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);      // GL_FILL, GL_LINE

        controls_->Update();

        // Bind shader
        shader_->Use();
        shader_->SetVec3("uFogColor", bgColor);
        shader_->SetFloat("uFogNear", 2);
        shader_->SetFloat("uFogFar", 15);

        // Draw mesh
        forest_->GetTransform().Rotate(Eigen::Vector3d(0, 1, 0), 0.005);
        forest_->Draw(camera_, shader_);

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

private:

    std::shared_ptr<Shader> shader_;
    std::shared_ptr<Mesh> forest_;

    CameraPtr camera_;

    std::shared_ptr<OrbitControls> controls_;

    // Model data
    std::vector<float> positionData_;
    std::vector<float> normalData_;
    std::vector<float> uvData_;

    // Texture data
    unsigned char* imgData_;
    int imgWidth_;
    int imgHeight_;
    int imgChannels_;

    // Background color
    glm::vec3 bgColor = {1.0f, 1.0f, 1.0f};

};

} // namespace vivid


int main() {
    vivid::FogDemoApp app;
    app.Run();

    return 0;
}