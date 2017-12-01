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

layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outPosition;
layout(location = 2) out vec4 outNormal;

void main() {
#if 0
	if (fs_color.w > 0.0) {
		// use custom color in fs_color
		outAlbedo = vec4(fs_color.xyz, 1.0);
	}
	else {
		// use green + lambert shading
		const vec3 lightDir = vec3(-0.577350269, 0.577350269, 0.577350269);
		float lambert = max(dot(fs_normal, lightDir), dot(-fs_normal, lightDir));
		lambert = clamp(lambert, 0.25, 1.0) * 0.5 + 0.5;
		vec3 color = vec3(0.1, 0.9, 0.2) * lambert;
		outAlbedo = vec4(color, 1.0);
	}
	
	// Lambertian Shading
	vec3 lightDirection = -normalize(vec3(2.0, 1.0, 2.0));
	vec3 color = vec3(0.75);
	float ambient = 0.2;
	float dotProd = (dot(normalize(fs_normal), lightDirection));
	color = color * dotProd + ambient;
#endif

	outAlbedo = vec4(0.75, 0.75, 0.75, 1.0);
	outPosition = fs_pos;
	outNormal = vec4(fs_normal, 0.0);
}