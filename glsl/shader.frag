#version 420

in vec3 fColor;

out vec4 finalColor;

void main()
{
	finalColor = vec4(fColor, 1.0);
}