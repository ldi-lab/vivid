#pragma once

#include <iostream>
#include "vivid/core/Camera.h"
#include "vivid/core/Mesh.h"
#include "vivid/core/Shader.h"
#include "vivid/extras/FrameBuffer.h"

namespace vivid {

class Shadow {
public:
    Shadow(CameraPtr lightCam, int width = 1024, int height = 1024);

    void RenderDepthMap(const std::vector<MeshPtr> &castMeshes);

    CameraPtr GetLightCam() {
        return lightCam_;
    }

    unsigned int GetDepthMapHandle() const {
        return depthFrameBuf_->depthTextureHandle_;
    }

private:
    CameraPtr lightCam_;
    int width_;
    int height_;

    ShaderPtr depthShader_;
    ShaderPtr shadowShader_;

    std::shared_ptr<FrameBuffer> depthFrameBuf_;




};
} // namespace vivid