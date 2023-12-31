#version 330 core

// Interpolated values from the vertex shaders
in vec4 vShadowCoord;
in vec2 vUv;

// Ouput data
layout(location = 0) out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D shadowMap;
uniform vec3 uColor;
uniform sampler2D uColorMap;
uniform bool uHasColorMap = false;

vec2 poissonDisk[16] = vec2[](
    vec2( -0.94201624, -0.39906216 ),
    vec2( 0.94558609, -0.76890725 ),
    vec2( -0.094184101, -0.92938870 ),
    vec2( 0.34495938, 0.29387760 ),
    vec2( -0.91588581, 0.45771432 ),
    vec2( -0.81544232, -0.87912464 ),
    vec2( -0.38277543, 0.27676845 ),
    vec2( 0.97484398, 0.75648379 ),
    vec2( 0.44323325, -0.97511554 ),
    vec2( 0.53742981, -0.47373420 ),
    vec2( -0.26496911, -0.41893023 ),
    vec2( 0.79197514, 0.19090188 ),
    vec2( -0.24188840, 0.99706507 ),
    vec2( -0.81409955, 0.91437590 ),
    vec2( 0.19984126, 0.78641367 ),
    vec2( 0.14383161, -0.14100790 )
);

void main(){
    vec3 tex = uColor;
    if (uHasColorMap) {
        tex = texture(uColorMap, vUv).rgb;
    }

    // the vShadowCoord.z / vShadowCoord.w is between [-1, 1], we need to
    // convert it to [0, 1], which is the range of shaowMap texture
    vec3 shadowCoord = vShadowCoord.xyz / vShadowCoord.w * 0.5 + 0.5;


    // -0.005 to suppress the shadow
    float bias = 0.005;
    float visibility = 1.0;
    // Sample the shadow map 4 times
    for (int i=0;i<4;i++){
        // use either :
        //  - Always the same samples.
        //    Gives a fixed pattern in the shadow, but no noise
        int index = i;
        //  - A random sample, based on the pixel's screen location.
        //    No banding, but the shadow moves with the camera, which looks weird.
        // int index = int(16.0*random(gl_FragCoord.xyy, i))%16;
        //  - A random sample, based on the pixel's position in world space.
        //    The position is rounded to the millimeter to avoid too much aliasing
        // int index = int(16.0*random(floor(Position_worldspace.xyz*1000.0), i))%16;

        // being fully in the shadow will eat up 4*0.2 = 0.8
        // 0.2 potentially remain, which is quite dark.
        float depth_shadowMap = texture(shadowMap, shadowCoord.xy + poissonDisk[index]/700.0).r;
        visibility -= 0.16 * (1.0 - step(shadowCoord.z - bias, depth_shadowMap));
    }

//    float depth_shadowMap = texture(shadowMap, shadowCoord.xy).r;
//    visibility = mix(0.2, 1.0, step(shadowCoord.z - 0.001, depth_shadowMap));

    // Light emission properties
    vec3 lightColor = vec3(1,1,1);

    color = visibility * tex * lightColor;

}