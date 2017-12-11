#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} camera;

// TODO: Declare fragment shader inputs
layout(location = 0) in vec2 fs_uv;
layout(location = 1) in vec3 fs_normal;
layout(location = 2) in vec4 fs_color;
layout(location = 3) in vec4 fs_pos;

layout(location = 0) out vec4 outVisibility;

void main() {
	outVisibility = vec4(fs_pos.x, fs_uv.x, fs_pos.z, fs_uv.y);
}
