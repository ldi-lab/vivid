#include "Texture.h"

namespace vivid {

Texture::Texture(unsigned char *data, int width, int height, int channels,
                 int warpS, int warpT, bool generateMipmaps, int minFilter, int magFilter)
         : width_(width), height_(height), channels_(channels), warpS_(warpS), warpT_(warpT),
         generateMipmaps_(generateMipmaps), minFilter_(minFilter), magFilter_(magFilter)
 {
    // Create one GL texture
    glGenTextures(1, &textureHandle_);

    // Bind
    glBindTexture(GL_TEXTURE_2D, textureHandle_);

    // Upload the image data to GPU
    if (channels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }


    // Generate mipmaps?
    if (generateMipmaps_) {
        glGenerateMipmap(GL_TEXTURE_2D);
        minFilter_ = GL_LINEAR_MIPMAP_LINEAR;
    }

     // Set texture parameters
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, warpS_);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, warpT_);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter_);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter_);
 }


 void Texture::Bind() const {
    glBindTexture(GL_TEXTURE_2D, textureHandle_);
}


void Texture::Update(unsigned char *data) {
    glBindTexture(GL_TEXTURE_2D, textureHandle_);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
}


} //namespace vivid