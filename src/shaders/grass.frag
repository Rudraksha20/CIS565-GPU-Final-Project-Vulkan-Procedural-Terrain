#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
	vec3 cameraPos;
} camera;

// TODO: Declare fragment shader inputs
layout(location = 0) in vec2 fs_uv;
layout(location = 1) in vec3 fs_normal;
layout(location = 2) in vec4 fs_color;
layout(location = 3) in vec3 fs_pos;

layout(location = 0) out vec4 outColor;

void main() {
	if (fs_color.w > 0.0) {
		// use custom color in fs_color
		outColor = vec4(fs_color.xyz, 1.0);
	}
	else {
		// use green + lambert shading
		const vec3 lightDir = vec3(-0.577350269, 0.577350269, 0.577350269);
		float lambert = max(dot(fs_normal, lightDir), dot(-fs_normal, lightDir));
		lambert = clamp(lambert, 0.25, 1.0) * 0.5 + 0.5;
		vec3 color = vec3(0.1, 0.9, 0.2) * lambert;
		outColor = vec4(color, 1.0);
	}
	
	// Lambertian Shading
	vec3 lightDirection = -normalize(vec3(20.0f, 1.0f, 1.0f));
	vec3 color = vec3(0.2f);
	vec3 lightColor = vec3(0.99, 0.721, 0.0745);
	float lightIntensity = 1.5;
	float ambient = 0.1;
	float dotProd = clamp(dot(normalize(fs_normal), lightDirection), 0.0, 1.0);
	color = color * lightIntensity * dotProd + ambient;

	// FOG
	vec3 fragment_pos = fs_pos;
    vec3 cam_to_point = fragment_pos - camera.cameraPos;
    float dist = length(cam_to_point);
    float fogcoord = dist;
    float fog_density = 0.06;
    float fogEnd = 25.0;
    float fogStart = 1.0;
    float fogfactor = 0.0;

	fogfactor = 1.0-clamp(exp(-pow(fog_density*fogcoord, 2.0)), 0.0, 1.0);
	color = vec3(mix(vec4(color, 1.0), vec4(0.8,0.8,0.9,1.0), fogfactor));

	// gamma correction
	color = pow(color, vec3(1.0/2.2));

	outColor = vec4(color, 1.0);
}
