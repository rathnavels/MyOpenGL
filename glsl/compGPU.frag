#version 410

uniform mat3 normalMatrix;
uniform sampler2D texUnit;
uniform sampler2D heightMap;
uniform float heightStep, gridSpacing;
uniform int scaleFactor;

in vec2 uvTE;
out vec4 outputF;

float height(float u, float v)
{
	return (texture(heightMap, vec2(u,v)).r * heightStep);
}

void main()
{
//	float delta = 1.0 / textureSize(heightMap,0).x * scaleFactor;
//
//	vec3 deltaX = vec3(2.0 * gridSpacing, height(uvTE.s + delta, uvTE.t) -  height(uvTE.s - delta, uvTE.t), 0.0);
//
//	vec3 deltaZ = vec3(0.0,height(uvTE.s, uvTE.t + delta) - height(uvTE.s, uvTE.t - delta), 2.0 * gridSpacing);
//
//	vec3 normalF = normalize(normalMatrix * cross(deltaZ, deltaX));
//
//	float intensity = max(dot(vec3(0.577,0.577,0.577),normalF),0.0);

	vec4 color = texture2D(texUnit, uvTE) * vec4 (0.8,0.8,0.8,1.0);

	//outputF = color * intensity + color * vec4(0.8,0.8,0.8,1.0);

	outputF = color * vec4(1.2f,1.2f,1.2f,1.0f);
}