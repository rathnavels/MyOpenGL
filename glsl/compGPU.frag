#version 410

uniform mat3				normalMatrix;
uniform sampler2D			texUnit;
uniform sampler2D			heightMap;
uniform float				heightStep;
uniform float				gridSpacing;
uniform int					scaleFactor;

in vec2						uvTE;
out vec4					outputF;

//float height(float u, float v)
//{
//	return (texture(heightMap, vec2(u,v)).r * heightStep);
//}

void main()
{
	outputF = texture2D(texUnit, uvTE) * vec4 (0.8,0.8,0.8,1.0);
}