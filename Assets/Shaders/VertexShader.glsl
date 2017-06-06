#version 430 core

layout (layout=0) out gPosition;
layout (layout=1) out gNormal;
layout (layout=2) out gAlbedoSpec;

layout(location=3) in mat4 modelMatrix;
layout(location=7) in vec4 VertexPosition;
layout(location=8) in vec4 VertexNormal;
layout(location=9) in vec3 VertexColor;
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