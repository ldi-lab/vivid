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
const std::string blinn_phong_vs = R"(
#version 330 core

// input
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord0;

// output
out vec3 vNormalC;      // fragment normal in camera space
out vec3 vPositionC;    // fragment position in camera space
out vec2 vUv;

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

    vUv = texCoord0;
}

)";

const std::string blinn_phong_fs = R"(
#version 330 core

// Interpolated values from the vertex shaders
in vec3 vNormalC;       // normal in camera space
in vec3 vPositionC;     // position in camera space
in vec2 vUv;

// output data
out vec3 color;

// uniforms
struct Light {
    vec3 positionC;     // position in camera space
    vec3 color;
    int type;           // 0: directional, 1: point, 2: spot
};

uniform vec3 uDiffuseColor;
uniform vec3 uSpecularColor;
uniform float uShininess = 5.0;

uniform sampler2D uDiffuseMap;
uniform bool uHasDiffuseMap = false;

uniform sampler2D uNormalMap;
uniform bool uHasNormalMap = false;

uniform sampler2D uSpecularMap;
uniform bool uHasSpecularMap = false;

uniform Light uLight;
uniform vec3 uAmbientColor;

void main() {
    // light direction (from fragment to light)
    vec3 lightDir;
    if (uLight.type == 0) {
        lightDir = normalize(-uLight.positionC);
    } else {
        lightDir = normalize(uLight.positionC - vPositionC);
    }

    // camera direction (from fragment to camera)
    vec3 viewDir = normalize(-vPositionC);

    // normal
    vec3 normal = normalize(vNormalC);
    if (uHasNormalMap) {
        //TODO: use normal map
        normal = normalize(texture(uNormalMap, vUv).rgb * 2.0 - 1.0);
    }

    // diffuse
    float diffuseContrib = max(dot(normal, lightDir), 0.0);

    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specularContrib = pow(max(dot(normal, halfwayDir), 0.0), uShininess);

    // material diffuse and specular color
    vec3 materialDiffuseColor = uDiffuseColor;
    if (uHasDiffuseMap) {
        materialDiffuseColor *= texture(uDiffuseMap, vUv).rgb;
    }
    vec3 materialSpecularColor = uSpecularColor;
    if (uHasSpecularMap) {
        materialSpecularColor *= texture(uSpecularMap, vUv).rgb;
    }

    // final color
    color = uAmbientColor * materialDiffuseColor + uLight.color
          * (diffuseContrib * materialDiffuseColor + specularContrib * materialSpecularColor);
}
)";




// ============= PBR shader ===============
const std::string pbr_vs = R"(
#version 330 core

// input vertex data
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord0;

// output data
out vec2 vUv;
out vec3 vNormalW;   // normal vector in world space
out vec3 vPosW;      // vertex position in world space

// uniforms
uniform mat4 MVP;
uniform mat4 modelMatrix;
uniform mat3 normalMatrixW;  // inverse transpose of model rotation matrix

void main() {
    vUv = texCoord0;

    // transform normals from model space to world space
    vNormalW = normalize(normalMatrixW * normal);

    // transform vertex position from model space to world space
    vPosW = vec3(modelMatrix * vec4(position, 1.0));

    gl_Position = MVP * vec4(position, 1.0);
}
)";

const std::string pbr_fs = R"(
#version 330 core

// input data
in vec2 vUv;
in vec3 vNormalW;   // normal vector in world space
in vec3 vPosW;      // vertex position in world space

// output data
out vec4 fragColor;

// uniform variables
uniform vec3 uCamPosW;   // camera position in world space

// material properties
uniform sampler2D uBaseColorMap;
uniform bool uHasBaseColorMap = false;
uniform vec3 uBaseColor = vec3(1);

uniform sampler2D uRmoMap;
uniform float uMetalness = 0.0;
uniform float uRoughness = 0.5;
//uniform float uOccusion = 0.0;
uniform bool uHasRmoMap = false;

uniform sampler2D uOpacityMap;
uniform bool uHasOpacityMap = false;

uniform sampler2D uEmissiveMap;
uniform bool uHasEmissiveMap = false;

// envarionment lighting
uniform sampler2D uEnvIrradianceMap;
uniform sampler2D uBrdfLutMap;
uniform sampler2D uEnvSpecularMap;
uniform bool uEnableIBL = false;

// Lights
#define MAX_LIGHTS 4
uniform int uLightCount = 0;
uniform vec3 uLightPositions[MAX_LIGHTS];
uniform vec3 uLightColors[MAX_LIGHTS];

const float PI = 3.14159265359;
const float INV_PI = 0.31830988618;
const float INV_PI2 = 0.15915494309;
const float LN2 = 0.69314718056;
const float Epsilon = 0.00001;
const float ENV_LOD = 6.0;  // environment map lod
const float LOD_YOFFSETS[8] = float[](0.0, 0.5, 0.75, 0.875, 0.9375, 0.96875, 0.984375, 0.9921875);

vec3 rgbToLinear(vec3 rgb) {
    return pow(rgb, vec3(2.2));
}

vec3 linearToRgb(vec3 color) {
    return pow(color, vec3(1.0 / 2.2));
}

vec4 rgbmToLinear(vec4 value) {
    float maxRange = 6.0;
    return vec4(value.rgb * value.a * maxRange, 1.0);
}

float ndfGGx(float NdotH, float roughness) {
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = NdotH * NdotH * (alpha2 - 1.0) + 1.0;
    return alpha2 / (PI * denom * denom);
}

float geometryOcculusion(float NdL, float NdV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    float gl = NdL / (NdL * (1.0 - k) + k);
    float gv = NdV / (NdV * (1.0 - k) + k);
    return gl * gv;
}

vec3 fresnelSchlick(float HdV, vec3 F0) {
    return F0 + (vec3(1.0) - F0) * pow(clamp(1.0 - HdV, 0.0, 1.0), 5.0);
}

vec2 cartesianToPolar(vec3 v) {
    float px = atan(v.z, v.x) * INV_PI2 + 0.5;
    float py = asin(v.y) * INV_PI + 0.5;
    return vec2(px, py);
}

void main() {
    // get base color (albedo) in linear space
    vec3 baseColor = rgbToLinear(uBaseColor);
    if (uHasBaseColorMap) {
        baseColor = rgbToLinear(texture(uBaseColorMap, vUv).rgb);
    }

    // get roughness, metallic, occlusion
    // RMO map is encoded as RGB = [Roughness, Metallic, Occlusion]
    float roughness = uRoughness;
    float metallic = uMetalness;
    if (uHasRmoMap) {
        vec4 rmoSample = texture(uRmoMap, vUv);
        roughness = clamp(rmoSample.r, 0.04, 1.0);
        metallic = clamp(rmoSample.g, 0.04, 1.0);
    }

    // get base alpha
    float alpha = 1.0;
    if (uHasOpacityMap) {
        alpha *= texture(uOpacityMap, vUv).g;
    }

    // get emissive color
    vec3 emissive = vec3(0);
    if (uHasEmissiveMap) {
        emissive = rgbToLinear(texture(uEmissiveMap, vUv).rgb);
    }

    // Fresnel paramter.
    // For dielectrics, F0 is usually 0.04;
    // For metals, use the base color as F0.
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, baseColor, metallic);

    // direction vectors
    vec3 N = normalize(vNormalW);              // normal vector
    vec3 V = normalize(uCamPosW - vPosW);    // view vector
    vec3 R = normalize(reflect(-V, N));                     // specular reflection vector
    float NdV = max(dot(N, V), 0.0);

    // calculate direct lighting output
    vec3 directLo = vec3(0);
    for (int i = 0; i < uLightCount; ++i) {
        vec3 L = normalize(uLightPositions[0] - vPosW);   // light direction vector
        vec3 H = normalize(V + L);                        // half vector

        // light attenuation
        float distance = length(uLightPositions[i] - vPosW);
        float attenuation = 1.0 / (distance * distance);
        vec3 lightRadiance = uLightColors[i] * attenuation;

        // Cook-Torrrance BRDF
        float NdH = max(dot(N, H), 0.0);
        float NdL = max(dot(N, L), 0.0);
        float HdV = max(dot(H, V), 0.0);
        float D = ndfGGx(NdH, roughness);
        float G = geometryOcculusion(NdL, NdV, roughness);
        vec3 F = fresnelSchlick(HdV, F0);

        // multiply the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partially metal
        vec3 diffuseDirect = (vec3(1.0) - F) *  (1.0 - metallic) * baseColor / PI;

        // specular part
        vec3 specularDirect = F * D * G / max(4.0 * NdL * NdV, Epsilon);

        // total contribution of this light
        directLo += (diffuseDirect + specularDirect) * lightRadiance * NdL;

        // add light specular to alpha for reflections on transparent surfaces (glass)
        alpha = max(alpha, max(specularDirect.r, max(specularDirect.g, specularDirect.b)));
    }

    // calculate indirect lighting
    vec3 ambientLo = baseColor * vec3(0.03);
    if (uEnableIBL) {
        /* diffuse part */
        vec3 diffuseIrradiance = rgbmToLinear(texture(uEnvIrradianceMap, cartesianToPolar(N))).rgb;
        // calculate the Fresnel term for ambient lighting. SInce we use the pre-filtered map
        // and irradiance is comming from many directions, we use the reflectance at normal incidence
        // as the F0 term
        vec3 F = fresnelSchlick(NdV, F0);
        vec3 kd = (vec3(1.0) - F) * (1.0 - metallic);
        vec3 diffuseIBL = diffuseIrradiance * kd * baseColor;

        /* specular part */
        // sample the pre-filtered specular map, the mipmap level is based on the roughness value
        float lod = roughness * ENV_LOD;
        float level0 = floor(lod);
        float level1 = min(ENV_LOD, level0 + 1.0);
        float weight1 = lod - level0;

        // sample the specular env map atlas depending on the roughness value
        vec2 uvPano = cartesianToPolar(R);
        uvPano.y /= 2.0;
        vec2 uv0 = uvPano / pow(2.0, level0);
        uv0.y += LOD_YOFFSETS[int(level0)];
        vec2 uv1 = uvPano / pow(2.0, level1);
        uv1.y += LOD_YOFFSETS[int(level1)];
        uv0.y = 1.0 - uv0.y;
        uv1.y = 1.0 - uv1.y;

        vec3 envSpecular0 = rgbmToLinear(texture(uEnvSpecularMap, uv0)).rgb;
        vec3 envSpecular1 = rgbmToLinear(texture(uEnvSpecularMap, uv1)).rgb;
        vec3 specularIrradiance = mix(envSpecular0, envSpecular1, weight1);

        // sample the BRDF LUT texture
        vec3 brdf = rgbToLinear(texture(uBrdfLutMap, vec2(NdV, roughness)).rgb);

        // bit of extra reflection for smooth materials
        float reflectivity = pow(1.0 - roughness, 2.0) * 0.05;
        vec3 specularIBL = (F0 * brdf.x + brdf.y + reflectivity) * specularIrradiance;

        ambientLo = diffuseIBL + specularIBL;

        // add IBL specular to alpha for reflections on transparent surfaces (glass)
        alpha = max(alpha, max(specularIBL.r, max(specularIBL.g, specularIBL.b)));
    }

    // final color
    vec3 color = directLo + ambientLo + emissive;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correction
    color = linearToRgb(color);

    fragColor = vec4(color, alpha);

}
)";



// ============= ground shader =============
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

ShaderPtr ShaderImpl::GetBlinnPhongShader() {
    static ShaderPtr shader = std::make_shared<Shader>(blinn_phong_vs.c_str(), blinn_phong_fs.c_str());
    return shader;
}

ShaderPtr ShaderImpl::GetPBRShader() {
    static ShaderPtr shader = std::make_shared<Shader>(pbr_vs.c_str(), pbr_fs.c_str());
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