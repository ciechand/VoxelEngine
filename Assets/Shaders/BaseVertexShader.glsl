#version 430 core

layout(location=0) in mat4 modelMatrix;
layout(location=4) in vec4 VertexPosition;
layout(location=5) in vec4 VertexNormal;
layout(location=6) in vec3 VertexColor;
out vec4 lightDirection;
out vec4 vNormal;
out vec3 vColor;
out vec4 shaderCoord;
 
layout(location=7) uniform mat4 V;
layout(location=11) uniform mat4 P;
uniform mat4 DepthVP;
uniform vec3 lightPos;

mat4 biasMatrix = mat4(0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.5,0.5,0.5,1.0);

void main()
{
	gl_Position =  P * V * modelMatrix * VertexPosition;
	shaderCoord = (DepthVP * modelMatrix * VertexPosition);
	shaderCoord = biasMatrix * (shaderCoord/shaderCoord.w);
	vColor = VertexColor;
	lightDirection = normalize(V*vec4(lightPos,0));
	vNormal = normalize(V*modelMatrix*VertexNormal);
}