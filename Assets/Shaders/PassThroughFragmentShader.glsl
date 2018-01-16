#version 330 core

// Ouput data
layout(location = 0) out vec4 color;

uniform sampler2D textureSample;

in vec2 UV;

void main(){
	color = texture(textureSample, UV)/1000000 + vec4(1.0,0.0,0.0,0.0);
}