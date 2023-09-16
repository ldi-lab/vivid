#include <iostream>
#include "vivid/Application.h"
#include "vivid/core/Mesh.h"
#include "vivid/core/Camera.h"
#include "vivid/core/Shader.h"
#include "vivid/OrbitControls.h"
#include "vivid/utils/GlmUtils.h"
#include "vivid/extras/ShaderImpl.h"
#include <glm/gtc/matrix_transform.hpp>

namespace vivid {
class CubeDemoApp : public Application {
public:
    CubeDemoApp() : Application(800, 600, "cube demo") {
        // Create cube mesh
        float a = 0.5f, b = 0.5f, c = 0.5f;
        std::vector<float> positionData = {
                -a, -b, -c,
                a, -b, -c,
                a, b, -c,
                -a, b, -c,
                -a, -b, c,
                a, -b, c,
                a, b, c,
                -a, b, c
        };
        std::vector<float> colorData = {
                1.f, 1.f, 0.f,
                0.f, 1.f, 0.f,
                1.f, 1.f, 1.f,
                0.f, 1.f, 0.f,
                0.f, 0.f, 1.f,
                0.f, 1.f, 1.f,
                1.f, 0.f, 1.f,
                1.f, 0.f, 0.f
        };
        std::vector<unsigned int> indices = {
                0, 1, 2, 2, 3, 0,
                0, 1, 5, 5, 4, 0,
                1, 2, 6, 6, 5, 1,
                2, 3, 7, 7, 6, 2,
                3, 0, 4, 4, 7, 3,
                4, 5, 6, 4, 6, 7
        };

        // Attributes
        auto positionAttr = std::make_shared<Attribute>(AttributeType::Position, 3, false, &positionData);
        auto colorAttr = std::make_shared<Attribute>(AttributeType::Color, 3, false, &colorData);

        // Geometry
        auto cubeGeometry = std::make_shared<Geometry>();
        cubeGeometry->AddAttribute(positionAttr);
        cubeGeometry->AddAttribute(colorAttr);
        cubeGeometry->SetIndex(indices);

        // Load shader
        shader_ = ShaderImpl::GetVertexColoredShader();

        // Mesh
        cube_ = std::make_shared<Mesh>(cubeGeometry, GL_TRIANGLES , 0);

        // Camera
        camera_ = std::make_shared<Camera>();
        glm::mat4 view_mat = glm::lookAt(glm::vec3(9, 3, -2), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
        Eigen::Matrix4d Tcw = vivid::GlmUtils::glm2eigen<double>(view_mat);
        camera_->SetTransform(Transform(Tcw.inverse()));


        // MVP matrix
        model_mat_ = glm::mat4(1.0f);

        controls_ = std::make_shared<OrbitControls>(window_, camera_);
    }

    void Render() override {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        controls_->Update();

        // Bind shader
        shader_->Use();

        // Draw mesh
        cube_->Draw(camera_, shader_);

    }

private:

    std::shared_ptr<Shader> shader_;
    std::shared_ptr<Mesh> cube_;
    glm::mat4 model_mat_;

    CameraPtr camera_;

    std::shared_ptr<OrbitControls> controls_;

};
} // namespace vivid


int main() {
    vivid::CubeDemoApp app;
    app.Run();

    return 0;
}