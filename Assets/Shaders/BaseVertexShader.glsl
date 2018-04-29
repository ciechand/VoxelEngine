#version 430 core

layout(location=0) in mat4 modelMatrix;
layout(location=4) in vec4 VertexPosition;
layout(location=5) in vec4 VertexNormal;
layout(location=6) in vec3 VertexColor;
out vec4 vNormal;
out vec3 vColor;
out vec4 shaderCoord;
 
layout(location=7) uniform mat4 V;
layout(location=11) uniform mat4 P;



void main()
{
	gl_Position =  P * V * modelMatrix * VertexPosition;
	shaderCoord = (modelMatrix * VertexPosition);
	vColor = VertexColor;
	//lightDirection = normalize(V*vec4(lightPos,0));
	vNormal = normalize(modelMatrix*VertexNormal);
}