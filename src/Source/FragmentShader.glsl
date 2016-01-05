#version 430 core

in vec4 colorOut;
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
	fColor = vec4(texture(textureSample, trueTexCoords).rgba);
	norm = Normal;
}