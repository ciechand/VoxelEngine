#version 430 core

in vec4 vNormal;
in vec3 vColor;
flat in float outBright;
out vec4 fColor;
out vec4 norm;


void main()
{
	//fColor = vec4(1.0f, vColor.g, 0.0f ,1.0f);
	fColor = vec4(vColor.r*(outBright/255.0f), vColor.g*(outBright/255.0f), vColor.b*(outBright/255.0f), 1.0f);
	norm = vNormal;
}