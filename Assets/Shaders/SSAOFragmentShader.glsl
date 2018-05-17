#version 430 core

in vec4 vNormal;
in vec4 shaderCoord;
in vec2 texCoords;
out float fColor;

uniform sampler2DArrayShadow shadowMap;
uniform sampler1D SSAOKernelMap;
uniform sampler2D SSAONoiseMap;

layout(location=12) uniform mat4 P;
layout(location=8) uniform mat4 V;

uniform uint kernelSize;
uniform vec2 windowSize;

//Define Variables for SSAO Processing.
float radius = 0.5;
float SSAOBias = 0.000005;

void main()
{	
	//process SSAO
	vec2 NoiseScale = vec2(windowSize.x/4.0, windowSize.y/4.0);
	vec4 inputNormal = normalize(vNormal);
	vec3 newNorm = (V*inputNormal).xyz;
	vec3 randVec = texture(SSAONoiseMap, texCoords*NoiseScale).xyz;
	//vec3 randVec = vec3(0.0f,0.0f,1.0f);

	vec3 tangent = normalize(randVec - newNorm * dot(randVec, newNorm));
	vec3 bitTangent = cross(newNorm, tangent);
	mat3 TBN = mat3(tangent, bitTangent, newNorm);
	
	vec3 fragPos = (V*shaderCoord).xyz;
	//Begin Processing of SSAO with inputed Kernel Samples
	float Occlusion = 0.0;
	for(int i=0; i<kernelSize; i++){
		vec4 kernelSample = texture(SSAOKernelMap, i);
		vec3 TSample = TBN*kernelSample.rgb;
		TSample = fragPos + TSample * radius;

		vec4 newCoord = vec4(TSample, 1.0);
		newCoord = P*newCoord;
		newCoord.xyz /= newCoord.w;
		newCoord.xyz = newCoord.xyz * 0.5 + 0.5;
		//newCoord = biasMatrix * newCoord;

		float sampleDepth = texture(shadowMap,vec4(newCoord.x, newCoord.y, 0, newCoord.z-SSAOBias));
		//Occlusion += (sampleDepth >= 0.5)?1.0:0.0;
		Occlusion += sampleDepth;
	}
	Occlusion = (Occlusion/kernelSize);
	fColor = Occlusion;
}