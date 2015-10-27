#version 430 core

in vec4 colorOut;
in vec2 textureCoordinatesOut;
in vec4 Normal;
out vec4 fColor;
out vec4 norm;

uniform sampler2D textureSample;

void main()
{
	//fColor = texture(textureSample, textureCoordinatesOut).rgb;
	fColor = colorOut;
	norm = Normal;
}