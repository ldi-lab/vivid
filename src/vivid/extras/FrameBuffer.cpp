#include "FrameBuffer.h"

namespace vivid {

FrameBuffer::FrameBuffer(int width, int height, bool colorFlag, bool depthFlag)
        : width_(width), height_(height)
{
    frameBufferHandle_ = 0;
    glGenFramebuffers(1, &frameBufferHandle_);
    if (colorFlag) {
        CreateColorTexture();
    }
    if (depthFlag) {
        CreateDepthTexture();
    }
    SetupDrawables();
}


bool FrameBuffer::Check() {
    Bind();
    return glCheckFramebufferStatus(GL_FRAMEBUFFER)
                                == GL_FRAMEBUFFER_COMPLETE;
}


void FrameBuffer::CreateColorTexture() {
    Bind();

    // Create color texture.
    colorTextureHandle_ = 0;
    glGenTextures(1, &colorTextureHandle_);
    glBindTexture(GL_TEXTURE_2D, colorTextureHandle_);
    // give an empty image to opengl.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)width_, (GLsizei)height_, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    // filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Attach
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTextureHandle_, 0);

}


void FrameBuffer::CreateDepthTexture() {
    // Bind frame buffer
    Bind();

    // Create depth texture. Slower than a depth buffer, but you can sample it later in your shader
    depthTextureHandle_ = 0;
    glGenTextures(1, &depthTextureHandle_);
    glBindTexture(GL_TEXTURE_2D, depthTextureHandle_);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT, (GLsizei)width_, (GLsizei)height_, 0,GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    // Attach
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTextureHandle_, 0);

    Unbind();

}


void FrameBuffer::SetupDrawables() {
    Bind();
    if (colorTextureHandle_ == 0) {
        glDrawBuffer(GL_NONE);  // no color output in the bound frame buffer, only depth
    } else {
        GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, drawBuffers);  // "1" is the size of drawBuffers
    }
    Unbind();
}


void FrameBuffer::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferHandle_);
}


void FrameBuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


} // namespace vivid