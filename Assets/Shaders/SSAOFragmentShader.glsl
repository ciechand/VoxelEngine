#version 430 core

in vec2 texCoords;

out vec4 fColor;

uniform sampler2D NormalBuffer;
uniform sampler2D positionBuffer;

uniform sampler1D SSAOKernelMap;
uniform sampler2D SSAONoiseMap; 

layout(location=12) uniform mat4 P;
layout(location=8) uniform mat4 V;

uniform uint kernelSize;
uniform vec2 windowSize;

//Define Variables for SSAO Processing.
float radius = 0.5;
float SSAOBias = 0.025;
float power = 2.0;
//mat4 biasMatrix = mat4(0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.5,0.5,0.5,1.0);

void main()
{	
	//Retrieve from textures
	vec3 shaderCoord =(texture(positionBuffer, texCoords)).xyz;
	vec3 vNormal = normalize((texture(NormalBuffer, texCoords).rgb));
	//process SSAO
	vec2 NoiseScale = vec2(windowSize.x/4.0, windowSize.y/4.0);
	vec3 randVec = normalize(texture(SSAONoiseMap, texCoords*NoiseScale).xyz);

	vec3 tangent = normalize(randVec - vNormal * dot(randVec, vNormal));
	vec3 bitTangent = cross(vNormal, tangent);
	mat3 TBN = mat3(tangent, bitTangent, vNormal);

	//Begin Processing of SSAO with inputed Kernel Samples
	float Occlusion = 0.0;
	for(int i=0; i<kernelSize; ++i){
		vec4 kernelSample = texture(SSAOKernelMap, float(i)/float(kernelSize));
		vec3 TSample = TBN*(kernelSample.rgb);
		TSample = shaderCoord + TSample * radius;

		vec4 newCoord = vec4(TSample, 1.0);
		newCoord = P*newCoord;
		newCoord.xyz /= newCoord.w;
		newCoord.xyz = newCoord.xyz * 0.5 + 0.5;

		float sampleDepth = texture(positionBuffer,newCoord.xy).z;
		float rangeCheck = smoothstep(0.0,1.0, radius / abs(shaderCoord.z-sampleDepth));
		Occlusion += (sampleDepth >= TSample.z+SSAOBias?1.0:0.0)*rangeCheck;
	}

	Occlusion =  1.0 - (Occlusion/float(kernelSize));
	//fColor = vec4(vec3(Occlusion),1.0);
	fColor = vec4(vec3(pow(Occlusion, power)),1.0);
}