#version 430 core

layout(location=0) in mat4 modelMatrix;
layout(location=4) in vec4 VertexPosition;
layout(location=5) in vec4 VertexNormal;
layout(location=6) in vec3 VertexColor;
out vec4 vNormal;
out vec3 vColor;
out vec4 shaderCoord;

layout(location=7) uniform mat4 VP;
uniform mat4 DepthVP;

mat4 biasMatrix = mat4(0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.5,0.5,0.5,1.0);

void main()
{
	gl_Position =  VP* modelMatrix * VertexPosition;
	shaderCoord = biasMatrix * (DepthVP* modelMatrix * VertexPosition);
	vColor = VertexColor;
	vNormal = VertexNormal;
}