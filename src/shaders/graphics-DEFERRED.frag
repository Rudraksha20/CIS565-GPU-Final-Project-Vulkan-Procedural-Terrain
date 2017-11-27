#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 1) uniform sampler2D texSampler;

layout (set = 1, binding = 2) uniform sampler2D samplerAlbedo;
layout (set = 1, binding = 3) uniform sampler2D samplerPosition;
layout (set = 1, binding = 4) uniform sampler2D samplerNormal;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

vec3 getColorAtUV(vec2 uv) {
	// Lambertian Shading
	const vec3 lightDirection = -normalize(vec3(2.0f, 1.0f, 2.0f));
	vec4 albedo = texture(samplerAlbedo, uv);
	const float ambient = 0.2;
	vec3 normal = texture(samplerNormal, uv).xyz;
	float dotProd = (dot(normalize(normal), lightDirection));
	return vec3(albedo) * dotProd + vec3(ambient);
}

vec3 FXAA(vec2 uv, vec3 color, float width, float height, float FXAA_SPAN_MAX, float FXAA_EDGE_THRESHOLD_MAX, float FXAA_EDGE_THRESHOLD_MIN) {
	float quality[12] = {1.5, 2.0, 2.0, 2.0, 4.0, 8.0, 4.0, 8.0, 8.0, 8.0, 16.0, 32.0};

	int x0 = uv.x;
	int y0 = uv.y;
	int x1 = glm::clamp(x0 + 1, 0, width - 1);
	int y1 = glm::clamp(y0 + 1, 0, height - 1);
	int xm1 = glm::clamp(x0 - 1, 0, width - 1);
	int ym1 = glm::clamp(y0 - 1, 0, height - 1);

	// uv Index of the four pixels on the sides of a given pixel
	vec2 uvIndexUp = vec2(x0, y1);
	vec2 uvIndexDown = vec2(x0,  ym1);
	vec2 uvIndexLeft = vec2(xm1, y0);
	vec2 uvIndexRight = vec2(x1, y0);

	// uv index of the four pixels in the corner around the given pixel
	vec2 uvIndexUpLeft = vec2(xm1, y1);
	vec2 uvIndexDownLeft = vec2(xm1, ym1);
	vec2 uvIndexUpRight = vec2(x1, y1);
	vec2 uvIndexDownRight = vec2(x1, ym1);

	// Standard luminosity values of RGB based on the percieption of individual colors by humans
	glm::vec3 luma(0.299, 0.587, 0.114);

	// Luminosity at the given pixel index
	float lumaCenter = glm::dot(color, luma);

	// Find the luminosity of the texture in the surrounding four pixels
	float lumaUp = glm::dot(getColorAtUV(uvIndexUp), luma);
	float lumaDown = glm::dot(getColorAtUV(uvIndexDown), luma);
	float lumaRight = glm::dot(getColorAtUV(uvIndexRight), luma);
	float lumaLeft = glm::dot(getColorAtUV(uvIndexLeft), luma);
	
	// Find the luminosity of the four corners around a given pixel
	// These four values combined with the above values will be used to determine if an edge is horizontal or vertical
	float lumaUpLeft = glm::dot(getColorAtUV(uvIndexUpLeft), luma);
	float lumaUpRight = glm::dot(getColorAtUV(uvIndexUpRight), luma);
	float lumaDownLeft = glm::dot(getColorAtUV(uvIndexDownLeft), luma);
	float lumaDownRight = glm::dot(getColorAtUV(uvIndexDownRight), luma);

	// Check if we are in a region which needs to be AA'ed
	
	// find the min and max luminosity around a given fragmnet
	float lumaMin = glm::min(lumaCenter, glm::min(glm::min(lumaUp, lumaDown), glm::min(lumaRight, lumaLeft)));
	float lumaMax = glm::max(lumaCenter, glm::max(glm::max(lumaUp, lumaDown), glm::max(lumaRight, lumaLeft)));

	// Find the deviation (DELTA) of the luminosity for deciding if there is a significant edge to perform AA around the given pixel index
	float delta = lumaMax - lumaMin;

	// 2. Find the min and the max luma deviance and if it is below the threshold return. No AA will be performed as it is not an edge.

	// If the deviation is not significant enough don't bother doing AA
	if (delta < glm::max(FXAA_EDGE_THRESHOLD_MIN, lumaMax * FXAA_EDGE_THRESHOLD_MAX)) {
		return color;
	}

	// 3. Find the luma of all the corner points and find the difference in luma horizontally and vertically.

	// Combine the lumas
	// Edge
	float lumaDownUp = lumaDown + lumaUp;
	float lumaLeftRight = lumaLeft + lumaRight;
	// Corners
	float lumaLeftCorners = lumaDownLeft + lumaUpLeft;
	float lumaDownCorners = lumaDownLeft + lumaDownRight;
	float lumaRightCorners = lumaDownRight + lumaUpRight;
	float lumaUpCorners = lumaUpRight + lumaUpLeft;

	// Compute an estimation of the gradient along the horizontal and vertical axis.
	float edgeHorizontal = glm::abs(-2.0 * lumaLeft + lumaLeftCorners) + glm::abs(-2.0 * lumaCenter + lumaDownUp) * 2.0 + glm::abs(-2.0 * lumaRight + lumaRightCorners);
	float edgeVertical = glm::abs(-2.0 * lumaUp + lumaUpCorners) + glm::abs(-2.0 * lumaCenter + lumaLeftRight) * 2.0 + glm::abs(-2.0 * lumaDown + lumaDownCorners);

	// Is edge horizontal or vertical
	bool isHorizontal = (edgeHorizontal >= edgeVertical);

	// 4. Check if the luma deviation is more vertically or horizontally to determine the edge direction.

	// Select the two neighboring texels lumas in the opposite direction to the local edge.
	float luma1 = isHorizontal ? lumaDown : lumaLeft;
	float luma2 = isHorizontal ? lumaUp : lumaRight;
	// Compute gradients in this direction.
	float gradient1 = luma1 - lumaCenter;
	float gradient2 = luma2 - lumaCenter;

	// Which direction is the steepest ?
	bool is1Steepest = glm::abs(gradient1) >= glm::abs(gradient2);

	// Gradient in the corresponding direction, normalized.
	float gradientScaled = 0.25*glm::max(abs(gradient1), abs(gradient2));

	// Choose the step size (one pixel) according to the edge direction.
	float stepLength = isHorizontal ? (1.0f/height) : (1.0f/width);

	// Average luma in the correct direction.
	float lumaLocalAverage = 0.0;

	if (is1Steepest) {
		// Switch the direction
		stepLength = -stepLength;
		lumaLocalAverage = 0.5*(luma1 + lumaCenter);
	}
	else {
		lumaLocalAverage = 0.5*(luma2 + lumaCenter);
	}

	// 5. Once the edge is determined we ofset the uv coordinates to be as close as to the pixel edge.

	// Shift UV in the correct direction by half a pixel.
	glm::vec2 currentUV = glm::vec2(x0, y0);
	if (isHorizontal) {
		currentUV.y += stepLength * 0.5;
	}
	else {
		currentUV.x += stepLength * 0.5;
	}

	// 6. Now iterate on both sides of the current pixel along the edge till we treach the end i.e. a significant gradient drop. This means we have reached the end of the edge.

	// Exploer the edge on both sides and find the endpoint
	// Do the first iteration and you are done if you find the luminosity gradient is significant
	// Compute offset (for each iteration step) in the correct direction.
	glm::vec2 offset = isHorizontal ? glm::vec2((1.0/width), 0.0) : glm::vec2(0.0, (1.0f/height));
	// Compute UVs to explore on each side of the edge, orthogonally. 
	// The QUALITY allows us to step faster.
	glm::vec2 uv1 = currentUV - offset;
	glm::vec2 uv2 = currentUV + offset;

	// Read the lumas at both current extremities of the exploration segment, and compute the delta wrt to the local average luma.
	float lumaEnd1 = glm::dot(getColorAtUV(uv1), luma);
	float lumaEnd2 = glm::dot(getColorAtUV(uv2), luma);
	lumaEnd1 -= lumaLocalAverage;
	lumaEnd2 -= lumaLocalAverage;

	// If the luma deltas at the current extremities are larger than the local gradient, we have reached the side of the edge.
	bool reached2 = glm::abs(lumaEnd2) >= gradientScaled;
	bool reached1 = glm::abs(lumaEnd1) >= gradientScaled;
	bool reachedBoth = reached1 && reached2;

	// If the side is not reached, we continue to explore in this direction.
	if (!reached1) {
		uv1 -= offset;
	}
	if (!reached2) {
		uv2 += offset;
	}

	// Itereating
	if(!reachedBoth) {
		for (int i = 1; i < FXAA_SPAN_MAX; i++) {
		
			// If needed, read luma in 1st direction, compute delta.
			if (!reached1) {
				lumaEnd1 = glm::dot(getColorAtUV(uv1), luma);
				lumaEnd1 = lumaEnd1 - lumaLocalAverage;
			}
			// If needed, read luma in opposite direction, compute delta.
			if (!reached2) {
				lumaEnd2 = glm::dot(getColorAtUV(uv2), luma);
				lumaEnd2 = lumaEnd2 - lumaLocalAverage;
			}
			// If the luma deltas at the current extremities is larger than the local gradient, we have reached the side of the edge.
			reached1 = abs(lumaEnd1) >= gradientScaled;
			reached2 = abs(lumaEnd2) >= gradientScaled;
			reachedBoth = reached1 && reached2;

			// If the side is not reached, we continue to explore in this direction, with a variable quality.
			if (!reached1) {
				uv1 -= offset * quality[i];
			}
			if (!reached2) {
				uv2 += offset * quality[i];
			}

			if (reachedBoth) {
				break;
			}
		}
	}
	// Done iterating

	// 7. Now we average the pixel uv coordinates based on how close it is to the either edge

	// Now we estimate the offset if we are at the center of the edge or near the far sides.
	// The closer we are to the far sides the more blurring will need to be done to make the edge look smooth

	// Compute the distances to each extremity of the edge.
	float distance1 = isHorizontal ? (x0 - uv1.x) : (y0 - uv1.y);
	float distance2 = isHorizontal ? (uv2.x - x0) : (uv2.y - y0);

	// In which direction is the extremity of the edge closer ?
	bool isDirection1 = distance1 < distance2;
	float distanceFinal = glm::min(distance1, distance2);

	// Length of the edge.
	float edgeThickness = (distance1 + distance2);

	// UV offset: read in the direction of the closest side of the edge.
	float pixelOffset = -distanceFinal / edgeThickness + 0.5;

	// Now check if the luminosity of the center pixe; corrosponds to that on the edges detected
	// If not than we may have stepped too far

	// Is the luma at center smaller than the local average ?
	bool isLumaCenterSmaller = lumaCenter < lumaLocalAverage;

	// If the luma at center is smaller than at its neighbour, the delta luma at each end should be positive (same variation).
	// (in the direction of the closer side of the edge.)
	bool correctVariation = ((isDirection1 ? lumaEnd1 : lumaEnd2) < 0.0) != isLumaCenterSmaller;

	// If the luma variation is incorrect, do not offset.
	float finalOffset = correctVariation ? pixelOffset : 0.0;

	// 8. Color the pixel

	// Compute the final UV coordinates.
	glm::vec2 finalUv = glm::vec2(x0, y0);
	if (isHorizontal) {
		finalUv.y += finalOffset * stepLength;
	}
	else {
		finalUv.x += finalOffset * stepLength;
	}

	// Read the color at the new UV coordinates, and use it.
	color = getColorAtUV(finalUv);
	return color;
}

void main() {
    //outColor = vec4(fragTexCoord.x, fragTexCoord.y, 0.0, 1.0);//texture(texSampler, fragTexCoord);
	vec3 color = getColorAtUV(fragTexCoord);
	int width = 640;
	int height = 480;
	float FXAA_SPAN_MAX = 12.0;
	float FXAA_EDGE_THRESHOLD_MAX = 1.0/8.0; 
	float FXAA_EDGE_THRESHOLD_MIN = 0.0312;

	// FXAA
	color = FXAA(uv, color, width, height, FXAA_SPAN_MAX, FXAA_EDGE_THRESHOLD_MAX, FXAA_EDGE_THRESHOLD_MIN);

	outColor = vec4(color.xyz, 1.0);
}
