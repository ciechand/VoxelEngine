#version 430 core

in vec4 vNormal;
in vec3 vColor;
in vec4 shaderCoord;
out vec4 fColor;

uniform sampler2DArrayShadow shadowMap;
layout(location=11) uniform mat4 P;
layout(location=7) uniform mat4 V;
uniform uint numLights;

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

mat4 biasMatrix = mat4(0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.5,0.5,0.5,1.0);

//Start of the main function.
void main()
{	
	float bias = 0.005;
	vec3 additiveColor = vec3(0.0,0.0,0.0);
	vec4 lightDirection;
	for(int i=1; i<numLights; i++){
		vec4 posInLightSpace = (DepthVP[i] * shaderCoord);
		vec4 finalShadowCoord = (biasMatrix * posInLightSpace);
		finalShadowCoord = finalShadowCoord/finalShadowCoord.w;
		float posVecLength = length(posInLightSpace-(DepthVP[i]*lights[i].Position));
		if(posVecLength > lights[i].Color.a) continue;

		lightDirection = normalize(lights[i].Position-shaderCoord);
		bias = clamp(0.05*tan(acos(clamp(dot(vNormal, lightDirection),0.0,1.0))),0.0,0.0003);
 
			if(clamp(dot(vNormal, lightDirection),0.0,1.0) > 0.0){
				percentPass = texture(shadowMap, vec4(finalShadowCoord.xy, i, finalShadowCoord.z-bias)); 
				visibility = (percentPass);
			}
				//additiveColor += DepthVP[i][0].xyz*100;
				additiveColor +=  (0.95*((vColor/255.0f)*visibility)*(clamp(dot(vNormal, lightDirection),0.0,1.0))*((lights[i].Color.rgb)/255.0)*(1-clamp(posVecLength/lights[i].Color.a,0.0,1.0))); //diffuse
	}

	vec4 finalColor = vec4((0.05*(vColor/255.0f)) + //Ambient
				(additiveColor/actualNumLights),1.0); // Added Diffuse
	finalColor = pow(finalColor, vec4(vec3(1.0/2.2),1.0));
	float darkenFactor = 0.2;
	fColor = clamp(finalColor, darkenFactor,1.0)-vec4(darkenFactor,darkenFactor,darkenFactor,0.0);

	//fColor = vec4(vec3(finalShadowCoord.z), 1.0);
}
