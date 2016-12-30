#version 430 core

in vec4 vertPosition;
in vec4 vertNormal;
in vec3 vertColor;
out vec4 vNormal;
out vec3 vColor;

void main()
{
	gl_Position = vertPosition;
	vColor = vertColor;
	vNormal = vertNormal;
}