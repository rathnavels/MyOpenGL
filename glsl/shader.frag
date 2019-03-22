#version 420

in vec2 tC;

out vec4 finalColor;

uniform sampler2D	uTexture0;

void main()
{
	finalColor = texture(uTexture0,tC);
	finalColor = vec4(1,0,0,0);
}