#version 430 core

layout (location = 0) in mat4 modelMatrix;
layout (location = 4) in vec4 vPosition;
layout (location = 5) in vec4 vNormal;
layout (location = 6) in vec2 textureCoordinatesIn;
out vec4 Normal;
out vec4 colorOut;
out vec2 textureCoordinatesOut;

uniform mat4 viewMatrix;
uniform mat4 proj;

void main()
{
	gl_Position = proj * modelMatrix * viewMatrix * vPosition;
	colorOut = vec4(1.0f,0.0f,0.0f,1.0f);
	Normal = vNormal;
	textureCoordinatesOut = textureCoordinatesIn;
}