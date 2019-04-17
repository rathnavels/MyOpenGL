#version 420

in vec2 tes_tC;

out vec4 finalColor;
in float hValue;

uniform sampler2D	uTexture0;

void main()
{
	finalColor = texture(uTexture0, tes_tC);

//	if(hValue >= 192)
//		finalColor = finalColor* (hValue/255.0f) * (7 - 767.0f/hValue);
//	else if(hValue < 192 && hValue >= 128)
//		finalColor = finalColor *(hValue/255.0f) * (7 - 767.0f/hValue);
//	else if(hValue < 128 && hValue >= 64)
//		finalColor = finalColor *(hValue/255.0f) * 1.5;
//	else
//		finalColor = finalColor * (hValue/255.0f)* 1;

//	float factor = 7;
//
//	if(hValue >= 128)
//		finalColor = finalColor* (hValue/255.0f) * (factor - 767.0f/hValue) * 4;
//	else
//		finalColor = finalColor * (hValue/255.0f) * 4;
}