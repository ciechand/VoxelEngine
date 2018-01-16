#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec2 vertexPosition;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

void main(){
	gl_Position =  vec4(vertexPosition,0,1);
	UV = (vertexPosition+vec2(1,1))/2.0;
}

