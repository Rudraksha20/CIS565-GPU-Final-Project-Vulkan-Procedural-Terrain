#version 450
#extension GL_ARB_separate_shader_objects : enable

//layout(set = 1, binding = 1) uniform sampler2D texSampler;
layout(set = 0, binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
	vec3 cameraPos;
} camera;

layout(set = 2, binding = 1) uniform sampler2D samplerSkybox;

layout(set = 1, binding = 0) uniform Time {
    float deltaTime;
    float totalTime;
} time;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
	// Skybox & Sun
	const vec3 sunPos = vec3(10.0 * cos(time.totalTime * 0.025), 2.0, 10.0 * sin(time.totalTime * 0.025));
	const vec3 sunDir = normalize(sunPos);//normalize(vec3(1.0, 0.333, -0.005));
	// sample from skybox texture
	// get sky's "position"
	// 100.0 = far plane
	vec4 skyPos = vec4(fragTexCoord.xy, 1.0, 1.0) * 100.0;
	vec4 worldSkyPos = inverse(camera.proj * camera.view) * skyPos;
	//worldSkyPos = normalize(worldSkyPos);
	vec3 lookDir = normalize(worldSkyPos.xyz - camera.cameraPos);
	float xzAngle = atan(lookDir.z, lookDir.x);//atan(worldSkyPos.z, worldSkyPos.x);
	float yAngle = asin(worldSkyPos.y);
	//lookDir.y = (lookDir.y < 0.0) ? (lookDir.y * 0.5 - 0.5) : (lookDir.y * 1.5 - 0.5);
	//lookDir.y = clamp(lookDir.y + 0.2, -1.0, 1.0);
	//lookDir.y = (lookDir.y > 0.6) ? lookDir.y + 0.2 + (lookDir.y - 0.6) * 0.45 : lookDir.y + 0.2;
	float radius = 1.0 - (lookDir.y + 1.0) * 0.5;
	vec2 skyboxUV = vec2(cos(xzAngle) * radius, sin(xzAngle) * radius) * 0.5 + vec2(0.5);
	vec4 skyColor = texture(samplerSkybox, skyboxUV);//vec4(y_angle, 0.0, xz_angle, 1.0);
	outColor = mix(vec4(0.768f, 0.8039f, 0.898f, 1.0), skyColor, 0.5);
	const float angle = acos(dot(lookDir, sunDir));
	const float maxSunMixFactor = 0.95;
	float sunMixFactor = angle < 0.010 ? maxSunMixFactor :
							angle < 0.040 ? maxSunMixFactor * (1.0 - (angle - 0.010) / 0.030) :
							                0.0;
	// if color is blue-ish, decrease sun influence to simulate cloud cover
	sunMixFactor *= (2.0 * skyColor.b > skyColor.r + skyColor.g) ? 1.0 : 0.35;											  
	outColor = mix(outColor, vec4(1.0, 0.9, 0.8, 1.0), sunMixFactor);
}
