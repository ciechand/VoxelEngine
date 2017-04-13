#version 430 core

in vec4 vNormal;
in vec3 vColor;
out vec4 fColor;
out vec4 norm;


void main()
{
	//fColor = vec4(1.0f, vColor.g, 0.0f ,1.0f);
	fColor = vec4(vColor.r, vColor.g, vColor.b, 1.0f);
	norm = vNormal;
}