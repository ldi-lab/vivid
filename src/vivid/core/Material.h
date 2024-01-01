#pragma once

#include <iostream>
#include <memory>
#include "vivid/core/Shader.h"


namespace vivid {

class Material {
public:
    Material() = default;

    virtual void SetUniforms(const ShaderPtr &shader) = 0;

};

using MaterialPtr = std::shared_ptr<Material>;

}