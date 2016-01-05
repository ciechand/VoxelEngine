#version 430 core

in mat4 modelMatrix;
in vec4 vPosition;
in vec4 vNormal;
in vec2 textureCoordinatesIn;
in int texPosIn;
out vec4 Normal;
out vec4 colorOut;
out vec2 textureCoordinatesOut;
flat out int texPosOut;

uniform mat4 viewMatrix;
uniform mat4 proj;

void main()
{
	gl_Position = proj  * viewMatrix * modelMatrix * vPosition;
	colorOut = vec4(0.0f,0.0f,0.0f,0.0f);
	Normal = vNormal;
	textureCoordinatesOut = textureCoordinatesIn;
	texPosOut = texPosIn;
}