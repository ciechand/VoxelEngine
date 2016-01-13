#version 430 core

in vec3 colorOut;
in vec2 textureCoordinatesOut;
in vec4 Normal;
flat in int texPosOut;
out vec4 fColor;
out vec4 norm;

uniform int TexSize;
uniform sampler2D textureSample;

void main()
{
	float positionx = (mod(texPosOut, TexSize)/(TexSize*16));
	float positiony = ((texPosOut/TexSize))/(TexSize*16);
	vec2 offset = vec2(textureCoordinatesOut.x/TexSize,textureCoordinatesOut.y/TexSize);
	vec2 trueTexCoords = vec2(positionx+offset.x, positiony+offset.y);

	vec4 texColor = texture(textureSample, trueTexCoords);

	if(texColor.a == 0.0) discard;

	vec4 colorPer = vec4(texColor.r/255.0f,texColor.g/255.0f,texColor.b/255.0f,texColor.a);
	vec4 transformColor = vec4(colorOut.r,colorOut.g,colorOut.b,1.0f);
	
	vec4 finalColor = transformColor*colorPer;

	fColor = finalColor;
	norm = Normal;
}