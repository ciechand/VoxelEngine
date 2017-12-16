#version 430 core

layout(location=0) in mat4 modelMatrix;
layout(location=4) in vec4 VertexPosition;

layout(location=7)  uniform mat4 VP;



void main(){
	gl_Position =  VP * modelMatrix * VertexPosition;
}