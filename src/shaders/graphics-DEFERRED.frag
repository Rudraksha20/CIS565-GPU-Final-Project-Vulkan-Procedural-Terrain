#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
	vec3 cameraPos;
} camera;

layout(set = 2, binding = 0) uniform Time {
    float deltaTime;
    float totalTime;
} time;

layout(set = 1, binding = 1) uniform sampler2D texSampler;

layout (set = 1, binding = 2) uniform sampler2D samplerAlbedo;
layout (set = 1, binding = 3) uniform sampler2D samplerPosition;
layout (set = 1, binding = 4) uniform sampler2D samplerNormal;
layout (set = 1, binding = 5) uniform sampler2D samplerSkybox;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

#define FOG		1
#define SHADOWS 1
#define SKYBOX  1
#define TEXTURE 1

// Noise
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

// Ray-marching for shadows
float rayMarchShadows(vec3 ro, vec3 rd, float mint, float maxt, vec3 normal) {
	// offset ro.y by a small epsilon to handle shadow acne
	float epsilon = 0.025;
	ro = ro + normal * epsilon;
	float originalH = ro.y;
	for(float t = mint; t < maxt;) {
		// travel along rd by t
		vec3 newPos = ro + t * rd;
		float newH = 1.0 + smoothNoise(newPos.xz * 0.125) * 6.0;
		if(newH > newPos.y) {
			return 0.1;
		}
		if(t < 5.0 * mint) {
			t += mint;
		} else {
			t += 2.0;
		}	
	}
	return 1.0;
}

vec3 getColorAtUV(vec2 uv, vec4 position) {
	// Lambertian Shading
	
	// Fragment Position
	float noise = smoothNoise(position.xz);

	// Primary Sun light
	vec3 lightPosition = vec3(10.0 * cos(time.totalTime * 0.025), 2.0, 10.0 * sin(time.totalTime * 0.025));
	const vec3 lightDirection = normalize(lightPosition);//normalize(position.xyz - lightPosition);
	float lightIntensity = 1.5;

#if TEXTURE
	vec4 albedo = texture(samplerAlbedo, uv);
#else
	vec4 albedo = vec4(0.88, 0.88, 0.88, 1.0);
#endif	

	const float ambient = 0.15;
	vec3 normal = normalize(texture(samplerNormal, uv).xyz);

	float mint = 0.1;
	float maxt = 30.0;
	// DEBUG VIEW
#if SHADOWS
	float occ = rayMarchShadows(position.xyz, lightDirection, mint, maxt, normal);
#else
	float occ = 1.0;
#endif
	
	// Primary light
	float dotProd = clamp(dot(normal, lightDirection), 0.0, 1.0);

	// Sky light
	float sky = clamp(0.5 + normal.y * 0.5, 0.0 , 1.0);

	// Indiect light
	float ind = clamp(dot(normal, normalize(lightDirection * vec3(-1.0, 0.0, -1.0))), 0.0, 1.0);

	//return vec3(albedo) * dotProd * lightIntensity + vec3(ambient);
	vec3 lightContribution = dotProd * pow(vec3(dotProd), vec3(1.0, 1.2, 1.5));
	lightContribution += sky; //vec3(0.768f, 0.8039f, 0.898f);//vec3(0.16, 0.2, 0.28);
	lightContribution += ind; //vec3(0.1, 0.1, 0.1);
	return vec3(albedo) * occ * lightContribution + vec3(ambient);
}

void main() {
    //outColor = vec4(fragTexCoord.x, fragTexCoord.y, 0.0, 1.0);//texture(texSampler, fragTexCoord);
    vec4 fragment_pos = texture(samplerPosition, fragTexCoord);
    // put point back into correct position
    const float tileDim = 15.0;

    // This is essentially re-does the step in the compute shader where we move tiles
    // to be closer to the camera (this step was undone in the previous frag shader to preserve precision)
    fragment_pos.x += floor(camera.cameraPos.x / tileDim) * tileDim;
    fragment_pos.z += floor(camera.cameraPos.z / tileDim) * tileDim;
	
    vec3 color = getColorAtUV(fragTexCoord, fragment_pos);

#if FOG
	// FOG
	vec3 sunPosition = vec3(50.0f, 1.0f, 50.0f);
	vec3 sunDirection = normalize(fragment_pos.xyz - sunPosition);
	//vec3 sunDirection = -normalize(vec3(50.0f, 1.0f, 50.0f));

    vec4 cam_to_point = fragment_pos - vec4(camera.cameraPos, 1.0);
    float dist = length(vec3(cam_to_point));
	cam_to_point = normalize(cam_to_point);
    float fogcoord = dist;
    float fog_density = 0.08;
    float fogEnd = 50.0;
    float fogStart = 0.0;
    float fogfactor = 0.0;
	//float sunContribution = max(dot(vec3(cam_to_point), sunDirection), 0.1);
	//vec3 fogColor = mix(vec3(0.3,0.4,0.4), vec3(0.8, 0.7, 0.5), sunContribution);
	vec3 fogColor = vec3(0.8,0.8,0.9);
	float c = 0.3;
	// Height based fog
	if(abs(cam_to_point.y) < 0.001) {
		cam_to_point.y = 0.001;
	}
	fogfactor = c * exp(-camera.cameraPos.y * fog_density) * (1.0 - exp(-fogcoord * cam_to_point.y * fog_density)) / cam_to_point.y;
	//fogfactor = 1.0-clamp(exp(-pow(fog_density*fogcoord, 2.0)), 0.0, 1.0);

	// Adding fog to the final color
	// DEBUG VIEW
	color = mix(color, fogColor, fogfactor);
#endif 
	
	// sun
	// sun's "position"
	//const vec3 sunPosition = vec3(cos(time.totalTime / 5.0), 0.4, sin(time.totalTime / 5.0));
	const vec3 sunPos = vec3(10.0 * cos(time.totalTime * 0.025), 2.0, 10.0 * sin(time.totalTime * 0.025));
	const vec3 sunDir = normalize(sunPos);//normalize(vec3(1.0, 0.333, -0.005));

	if(fragment_pos.y <= 0 ) {
#if SKYBOX
		// sample from skybox texture
		// get sky's "position"
		// 100.0 = far plane
		vec4 skyPos = vec4(fragTexCoord.xy, 1.0, 1.0) * 100.0;
		vec4 worldSkyPos = inverse(camera.proj * camera.view) * skyPos;
		//worldSkyPos = normalize(worldSkyPos);
		vec3 lookDir = normalize(worldSkyPos.xyz - vec3(camera.cameraPos.x, 0.0, camera.cameraPos.z));
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
#else
		outColor = vec4(0.768f, 0.8039f, 0.898f, 1.0);
#endif
	}
	else {
		// gamma correction
		//color = pow( color, vec3(1.0/2.2) );
		// sample some points around the sun to infer how occluded it is
		float xzAngle = atan(sunDir.z, sunDir.x);
		float radius = 1.0 - (sunDir.y + 1.0) * 0.5;
		float occlusions = 0.0;
#if 0
		for (float i = -1.0; i < 1.1; i += 1.0) {
			for (float j = -1.0 ; j < 1.1; j += 1.0) {
				float angle = xzAngle + i * 0.002;
				float r = radius + j * 0.002;
				vec2 sampleUV = vec2(cos(angle) * r, sin(angle) * r) * 0.5 + vec2(0.5);
				vec4 sampleColor = texture(samplerSkybox, sampleUV);
				if (2.0 * sampleColor.b <= sampleColor.r + sampleColor.g) {
					occlusions += 1.0;
				}
			}
		}
		// debug view
		float cloudFactor = (1.0  - (occlusions / 9.0) * 0.2);
#endif
		outColor = vec4(color.xyz, 1.0);
	}
}
