#version 430 core

in vec4 vNormal;
in vec3 vColor;
in vec4 shaderCoord;
in vec2 texCoords;
layout (location=0) out vec4 NormalBuffer;
layout (location=1) out vec4 ColorBuffer;
layout (location=2) out vec4 PositionBuffer;
layout (location=3) out vec4 TextureCoordBuffer;
out float fragDepth;

//Start of the main function.
void main()
{	
	NormalBuffer = vec4(vNormal.xyz,1.0);
	ColorBuffer = vec4(vColor, 1.0);
	PositionBuffer = vec4(shaderCoord.xyz, 1.0);
	TextureCoordBuffer = vec4(texCoords, 0.0, 1.0);
	fragDepth = gl_FragCoord.z;

	// NormalBuffer = vec4(vec3(0.5),1.0);
	// ColorBuffer = vec4(vec3(0.5), 1.0);
	// PositionBuffer = vec4(vec3(0.5), 1.0);
	// TextureCoordBuffer = vec4(vec2(0.5), 0.0, 1.0);
	// fragDepth = 0.5;
}
