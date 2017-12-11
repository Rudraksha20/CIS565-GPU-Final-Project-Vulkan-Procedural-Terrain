#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
    vec3 cameraPos;
} camera;

layout (set = 2, binding = 0) uniform sampler2D samplerGrass;

// TODO: Declare fragment shader inputs
layout(location = 0) in vec2 fs_uv;
layout(location = 1) in vec3 fs_normal;
layout(location = 2) in vec4 fs_color;
layout(location = 3) in vec4 fs_pos;

layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outPosition;
layout(location = 2) out vec4 outNormal;

void main() {
	outAlbedo = mix(texture(samplerGrass, fs_uv), vec4(1.0, 0.98, 0.98, 1.0), smoothstep(1.0, 5.0, fs_pos.y));
    // make number smaller so more of its precision is preserved
    // when converting to tiny 16-bit float
    const float tileDim = 15.0;

    // This is essentially undoing the step in the compute shader where we move tiles
    // to be closer to the camera
    const float offsetX = floor(camera.cameraPos.x / tileDim) * tileDim;
    const float offsetZ = floor(camera.cameraPos.z / tileDim) * tileDim;
	outPosition = fs_pos - vec4(offsetX, 0.0, offsetZ, 0.0);
	outNormal = vec4(fs_normal, 0.0);
}