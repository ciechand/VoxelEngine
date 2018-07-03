#version 430 core

in vec4 vNormal;
in vec3 vColor;
in vec4 shaderCoord;
in vec2 texCoords;
out vec4 NormalBuffer;
out vec4 ColorBuffer;
out vec4 PositionBuffer;
out vec4 TextureCoordBuffer;
out float fragDepth;

//Start of the main function.
void main()
{	
	NormalBuffer = vec4(vNormal.xyz,1.0);
	ColorBuffer = vec4(vColor, 1.0);
	PositionBuffer = vec4(shaderCoord.xyz, 1.0);
	TextureCoordBuffer = vec4(texCoords, 0.0, 1.0);
	fragDepth = gl_FragCoord.z;
}
