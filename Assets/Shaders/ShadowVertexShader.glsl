#version 430 core

layout(location=0) in mat4 modelMatrix;
layout(location=4) in vec4 VertexPosition;

layout(location=7)  uniform mat4 PV;



void main(){
	gl_Position =  PV * modelMatrix * VertexPosition;
}