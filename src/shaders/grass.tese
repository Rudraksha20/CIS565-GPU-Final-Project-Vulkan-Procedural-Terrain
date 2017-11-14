#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(quads, equal_spacing, ccw) in;

layout(set = 0, binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} camera;

// TODO: Declare tessellation evaluation shader inputs and outputs
layout(location = 0) out vec2 fs_uv;
layout(location = 1) out vec3 fs_normal;
layout(location = 2) out vec4 fs_color;

layout(location = 0) patch in vec4 tese_v1;
layout(location = 1) patch in vec4 tese_v2;
layout(location = 2) patch in vec4 tese_up;
layout(location = 3) patch in vec4 tese_bitangent;
layout(location = 4) patch in vec4 tese_color;

// https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
float rand(vec2 n) { 
	return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(vec2 p){
	vec2 ip = floor(p);
	vec2 u = fract(p);
	u = u*u*(3.0-2.0*u);
	
	float res = mix(
		mix(rand(ip),rand(ip+vec2(1.0,0.0)),u.x),
		mix(rand(ip+vec2(0.0,1.0)),rand(ip+vec2(1.0,1.0)),u.x),u.y);
	return res*res;
}

// http://flafla2.github.io/2014/08/09/perlinnoise.html
float smoothNoise(vec2 p){
	float total = 0.0;
	float freq = 1.0;
	float ampl = 1.0;
	float maxVal = 0.0;
	for (int i = 0; i < 6; i++) {
		total += noise(p * freq) * ampl;
		maxVal += ampl;
		ampl *= 0.5;
		freq *= 2.0;
	}
	return total / maxVal;
}

void main() {
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

	vec4 worldPos = gl_in[0].gl_Position;

	// hard-code in planeDim...
	const float planeDim = 15.0;

	worldPos += vec4(u * planeDim, 0.0, v * planeDim, 0.0);
	worldPos.y += smoothNoise(worldPos.xz) * 4.0;
	worldPos.w = 1.0;

	mat4 viewProj = camera.proj * camera.view;
	gl_Position = viewProj * worldPos;
	fs_color = vec4(noise(worldPos.yy + worldPos.x * worldPos.z), noise(worldPos.yy), noise(worldPos.yy * worldPos.x * worldPos.z), 1.0);
	fs_color.xyz *= 1.25;
	fs_uv.x = (0.49 <= u && u <= 0.5) ? 1.0 : 0.0;
	fs_uv.y = (0.24 <= v && v <= 0.26) ? 1.0 : 0.0;
	fs_normal = vec3(1.0);
}
