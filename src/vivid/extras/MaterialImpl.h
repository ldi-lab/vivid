#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <utility>
#include "vivid/core/Material.h"
#include "vivid/core/Texture.h"
#include "vivid/core/Shader.h"

namespace vivid {

class BasicColorMaterial : public Material {
public:
    BasicColorMaterial(const glm::vec3 &color, TexturePtr colorTex = nullptr)
        : Material(), color_(color), colorTexture_(std::move(colorTex)) {}

    void SetColor(const glm::vec3& color) {
        color_ = color;
    }

    glm::vec3 GetColor() const {
        return color_;
    }

    void SetColorTexture(const TexturePtr &colorTex) {
        colorTexture_ = colorTex;
    }

    TexturePtr GetColorTexture() const {
        return colorTexture_;
    }

    void SetUniforms(const ShaderPtr& shader) override {
        shader->SetVec3("uColor", color_);
        if (colorTexture_) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, colorTexture_->GetHandle());
            shader->SetInt("uColorMap", 0);
            shader->SetBool("uHasColorMap", true);
        } else {
            shader->SetBool("uHasColorMap", false);
        }
    }

private:
    glm::vec3 color_;
    TexturePtr colorTexture_ = nullptr;
};


class PointCloudMaterial : public Material {
public:
    explicit PointCloudMaterial(float pointSize = 1.0f)
        : Material(), pointSize_(pointSize) {}

    void SetPointSize(float pointSize) {
        pointSize_ = pointSize;
    }

    float GetPointSize() const {
        return pointSize_;
    }

    void SetUniforms(const ShaderPtr &shader) override {
        shader->SetFloat("uPointSize", pointSize_);
    }

private:
    float pointSize_;
};


class PhongMaterial : public Material {
public:
    PhongMaterial(const glm::vec3 &diffuseColor,
                  const glm::vec3 &specularColor,
                  float shininess = 5.0f,
                  TexturePtr diffuseTex = nullptr,
                  TexturePtr specularTex = nullptr,
                  TexturePtr normalTex = nullptr)
        : Material(),
          diffuseColor_(diffuseColor),
          specularColor_(specularColor),
          shininess_(shininess),
          diffuseTexture_(std::move(diffuseTex)),
          specularTexture_(std::move(specularTex)),
          normalTexture_(std::move(normalTex)) {}

    void SetDiffuseColor(const glm::vec3& color) {
        diffuseColor_ = color;
    }

    glm::vec3 GetDiffuseColor() const {
        return diffuseColor_;
    }

    void SetSpecularColor(const glm::vec3& color) {
        specularColor_ = color;
    }

    glm::vec3 GetSpecularColor() const {
        return specularColor_;
    }

    void SetShininess(float shininess) {
        shininess_ = shininess;
    }

    float GetShininess() const {
        return shininess_;
    }

    void SetDiffuseTexture(const TexturePtr& tex) {
        diffuseTexture_ = tex;
    }

    TexturePtr GetDiffuseTexture() const {
        return diffuseTexture_;
    }

    void SetSpecularTexture(const TexturePtr& tex) {
        specularTexture_ = tex;
    }

    TexturePtr GetSpecularTexture() const {
        return specularTexture_;
    }

    void SetUniforms(const ShaderPtr &shader) override {
        shader->SetVec3("uDiffuseColor", diffuseColor_);
        shader->SetVec3("uSpecularColor", specularColor_);
        shader->SetFloat("uShininess", shininess_);

        if (diffuseTexture_) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseTexture_->GetHandle());
            shader->SetInt("uDiffuseMap", 0);
            shader->SetBool("uHasDiffuseMap", true);
        } else {
            shader->SetBool("uHasDiffuseMap", false);
        }

        if (specularTexture_) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, specularTexture_->GetHandle());
            shader->SetInt("uSpecularMap", 1);
            shader->SetBool("uHasSpecularMap", true);
        } else {
            shader->SetBool("uHasSpecularMap", false);
        }

        if (normalTexture_) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, normalTexture_->GetHandle());
            shader->SetInt("uNormalMap", 2);
            shader->SetBool("uHasNormalMap", true);
        } else {
            shader->SetBool("uHasNormalMap", false);
        }
    }

private:
    glm::vec3 diffuseColor_;
    glm::vec3 specularColor_;
    float shininess_;

    TexturePtr diffuseTexture_;
    TexturePtr specularTexture_;
    TexturePtr normalTexture_;
};


class PbrMaterial : public Material {
public:
    PbrMaterial() = default;

private:
    glm::vec3 albedo_;
    float metallic_;
    float roughness_;
    float ao_;

    TexturePtr albedoTexture_;
    TexturePtr metallicTexture_;
    TexturePtr roughnessTexture_;
    TexturePtr aoTexture_;
};





}