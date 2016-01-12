#version 430 core

in mat4 modelMatrix;
in vec4 vPosition;
in vec4 vNormal;
in vec2 textureCoordinatesIn;
in int texPosIn;
in vec3 iColor;
out vec4 Normal;
out vec3 colorOut;
out vec2 textureCoordinatesOut;
flat out int texPosOut;

uniform mat4 viewMatrix;
uniform mat4 proj;

void main()
{
	gl_Position = proj  * viewMatrix * modelMatrix * vPosition;
	colorOut = iColor;
	Normal = vNormal;
	textureCoordinatesOut = textureCoordinatesIn;
	texPosOut = texPosIn;
}