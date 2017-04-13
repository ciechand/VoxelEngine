#version 430 core

in vec4 VertexPosition;
in vec4 VertexNormal;
in vec3 VertexColor;
out vec4 vNormal;
out vec3 vColor;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	gl_Position =  projectionMatrix * viewMatrix * VertexPosition;
	//gl_Position =  VertexPosition;
	vColor = VertexColor;
	vNormal = VertexNormal;
}