#version 430 core

out float fragmentdepth;

void main(){
	//opengl does this already.
	fragmentdepth = gl_FragCoord.z;
}