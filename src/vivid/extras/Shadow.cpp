#include <utility>

#include "vivid/extras/Shadow.h"
#include "vivid/extras/ShaderImpl.h"

namespace vivid {

Shadow::Shadow(CameraPtr lightCam, int width, int height)
    : lightCam_(std::move(lightCam)), width_(width), height_(height)
{
    // create frame buffer for depth map
    depthFrameBuf_ = std::make_shared<FrameBuffer>(width_, height_, false, true);

    // create depth shader
    depthShader_ = ShaderImpl::GetDepthShader();
}


void Shadow::RenderDepthMap(const std::vector<MeshPtr> &castMeshes) {
    // render depth map
    depthFrameBuf_->Bind();
    glViewport(0, 0, width_, height_);
    glClear(GL_DEPTH_BUFFER_BIT);
    depthShader_->Use();
    for (const auto &mesh : castMeshes) {
        mesh->Draw(lightCam_, depthShader_);
    }
    depthFrameBuf_->Unbind();
}


}; // namespace vivid