#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 1) uniform sampler2D texSampler;

layout (set = 1, binding = 2) uniform sampler2D samplerAlbedo;
layout (set = 1, binding = 3) uniform sampler2D samplerPosition;
layout (set = 1, binding = 4) uniform sampler2D samplerNormal;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    //outColor = vec4(fragTexCoord.x, fragTexCoord.y, 0.0, 1.0);//texture(texSampler, fragTexCoord);
	// Lambertian Shading
	const vec3 lightDirection = -normalize(vec3(2.0f, 1.0f, 2.0f));
	vec4 albedo = texture(samplerAlbedo, fragTexCoord);
	const float ambient = 0.2;
	vec3 normal = texture(samplerNormal, fragTexCoord).xyz;
	float dotProd = (dot(normalize(normal), lightDirection));
	vec4 color = albedo * dotProd + vec4(ambient);
	outColor = vec4(color.xyz, 1.0);
}
