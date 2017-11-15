#version 430 core

layout(location = 0) out float fragmentdepth;

void main()
{
	//opengl does this already.
	fragmentdepth = gl_FragCoord.z;
}