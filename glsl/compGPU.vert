#version 410

layout(location = 0) in vec2 vertex;


out vec2 posV;

void main()
{
	posV = vertex;
}
