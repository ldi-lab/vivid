#include <iostream>
#include "vivid/Application.h"
#include "vivid/core/Mesh.h"
#include "vivid/core/Camera.h"
#include "vivid/core/Shader.h"
#include "vivid/core/Attribute.h"
#include "vivid/extras/ShaderImpl.h"
#include "vivid/OrbitControls.h"
#include "vivid/utils/GlmUtils.h"

namespace vivid {
    class TriangleScreenShaderApp : public Application {
    public:
        TriangleScreenShaderApp() : Application(800, 600, "triangle screen shader") {
            // Rather than using a plane (two triangles) to cover the viewport here is a
            // triangle that includes -1 to 1 range for 'position', and 0 to 1 range for 'uv'.
            // Excess will be out of the viewport.

            //         position                uv
            //      (-1, 3)                  (0, 2)
            //         |\                      |\
            //         |__\(1, 1)              |__\(1, 1)
            //         |__|_\                  |__|_\
            //   (-1, -1)   (3, -1)        (0, 0)   (2, 0)

            // Create triangle mesh
            std::vector<float> positionData = {
                    -1, -1, 0,
                    3, -1, 0,
                    -1, 3, 0
            };
            std::vector<float> uvData = {
                    0, 0,
                    2, 0,
                    0, 0
            };
            std::vector<unsigned int> indices = {
                    0, 1, 2
            };

            auto vertex_positions = std::make_shared<Attribute>(AttributeType::Position, 3, false);
            vertex_positions->SetData(positionData);

            auto vertex_uvs = std::make_shared<Attribute>(AttributeType::TexCoord0, 2, false);
            vertex_uvs->SetData(uvData);

            auto triangleGeometry = std::make_shared<Geometry>();
            triangleGeometry->AddAttribute(vertex_positions);
            triangleGeometry->AddAttribute(vertex_uvs);
            triangleGeometry->SetIndex(indices);


            // Load shader
            shader_ = ShaderImpl::LoadShader("./shaders/TriangleScreenShader.vert", "./shaders/TriangleScreenShader.frag");

            triangle_ = std::make_shared<Mesh>(triangleGeometry);

            camera_ = std::make_shared<Camera>();

        }

        void Render() override {
            glClearColor(0.5f, 0.3f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Bind shader
            shader_->Use();

            // Set uniforms
            shader_->SetVec3("uColor", glm::vec3(0.3f, 0.2f, 0.5f));
            shader_->SetFloat("uTime", time_);

            // Draw mesh
            triangle_->Draw(camera_, shader_);

            time_ += 0.01f;

        }

    private:

        std::shared_ptr<Shader> shader_;
        std::shared_ptr<Mesh> triangle_;
        std::shared_ptr<Camera> camera_;

        float time_ = 0.f;

    };
} // namespace vivid


int main() {
    vivid::TriangleScreenShaderApp app;
    app.Run();

    return 0;
}