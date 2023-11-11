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
uniform float uMetalness = 0.0;
uniform float uRoughness = 0.5;

// debug
uniform float uDiffuseCoef = 1.0;
uniform float uSpecularCoef = 1.0;

// Lights
#define MAX_LIGHTS 4
uniform int uLightCount = 0;
uniform vec3 uLightPositions[MAX_LIGHTS];
uniform vec3 uLightColors[MAX_LIGHTS];

const float PI = 3.14159265359;
const float Epsilon = 0.00001;

vec3 rgbToLinear(vec3 rgb) {
    return pow(rgb, vec3(2.2));
}

vec3 linearToRgb(vec3 color) {
    return pow(color, vec3(1.0 / 2.2));
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

void main() {
    // get base color (albedo) in linear space
    vec3 baseColor = rgbToLinear(uBaseColor);
    if (uHasBaseColorMap) {
        baseColor = rgbToLinear(texture(uBaseColorMap, vUv).rgb);
    }

    // get roughness, metallic, occlusion
    float roughness = uRoughness;
    float metallic = uMetalness;

    // Fresnel paramter.
    // For dielectrics, F0 is usually 0.04;
    // For metals, use the base color as F0.
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, baseColor, metallic);

    // direction vectors
    vec3 N = normalize(vNormalW);           // normal vector
    vec3 V = normalize(uCamPosW - vPosW);    // view vector

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
        float NdV = max(dot(N, V), 0.0);
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
        directLo += (uDiffuseCoef * diffuseDirect + uSpecularCoef * specularDirect) * lightRadiance * NdL;
    }

    // calculate ambient lighting
    vec3 ambientLo = baseColor * vec3(0.03);

    // final color
    vec3 color = directLo + ambientLo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correction
    color = linearToRgb(color);

    fragColor = vec4(color, 1.0);

}

