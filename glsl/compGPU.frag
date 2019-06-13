#version 410

uniform mat3				normalMatrix;
uniform sampler2D			texUnit;
uniform sampler2D			heightMap;
uniform float				heightStep;
uniform float				gridSpacing;
uniform int					scaleFactor;

uniform mat3            mTex;

in vec2						uvTE;
out vec4					outputF;

//float height(float u, float v)
//{
//	return (texture(heightMap, vec2(u,v)).r * heightStep);
//}

void main()
{
	//outputF = texture2D(texUnit, (mTex * glm::vec3(uvTE,1.0)).st) * vec4 (0.8,0.8,0.8,1.0);
	outputF = texture2D(texUnit, uvTE) * vec4 (0.8,0.8,0.8,1.0);
}