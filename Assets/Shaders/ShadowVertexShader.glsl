#version 430 core

layout(location=4) in vec4 VertexPosition;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

mat4 modelMatrix = mat4(1.0);

void main()
{
	gl_Position =  projectionMatrix * viewMatrix * modelMatrix * VertexPosition;
}