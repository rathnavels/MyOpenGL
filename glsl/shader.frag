#version 420

in vec2 tes_tC;

out vec4 finalColor;

uniform sampler2D	uTexture0;

void main()
{
	finalColor = texture(uTexture0, tes_tC);
}