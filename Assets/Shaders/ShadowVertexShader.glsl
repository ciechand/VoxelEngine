#version 430 core

layout(location=0) in mat4 modelMatrix;
layout(location=4) in vec4 VertexPosition;

void main(){
	gl_Position = modelMatrix * VertexPosition;
}