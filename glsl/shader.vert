#version 440

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 texCoord;

out vec2 tC;

void main()
{
	tC = texCoord;
	gl_Position = vec4(vertex,1);
}