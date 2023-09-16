#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <map>

namespace vivid {

struct Uniform {
    std::string name;
    unsigned int type;
};

class Shader {
public:
    Shader();

    Shader(const char* vertexShaderCode, const char* fragmentShaderCode);

    void Use() const;

    // Set uniforms
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetMat4(const std::string& name, const glm::mat4 &m) const;
    void SetMat3(const std::string& name, const glm::mat3 &m) const;
    void SetVec3(const std::string& name, const glm::vec3 &v) const;
    void SetVec4(const std::string& name, const glm::vec4 &v) const;

    const std::map<int, std::string>& AttributeLocations() const {
        return attributeLocations_;
    }

    const std::string& AttributeLocationsStr() const {
        return attributeLocationsStr_;
    }

    bool HasUniform(const std::string& name) const {
        return activeUniforms_.count(name) > 0;
    }

private:
    void Create(const char* vertexShaderCode, const char* fragmentShaderCode);

    void ExtractAttributeLocations();

    void ExtractUniformLocations();

    unsigned int programHandle_;

    std::map<int, std::string> attributeLocations_;
    std::string attributeLocationsStr_;

    std::map<std::string, Uniform> activeUniforms_;
};

using ShaderPtr = std::shared_ptr<Shader>;

} // namespace vivid
