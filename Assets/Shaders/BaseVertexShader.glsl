#version 430 core

layout(location=0) in mat4 modelMatrix;
layout(location=4) in vec4 VertexPosition;
layout(location=5) in vec4 VertexNormal;
layout(location=6) in vec3 VertexColor;
layout(location=7) in vec2 TextureCoords;
out vec4 vNormal;
out vec3 vColor;
out vec4 shaderCoord;
out vec2 texCoords;
 
layout(location=8) uniform mat4 V;
layout(location=12) uniform mat4 P;



void main()
{

	shaderCoord = (V*modelMatrix*VertexPosition);

	 mat4 normalMatrix = transpose(inverse(V*modelMatrix));
	vNormal = (normalMatrix*VertexNormal);

	texCoords = TextureCoords;
	vColor = VertexColor;
	gl_Position =  P*shaderCoord;
}