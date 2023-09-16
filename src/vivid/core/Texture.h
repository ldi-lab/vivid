#pragma once

#include <iostream>
#include <glad/glad.h>

namespace vivid {

class Texture {
public:
    Texture(
            unsigned char *data,
            int width,
            int height,
            int channels,
            int warpS = GL_CLAMP_TO_EDGE,
            int warpT = GL_CLAMP_TO_EDGE,
            bool generateMipmaps = false,
            int minFilter = GL_LINEAR,
            int magFilter = GL_LINEAR);

    void Bind() const;

    unsigned int GetHandle() const {
        return textureHandle_;
    }


private:
    int width_;
    int height_;
    int channels_;
    int warpS_;
    int warpT_;
    bool generateMipmaps_;
    int minFilter_;
    int magFilter_;

    // GL handle
    unsigned int textureHandle_ = 0;

};

} //namespace vivid