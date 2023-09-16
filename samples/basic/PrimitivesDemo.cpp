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
#include "vivid/primitives/AxesHelper.h"
#include "vivid/primitives/CylinderGeometry.h"
#include "vivid/primitives/BoneGeometry.h"
#include "vivid/extras/ShaderImpl.h"
#include <glm/gtc/matrix_transform.hpp>

namespace vivid {
    class PrimitivesDemoApp : public Application {
    public:
        PrimitivesDemoApp() : Application(800, 600, "cube demo") {
            // Load shaders
            shader_ = ShaderImpl::LoadShader("./shaders/SimpleColorShading.vert", "./shaders/SimpleColorShading.frag");
            helperShader_ = ShaderImpl::GetVertexColoredShader();

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

            // Axes helper
            auto axesGeometry = std::make_shared<AxesHelper>(3);
            axes_ = std::make_shared<Mesh>(axesGeometry, GL_LINES);

            // Cylinder
            auto cylinderGeometry = std::make_shared<CylinderGeometry>(0.5, 0.2, 3, 12, 4);
            cylinder_ = std::make_shared<Mesh>(cylinderGeometry);
            cylinder_->GetTransform().SetPosition(Eigen::Vector3d(1, 2, 0));

            // Bone
            auto boneGeometry = std::make_shared<BoneGeometry>(2, 4, Eigen::Vector3f(0,0,1));
            bone_ = std::make_shared<Mesh>(boneGeometry);

            // Camera
            Eigen::Vector3d lookAtTarget(0, 0, 0);
            camera_ = std::make_shared<Camera>();
            glm::mat4 view_mat = glm::lookAt(glm::vec3(6, 0, 0), glm::vec3(lookAtTarget.x(), lookAtTarget.y(), lookAtTarget.z()), glm::vec3(0, 0, 1));
            Eigen::Matrix4d Tcw = vivid::GlmUtils::glm2eigen<double>(view_mat);
            camera_->SetTransform(Transform(Tcw.inverse()));

            controls_ = std::make_shared<OrbitControls>(window_, camera_, lookAtTarget);
        }

        void Render() override {
            glClearColor(0.75f, 0.9f, 0.9f, 1.0f);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            controls_->Update();

            // Bind shader
            shader_->Use();

            // Draw mesh
            plane_->GetTransform().Rotate(Eigen::Vector3d(0, 0, 1), 0.005);
            box_->GetTransform().Rotate(Eigen::Vector3d(0, 0, 1), 0.005);
            sphere_->GetTransform().Rotate(Eigen::Vector3d(0, 0, 1), 0.01);
            plane_->Draw(camera_, shader_);
            box_->Draw(camera_, shader_);
            sphere_->Draw(camera_, shader_);
            cylinder_->Draw(camera_, shader_);
            bone_->Draw(camera_, shader_);

            // Draw helper
            helperShader_->Use();
            axes_->Draw(camera_, helperShader_);
        }

    private:

        std::shared_ptr<Shader> shader_;
        std::shared_ptr<Mesh> plane_;
        std::shared_ptr<Mesh> box_;
        std::shared_ptr<Mesh> sphere_;
        std::shared_ptr<Mesh> cylinder_;
        std::shared_ptr<Mesh> bone_;

        // Helper
        std::shared_ptr<Mesh> axes_;
        std::shared_ptr<Shader> helperShader_;

        CameraPtr camera_;

        std::shared_ptr<OrbitControls> controls_;

    };
} // namespace vivid


int main() {
    vivid::PrimitivesDemoApp app;
    app.Run();

    return 0;
}