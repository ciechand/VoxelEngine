#version 430 core

layout(location=0) in mat4 modelMatrix;
layout(location=4) in vec4 VertexPosition;
layout(location=5) in vec4 VertexNormal;
layout(location=7) in vec2 TextureCoords;

out vec4 vNormal;
out vec4 shaderCoord;
out vec2 texCoords;

layout(location=12) uniform mat4 P;
layout(location=8) uniform mat4 V;

void main()
{
	gl_Position =  P * V * modelMatrix * VertexPosition;
	shaderCoord = (modelMatrix * VertexPosition);
	//lightDirection = normalize(V*vec4(lightPos,0));
	vNormal = (modelMatrix*VertexNormal);
	texCoords = TextureCoords;
}