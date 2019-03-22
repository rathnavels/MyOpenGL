#version 420

in vec3 tColor;

out vec4 finalColor;

void main()
{
	finalColor = vec4(1.0, 0.5, 1.0, 1.0);
	//finalColor = vec4(tColor, 1.0);
}