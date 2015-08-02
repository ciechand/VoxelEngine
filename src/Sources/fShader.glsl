#version 430 core

in vec4 color;
in vec2 texCoor;
out vec3 fColor;

uniform sampler2D textureSample;

void main()
{
	fColor = texture(textureSample, texCoor).rgb;
}