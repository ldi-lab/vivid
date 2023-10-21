#pragma once

#include <iostream>
#include "vivid/core/Shader.h"

namespace vivid {

class ShaderImpl {
public:
    ShaderImpl() = default;

    static ShaderPtr GetVertexColoredShader();

    static ShaderPtr GetColoredBasicShader();

    static ShaderPtr GetTexturedBasicShader();

    static ShaderPtr GetColoredBlinnPhongShader();

    static ShaderPtr GetTexturedBlinnPhongShader();

    static ShaderPtr GetPBRShader();

    static ShaderPtr GetToonShader();

    static ShaderPtr GetGroundShader();

    static ShaderPtr LoadShader(const std::string& vertexShaderPath, const std::string& fragShaderPath);

};

} // namespace vivid