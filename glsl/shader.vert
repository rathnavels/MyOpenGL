#version 440

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 color;

out vec3 fColor;

uniform mat4 mMVP;

void main()
{
	fColor = color;
	gl_Position = mMVP * vec4(vertex,1);
}