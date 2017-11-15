#version 430 core

in vec4 vNormal;
in vec3 vColor;
out vec4 fColor;
out vec4 norm;


void main()
{
	fColor = vec4(vColor, 1.0f);
	norm = vNormal;
}