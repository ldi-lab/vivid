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
uniform float uOccusion = 0.0;
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

