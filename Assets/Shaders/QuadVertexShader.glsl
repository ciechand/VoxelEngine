#version 430 core

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec2 TextureCoords;

out vec2 texCoords;

void main (){
	texCoords = TextureCoords;
	gl_Position = vec4(VertexPosition, 1.0);
}