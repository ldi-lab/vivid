#include <iostream>
#include <random>
#include "vivid/vivid.h"
#include "vivid/utils/GlmUtils.h"
#include <glm/gtc/matrix_transform.hpp>

namespace vivid {
class PointCloudDemoApp : public Application {
public:
    PointCloudDemoApp() : Application(800, 600, "cube demo") {
        // Load shaders
        shader_ = ShaderImpl::LoadShader("./shaders/PointCloud.vert", "./shaders/PointCloud.frag");

        // create point cloud
        GeneratePointCloud(points_, colors_, numPoints_);

        // Camera
        Eigen::Vector3d lookAtTarget(0, 0, 0);
        camera_ = std::make_shared<Camera>();
        glm::mat4 view_mat = glm::lookAt(glm::vec3(6, 0, 3), glm::vec3(lookAtTarget.x(), lookAtTarget.y(), lookAtTarget.z()), glm::vec3(0, 0, 1));
        Eigen::Matrix4d Tcw = vivid::GlmUtils::glm2eigen<double>(view_mat);
        camera_->SetTransform(Transform(Tcw.inverse()));

        controls_ = std::make_shared<OrbitControls>(window_, camera_, lookAtTarget);
    }

    void Render() override {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        controls_->Update();

        // Bind shader
        shader_->Use();
        shader_->SetFloat("uPointSize", pointSize_);

        // Draw mesh
        pointCloud_->Draw(camera_, shader_);

        // Draw UI
        ui_.NewFrame();
        ImGui::PushStyleColor(ImGuiCol_WindowBg, {0.1, 0.1, 0.1, 0.6});
        ImGui::SetNextWindowSize({260, 180});
        ImGui::Begin("Settings");
        ImGui::Text("FPS: ");
        ImGui::SameLine();
        ImGui::TextColored({0, 1, 0, 1}, "%d", static_cast<int>(ImGui::GetIO().Framerate));
        ImGui::InputFloat("Point Size", &pointSize_, 1.0);
        if (ImGui::InputInt("Point Count", &numPoints_, 10000)) {
            GeneratePointCloud(points_, colors_, numPoints_);
        }
        ImGui::End();
        ImGui::PopStyleColor();
        ui_.Render();
    }


    void GeneratePointCloud(std::vector<float> &points, std::vector<float> &colors, size_t num_points) {
        points.resize(num_points * 3);
        colors.resize(num_points * 3);

        const float s = 2.f;
        std::default_random_engine dre;
        std::uniform_real_distribution<float> urd(0, s);

        for (size_t i = 0; i < num_points * 3; ++i) {
            float x = urd(dre);
            points[i] = x;
        }

        for (size_t i = 0; i < num_points; ++i) {
            size_t i3 = i * 3;
            colors[i3 + 1] = 1 - points[i3] / s;
            colors[i3 + 2] = 1 - points[i3 + 1] / s;
            colors[i3] = 1 - points[i3 + 2] / s;
        }

        auto geo = std::make_shared<Geometry>();
        geo->AddAttribute(std::make_shared<Attribute>(AttributeType::Position, 3, false, &points_));
        geo->AddAttribute(std::make_shared<Attribute>(AttributeType::Color, 3, false, &colors_));
        pointCloud_ = std::make_shared<Mesh>(geo, GL_POINTS);
    }

private:

    // point cloud
    MeshPtr pointCloud_;
    std::shared_ptr<Shader> shader_;

    float pointSize_ = 2.0;
    int numPoints_ = 10000;
    std::vector<float> points_;
    std::vector<float> colors_;

    CameraPtr camera_;

    std::shared_ptr<OrbitControls> controls_;

};
} // namespace vivid


int main() {
    vivid::PointCloudDemoApp app;
    app.Run();

    return 0;
}