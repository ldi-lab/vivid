#version 330 core

// Interpolated values from the vertes shaders
in vec3 vNormal;             // normal in camera space
in vec3 vPosition;            // position in camera space

// Output data
out vec3 color;

// Uniforms
struct Light {
    vec4 positionC; // position in camera space
    vec3 color;
    float intensity;
};

struct Material {
    vec3 diffuseColor;
    vec3 specularColor;
    float shininess;
};

uniform vec3 envAmbientColor;    // environment ambient color
uniform Light light;
uniform Material material;

void main() {

    // light direction
    vec3 lightDir;
    if (light.positionC.w == 0.0) {
        // directional light
        lightDir = normalize(light.positionC.xyz);
    } else {
        // point light
        lightDir = normalize(light.positionC.xyz - vPosition);
    }

    // camera direction
    vec3 camDir = -normalize(vPosition);

    // Diffuse
    float diffuseIntensity = max(dot(lightDir, vNormal), 0.0);

    // Specular
    vec3 H = normalize(lightDir + camDir);
    float specularIntensity = pow(max(dot(vNormal, H), 0.0), material.shininess);

    color = envAmbientColor * material.diffuseColor
             + diffuseIntensity * material.diffuseColor * light.color * light.intensity
             + specularIntensity * material.specularColor * light.color * light.intensity;
}