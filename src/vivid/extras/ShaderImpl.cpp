#include <memory>
#include <string>
#include <fstream>
#include "vivid/extras/ShaderImpl.h"


namespace vivid {

// ============= vertex colored shader =============
const std::string vertex_colored_vs = R"(
#version 330 core

// input
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

// uniforms
uniform mat4 MVP;
uniform float uPointSize = 1.0; // for rendering points

// output
out vec3 fragColor;

void main() {
    gl_Position = MVP * vec4(position, 1.0);
    gl_PointSize = uPointSize;
    fragColor = color;
}

)";


const std::string vertex_colored_fs = R"(
#version 330 core

// input
in vec3 fragColor;

// output
out vec3 color;

void main() {
    color = fragColor;
}

)";



// ============= colored basic shader =============
const std::string colored_basic_vs = R"(
#version 330 core

// input
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord0;

// uniforms
uniform mat4 MVP;

// output
out vec2 vUv;

void main() {
    gl_Position = MVP * vec4(position, 1.0);
    vUv = texCoord0;
}
)";

const std::string colored_basic_fs = R"(
#version 330 core

// input
in vec2 vUv;

// uniforms
uniform vec3 uColor = vec3(1.0, 1.0, 1.0);
uniform sampler2D uColorMap;
uniform bool uHasColorMap = false;

// output
out vec3 color;

void main() {
    color = uColor;
    if (uHasColorMap) {
        color *= texture(uColorMap, vUv).rgb;
    }
}

)";




// ============= basic shading shader ===================
const std::string basic_shading_vs = R"(
#version 330 core

// Input vertex data
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord0;

// Output data
out vec2 vUv;
out vec3 vNormal;   // normal vector in camera space

// Uniforms
uniform mat4 MVP;
uniform mat3 normalMatrix;  // inverse transpose of modelview matrix, for transforming normals from object space to camera space


void main() {
    vUv = texCoord0;
    vNormal = normalize(normalMatrix * normal);
    gl_Position = MVP * vec4(position, 1.0);
}
)";

const std::string basic_shading_fs = R"(
#version 330 core

// input values from the vertex shaders
in vec2 vUv;
in vec3 vNormal;

// output data
out vec3 color;

// texture
uniform vec3 uColor;
uniform sampler2D uColorMap;
uniform bool uHasColorMap = false;

void main() {
    vec3 light = vec3(0.5, 0.2, 1.0);
    vec3 normal = normalize(vNormal);
    vec3 tex = uColor;
    if (uHasColorMap) {
        tex = texture(uColorMap, vUv).rgb;
    }
    float shading = dot(normal, light) * 0.15;
    color = tex + shading;
}
)";



// ============= colored blinn phong shader =============
const std::string colored_blinn_phong_vs = R"(
#version 330 core

// input
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

// output
out vec3 vNormalC;      // fragment normal in camera space
out vec3 vPositionC;    // fragment position in camera space

// Uniforms
uniform mat4 MVP;
uniform mat3 normalMatrix; // convert normal from model space to camera space
uniform mat4 modelViewMatrix;

void main() {
    // Output position of the vertex, in clip space.
    gl_Position = MVP * vec4(position, 1);

    // normal in the camera space
    vNormalC = normalize(normalMatrix * normal);

    // position in the camera space
    vPositionC = (modelViewMatrix * vec4(position, 1)).xyz;
}

)";

const std::string colored_blinn_phong_fs = R"(
#version 330 core

// Interpolated values from the vertex shaders
in vec3 vNormalC;       // normal in camera space
in vec3 vPositionC;     // position in camera space

// output data
out vec3 color;

// uniforms
struct Light {
    vec3 positionC;     // position in camera space
    vec3 color;
    float intensity;
    int type;           // 0: directional, 1: point, 2: spot
};

struct Material {
    vec3 diffuseColor;
    vec3 specularColor;
    float shininess;
};

uniform Light light;
uniform vec3 ambientColor;
uniform Material material;

void main() {
    // light direction (from fragment to light)
    vec3 lightDir;
    if (light.type == 0) {
        lightDir = normalize(-light.positionC);
    } else {
        lightDir = normalize(light.positionC - vPositionC);
    }

    // camera direction (from fragment to camera)
    vec3 viewDir = normalize(-vPositionC);

    // diffuse
    float diffuse = max(dot(vNormalC, lightDir), 0.0);

    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specular = pow(max(dot(vNormalC, halfwayDir), 0.0), material.shininess);

    // ambient
    vec3 ambient = ambientColor * material.diffuseColor;

    // final color
    color = ambient + light.color * light.intensity
          * (diffuse * material.diffuseColor + specular * material.specularColor);
}
)";



// ============= colored basic shader =============
const std::string ground_vs = R"(
#version 330 core

// Input vertex data
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord0;

// Output data
out vec2 vUv;
out vec3 vNormal;   // normal vector in camera space

// Uniforms
uniform mat4 MVP;
uniform mat3 normalMatrix;  // inverse transpose of modelview matrix, for transforming normals from object space to camera space

void main() {
    vUv = texCoord0;
    vNormal = normalize(normalMatrix * normal);
    gl_Position = MVP * vec4(position, 1.0);
}
)";

const std::string ground_fs = R"(
#version 330 core

// input values from the vertex shaders
in vec2 vUv;
in vec3 vNormal;

// output data
out vec3 color;

// texture
uniform sampler2D diffuseMap;

void main() {
    vec3 light = vec3(0.5, 0.2, 1.0);
    vec3 normal = normalize(vNormal);
    vec3 tex = texture(diffuseMap, vUv).rgb;
    float shading = dot(normal, light) * 0.15;
    color = 0.5 * tex + shading;
}
)";


const std::string depth_vs = R"(
#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(position, 1);
}
)";


const std::string depth_fs = R"(
#version 330 core

// output data
layout(location = 0) out float fragmentDepth;

void main() {
    fragmentDepth = gl_FragCoord.z;
}
)";




// ============= 2D screen shader =============
const std::string screen_shader_vs = R"(
#version 330 core

// input
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord0;

// output
out vec2 vUv;

void main()
{
    vUv = texCoord0;
    gl_Position = vec4(position.xy, 0.0, 1.0);
}
)";

const std::string screen_shader_fs = R"(
#version 330 core

in vec2 vUv;

out vec3 color;

uniform float uTime;
uniform vec3 uColor;

void main() {
    color = vec3(0.3, 0.3, 0.3) + 0.3 * cos(vUv.xyx + uTime) + uColor;
}
)";



ShaderPtr ShaderImpl::GetVertexColoredShader() {
    static ShaderPtr shader = std::make_shared<Shader>(vertex_colored_vs.c_str(), vertex_colored_fs.c_str());
    return shader;
}

ShaderPtr ShaderImpl::GetColoredBasicShader() {
    static ShaderPtr shader = std::make_shared<Shader>(colored_basic_vs.c_str(), colored_basic_fs.c_str());
    return shader;
}

ShaderPtr ShaderImpl::GetBasicShadingShader() {
    static ShaderPtr shader = std::make_shared<Shader>(basic_shading_vs.c_str(), basic_shading_fs.c_str());
    return shader;
}

ShaderPtr ShaderImpl::GetColoredBlinnPhongShader() {
    static ShaderPtr shader = std::make_shared<Shader>(colored_blinn_phong_vs.c_str(), colored_blinn_phong_fs.c_str());
    return shader;
}

ShaderPtr ShaderImpl::GetGroundShader() {
    static ShaderPtr shader = std::make_shared<Shader>(ground_vs.c_str(), ground_fs.c_str());
    return shader;
}

ShaderPtr ShaderImpl::GetDepthShader() {
    static ShaderPtr shader = std::make_shared<Shader>(depth_vs.c_str(), depth_fs.c_str());
    return shader;
}

ShaderPtr ShaderImpl::GetScreenShader() {
    static ShaderPtr shader = std::make_shared<Shader>(screen_shader_vs.c_str(), screen_shader_fs.c_str());
    return shader;
}


//ShaderPtr ShaderImpl::GetTexturedBasicShader() {
//    static ShaderPtr shader = std::make_shared<Shader>(textured_basic_vs, textured_basic_fs);
//    return shader;
//}
//
//ShaderPtr ShaderImpl::GetTexturedBlinnPhongShader() {
//    static ShaderPtr shader = std::make_shared<Shader>(textured_blinn_phong_vs, textured_blinn_phong_fs);
//    return shader;
//}

ShaderPtr ShaderImpl::LoadShader(const std::string &vertexShaderPath, const std::string &fragShaderPath) {
    std::ifstream ifs(vertexShaderPath);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open vertex shader file: " << vertexShaderPath << std::endl;
        exit(-1);
    }
    std::string vertexShaderSource((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    ifs.close();

    ifs.open(fragShaderPath);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open fragment shader file: " << fragShaderPath << std::endl;
        exit(-1);
    }
    std::string fragShaderSource((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    ifs.close();

    return std::make_shared<Shader>(vertexShaderSource.c_str(), fragShaderSource.c_str());
}



} // namespace vivid