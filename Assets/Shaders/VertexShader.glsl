#version 430 core

layout(location=0) in mat4 modelMatrix;
layout(location=4) in vec4 VertexPosition;
layout(location=5) in vec4 VertexNormal;
layout(location=6) in vec3 VertexColor;
out vec4 vNormal;
out vec3 vColor;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	gl_Position =  projectionMatrix * viewMatrix * modelMatrix * VertexPosition;
	//gl_Position =  VertexPosition;
	vColor = VertexColor;
	vNormal = VertexNormal;
}