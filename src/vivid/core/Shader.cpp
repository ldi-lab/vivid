#include <fstream>
#include <sstream>
#include <vector>
#include <glad/glad.h>
#include "vivid/core/Shader.h"
#include "vivid/core/Attribute.h"


namespace vivid {

bool CompileShader(GLuint shaderHandle, const char* shaderCode) {
    glShaderSource(shaderHandle, 1, &shaderCode, nullptr);
    glCompileShader(shaderHandle);

    // Check vertex shader
    GLint result = GL_FALSE;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int infoLength;
        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &infoLength);
        std::vector<char> errMessage(infoLength + 1);
        glGetShaderInfoLog(shaderHandle, infoLength, nullptr, &errMessage[0]);
        printf("%s\n", &errMessage[0]);
        return false;
    }
    return true;
}


Shader::Shader() {
    programHandle_ = 0;
}


Shader::Shader(const char* vertexShaderCode, const char* fragmentShaderCode) {
    programHandle_ = 0;
    Create(vertexShaderCode, fragmentShaderCode);
}


void Shader::Create(const char *vertexShaderCode, const char *fragmentShaderCode) {

    // Create the shaders
    GLuint vertShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Compile vertex shader and check
    if (!CompileShader(vertShaderID, vertexShaderCode)) {
        std::cerr << "failed to compile vertex shader!";
        exit(-1);
    }

    // Compile fragment shader and check
    if (!CompileShader(fragShaderID, fragmentShaderCode)) {
        std::cerr << "failed to compile fragment shader!";
        exit(-1);
    }

    // Link the program
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertShaderID);
    glAttachShader(programID, fragShaderID);
    glLinkProgram(programID);

    // Check the program
    GLint ret;
    glGetProgramiv(programID, GL_LINK_STATUS, &ret);
    if (ret == GL_FALSE) {
        int infoLength;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLength);
        std::vector<char> errMessage(infoLength + 1);
        glGetProgramInfoLog(programID, infoLength, nullptr, &errMessage[0]);
        printf("%s\n", &errMessage[0]);
        std::cerr << "failed to link program!";
        exit(-1);
    }

    // Detach program and delete shaders
    glDetachShader(programID, vertShaderID);
    glDetachShader(programID, fragShaderID);
    glDeleteShader(vertShaderID);
    glDeleteShader(fragShaderID);

    programHandle_ = programID;

    // Get attribute locations
    ExtractAttributeLocations();

    // Extract uniforms
    ExtractUniformLocations();

}


void Shader::ExtractAttributeLocations() {
    glUseProgram(programHandle_);
    attributeLocations_.clear();
    for (int i = 0; i < kAttribNum; i++) {
        auto attrType = static_cast<AttributeType>(i);
        std::string name = AttributeName(attrType);
        int loc = glGetAttribLocation(programHandle_, name.c_str());
        if (loc >= 0) {
            if (attributeLocations_.count(loc) > 0) {
                std::cerr << "Error: find repeated attribute locations in the shader!\n";
                exit(-1);
            }
            attributeLocations_[loc] = name;
            std::cout << "attribute=" << name << ", location=" << loc << std::endl;
        }
    }

    // Convert attribute locations to a string
    std::stringstream ss;
    for (auto& it : attributeLocations_) {
        ss << it.second << "_" << it.first << "_";
    }
    attributeLocationsStr_ = ss.str();
    std::cout << "attribute locations str: " << attributeLocationsStr_ << std::endl;
}


void Shader::ExtractUniformLocations() {
    glUseProgram(programHandle_);
    activeUniforms_.clear();
    int numUniforms = 0;
    glGetProgramiv(programHandle_, GL_ACTIVE_UNIFORMS, &numUniforms);
    std::vector<GLchar> nameData(32);
    for (int i = 0; i < numUniforms; i++) {
        GLsizei arraySize = 0;
        GLenum type = 0;
        GLsizei actualLength = 0;
        glGetActiveUniform(programHandle_, (GLuint)i, (GLsizei)nameData.size(), &actualLength, &arraySize, &type, &nameData[0]);
        std::string name((char*)nameData.data(), actualLength);
        Uniform uniform;
        uniform.name = name;
        uniform.type = static_cast<unsigned int>(type);
        activeUniforms_[name] = uniform;
        std::cout << "uniform=" << name << ", type=" << type << std::endl;
    }
}


void Shader::Use() const {
    // Bind
    glUseProgram(programHandle_);

}


void Shader::SetBool(const std::string &name, bool value) const {
    CheckUniformName(name);
    glUniform1i(glGetUniformLocation(programHandle_, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string &name, int value) const {
    CheckUniformName(name);
    glUniform1i(glGetUniformLocation(programHandle_, name.c_str()), value);
}

void Shader::SetFloat(const std::string &name, float value) const {
    CheckUniformName(name);
    glUniform1f(glGetUniformLocation(programHandle_, name.c_str()), value);
}

void Shader::SetMat4(const std::string &name, const glm::mat4 &m) const {
    CheckUniformName(name);
    glUniformMatrix4fv(glGetUniformLocation(programHandle_, name.c_str()),
                                 1, GL_FALSE, &m[0][0]);
}

void Shader::SetMat3(const std::string &name, const glm::mat3 &m) const {
    CheckUniformName(name);
    glUniformMatrix3fv(glGetUniformLocation(programHandle_, name.c_str()),
                                 1, GL_FALSE, &m[0][0]);
}

void Shader::SetVec3(const std::string &name, const glm::vec3 &v) const {
    CheckUniformName(name);
    glUniform3fv(glGetUniformLocation(programHandle_, name.c_str()), 1, &v[0]);
}

void Shader::SetVec4(const std::string &name, const glm::vec4 &v) const {
    CheckUniformName(name);
    glUniform4fv(glGetUniformLocation(programHandle_, name.c_str()), 1, &v[0]);
}


void Shader::CheckUniformName(const std::string &name) const {
    if (!HasUniform(name)) {
        std::cerr << "Warning: uniform (" << name << ") not found in the shader!\n";
    }
}


} // vivid