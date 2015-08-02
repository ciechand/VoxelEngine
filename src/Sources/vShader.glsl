#version 430 core

in vec4 vPosition;
in vec4 vNormal;
in vec2 tex;
out vec4 color;
out vec2 texCoor;

uniform mat4 mv;
uniform mat4 proj;

void main()
{
	gl_Position = proj * mv * vPosition;
	color = vec4(1.0f,0.0f,0.0f,1.0f);
	texCoor = tex;
}