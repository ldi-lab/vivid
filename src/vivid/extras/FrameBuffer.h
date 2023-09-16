#pragma once

#include <iostream>
#include <glad/glad.h>

namespace vivid {

class FrameBuffer {
public:
    FrameBuffer(int width, int height, bool colorFlag = false, bool depthFlag = false);

    bool Check();

    void CreateColorTexture();

    void CreateDepthTexture();

    void SetupDrawables();

    void Bind();

    void Unbind();


public:
    int width_;
    int height_;

    unsigned int frameBufferHandle_ = 0;

    unsigned int colorTextureHandle_ = 0;
    unsigned int depthTextureHandle_ = 0;

};

} // namespace vivid