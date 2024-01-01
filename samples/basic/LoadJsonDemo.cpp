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
class LoadJsonDemoApp : public Application {
public:
    LoadJsonDemoApp() : Application(800, 600, "Load json demo") {
        // Load model from json file
        LoadModel("./models/fox.json");

        // Load texture from image file
        imgData_ = IOUtil::LoadImage("./models/fox.jpg", imgWidth_, imgHeight_, imgChannels_);

        // Create texture
        std::cout << "create texture...\n";
        auto colorTexture = std::make_shared<Texture>(imgData_, imgWidth_, imgHeight_, imgChannels_);

        // Attributes
        std::cout << "create texture...\n";
        auto positionAttr = std::make_shared<Attribute>(AttributeType::Position, 3, false, &positionData_);
        auto normalAttr = std::make_shared<Attribute>(AttributeType::Normal, 3, true, &normalData_);
        auto uvAttr = std::make_shared<Attribute>(AttributeType::TexCoord0, 2, false, &uvData_);

        // Geometry
        std::cout << "create geometry...\n";
        auto foxGeometry = std::make_shared<Geometry>();
        foxGeometry->AddAttribute(positionAttr);
        foxGeometry->AddAttribute(normalAttr);
        foxGeometry->AddAttribute(uvAttr);

        // Load shader
        std::cout << "load shader...\n";
        //shader_ = ShaderImpl::LoadShader("./shaders/SimpleShading.vert", "./shaders/SimpleShading.frag");
        shader_ = ShaderImpl::GetBasicShadingShader();

        // material
        auto material = std::make_shared<BasicColorMaterial>(glm::vec3(0.5), colorTexture);

        // Mesh
        std::cout << "create mesh...\n";
        fox_ = std::make_shared<Mesh>(foxGeometry, material);

        // Camera
        std::cout << "create camera...\n";
        camera_ = std::make_shared<Camera>();
        glm::mat4 view_mat = glm::lookAt(glm::vec3(9, 3, -2), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
        Eigen::Matrix4d Tcw = vivid::GlmUtils::glm2eigen<double>(view_mat);
        camera_->SetTransform(Transform(Tcw.inverse()));

        controls_ = std::make_shared<OrbitControls>(window_, camera_, Eigen::Vector3d(0,1,0), UpDir::Y);
    }

    void Render() override {
        glClearColor(0.75f, 0.9f, 0.9f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // if you want to draw the wireframe, set the fill mode
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);      // GL_FILL, GL_LINE

        controls_->Update();

        // Bind shader
        shader_->Use();

        // Draw mesh
        fox_->GetTransform().Rotate(Eigen::Vector3d(0, 1, 0), 0.005);
        fox_->Draw(camera_, shader_);

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
    std::shared_ptr<Mesh> fox_;

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

};

} // namespace vivid


int main() {
    vivid::LoadJsonDemoApp app;
    app.Run();

    return 0;
}