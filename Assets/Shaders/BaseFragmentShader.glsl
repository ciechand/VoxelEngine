#version 430 core

in vec4 vNormal;
in vec3 vColor;
in vec4 shaderCoord;
in vec2 texCoords;
out vec4 fColor;

uniform sampler2DArrayShadow shadowMap;
uniform sampler1D SSAOKernelMap;
uniform sampler2D SSAONoiseMap;

layout(location=12) uniform mat4 P;
layout(location=8) uniform mat4 V;
uniform uint numLights;
uniform uint kernelSize;
uniform vec2 windowSize;

//Information necessary to process all the lights.
struct Light{
	vec4 Position;
	vec4 Color;
};

layout (std430, binding=1) buffer LightData{
	Light lights[];
};

layout (std430, binding=2) buffer LightMatrixData{
	mat4 DepthVP[];
};

//Variables necessary for basic processing
uint actualNumLights = numLights;
float percentPass = 0.0;
float visibility = 1.0;
float radius = 0.5;
float tempOutput = 0.0;
float SSAOBias = 0.000005;

mat4 biasMatrix = mat4(0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.5,0.5,0.5,1.0);

//Start of the main function.
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
	//Process basic lighting
	float bias = 0.005;
	vec3 additiveColor = vec3(0.0,0.0,0.0);
	vec4 lightDirection;
	for(int i=0; i<numLights; i++){
		vec4 posInLightSpace = (DepthVP[i] * shaderCoord);
		vec4 finalShadowCoord = (biasMatrix * posInLightSpace);
		finalShadowCoord = finalShadowCoord/finalShadowCoord.w;
		float posVecLength = length(posInLightSpace-(DepthVP[i]*lights[i].Position));
		if(posVecLength > lights[i].Color.a) continue;

		lightDirection = normalize(lights[i].Position-shaderCoord);
		bias = clamp(0.05*tan(acos(clamp(dot(vNormal, lightDirection),0.0,1.0))),0.00,0.002);
 
			if(clamp(dot(vNormal, lightDirection),0.0,1.0) > 0.0){
				percentPass = texture(shadowMap, vec4(finalShadowCoord.xy, i, finalShadowCoord.z-bias)); 
				//percentPass = 0.5;
				visibility = (percentPass);
			}
				//additiveColor += DepthVP[i][0].xyz*100;
				additiveColor +=  (0.95*((vColor/255.0f)*visibility)*(clamp(dot(vNormal, lightDirection),0.0,1.0))*((lights[i].Color.rgb)/255.0)*(1-clamp(posVecLength/lights[i].Color.a,0.0,1.0))); //diffuse
	}


	//vec4 fragmentPos = P*V*shaderCoord;
	//float Occlusion = texture(SSAOBlurredMap, shaderCoord.xy).r;

	vec4 finalColor = vec4((0.05*(vColor/255.0f)) + //Ambient
				((additiveColor/actualNumLights)*Occlusion),1.0); // Added Diffuse
	//finalColor = pow(finalColor, vec4(vec3(1.0/2.2),1.0));
	//float darkenFactor = 0.2;
	//fColor = clamp(finalColor, darkenFactor,1.0)-vec4(darkenFactor,darkenFactor,darkenFactor,0.0);
	fColor = finalColor;
	//fColor = vec4(vec3(gl_FragCoord.z), 1.0);
}
