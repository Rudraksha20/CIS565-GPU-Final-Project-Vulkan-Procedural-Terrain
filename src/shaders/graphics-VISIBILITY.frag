#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 1) uniform sampler2D texSampler;

layout (set = 1, binding = 2) uniform sampler2D samplerVisibility;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

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
	vec4 viz = texture(samplerVisibility, fragTexCoord);
	vec4 worldPos = vec4(viz.x, 1.0, viz.z, 1.0);
	vec2 uv = viz.yw; // not used yet

	// Re-compute noise
	worldPos.y += smoothNoise(worldPos.xz * 0.125) * 6.0;

	// Re-compute normal
	float deviation = 0.0001;
	vec3 posXOffset = worldPos.xyz;
	posXOffset.x += 1.0 * deviation;
	posXOffset.y = 1.0;
	posXOffset.y += smoothNoise(posXOffset.xz * 0.125) * 6.0;
	vec3 posZOffset = worldPos.xyz;
	posZOffset.z += 1.0 * deviation;
	posZOffset.y = 1.0;
	posZOffset.y += smoothNoise(posZOffset.xz * 0.125) * 6.0;

	vec3 normal = normalize(cross(posXOffset - worldPos.xyz, posZOffset - worldPos.xyz));

	// Lambertian Shading
	const vec3 lightDirection = -normalize(vec3(2.0f, 1.0f, 2.0f));
	vec4 albedo = vec4(0.75, 0.75, 0.75, 1.0);//texture(samplerAlbedo, fragTexCoord);
	const float ambient = 0.2;
	float dotProd = (dot(normalize(normal), lightDirection));
	vec4 color = albedo * dotProd + vec4(ambient);
	outColor = vec4(color.xyz, 1.0);
}
