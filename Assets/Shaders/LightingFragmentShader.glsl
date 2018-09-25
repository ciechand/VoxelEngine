 #version 430 core

in vec2 texCoords;

out vec4 fColor;

uniform sampler2D ColorBuffer;
uniform sampler2D NormalBuffer;
uniform sampler2D PositionBuffer;

uniform samplerCubeArrayShadow shadowMap;
uniform sampler2D SSAOBlurOutput;

//layout(location=12) uniform mat4 P;
layout(location=8) uniform mat4 V;
uniform uint numLights;
uniform float farPlane;

//Information necessary to process all the lights.
struct Light{
	vec4 Position;
	vec4 Color;
};

layout (std430, binding=1) buffer LightData{
	Light lights[];
};

//Variables necessary for basic processing
uint actualNumLights = 0;
float percentPass = 0.0;
float visibility = 0.0;

mat4 biasMatrix = mat4(0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.5,0.5,0.5,1.0);

void main(){
	//Extract things from the textures
	vec4 vColor = texture(ColorBuffer, texCoords);
	vec4 vNormal = (inverse(transpose(inverse(V)))*texture(NormalBuffer, texCoords));
	vec4 shaderCoord = inverse(V)*texture(PositionBuffer, texCoords);
	//vec2 texes = texture(TextureCoordBuffer,texCoords).xy;
	float ssao = texture(SSAOBlurOutput, texCoords).r;
	//vec3 ssao = texture(SSAOBlurOutput, texCoords).xyz;
	//Process basic lighting
	float bias = 0.005;
	vec3 additiveColor = vec3(0.0,0.0,0.0);
	vec4 lightDirection;
	for(int i=0; i<numLights; i++){ 
		vec4 posVec = shaderCoord - lights[i].Position;
		float posVecLength = length(posVec);
		if(posVecLength > lights[i].Color.a) continue;
		actualNumLights++;
		lightDirection = normalize(-posVec);

		float clampedLightDir = clamp(dot(vNormal, lightDirection),0.0,1.0);

		bias = clamp(0.0005*tan(acos(clampedLightDir)),0.00,1.0);

 		visibility = 0.0;
			if(clampedLightDir > 0.0){
				percentPass = texture(shadowMap, vec4(posVec.xyz, i), (posVecLength-bias)/farPlane); 
				//percentPass = 0.5;
				visibility += (percentPass);
			}
		//visibility = 1.0;
		//additiveColor += ((lights[i].Color.rgb/255.0));
		additiveColor +=  0.8*(((vColor.rgb)*visibility)* //Color and visibility Checks
							(clampedLightDir)* // check the angle between normal or surface and light
							((lights[i].Color.rgb/255.0))* // Add in the light color
							(1-clamp(posVecLength/lights[i].Color.a,0.0,1.0))); //fade the edge of the lights
	}


	//vec4 fragmentPos = P*V*shaderCoord;
	//float Occlusion = texture(SSAOBlurredMap, shaderCoord.xy).r;

	vec4 finalColor = vec4(0.0);
	if(actualNumLights != 0){
		finalColor = vec4(((0.2*(vColor.rgb)) + //Ambient
				((additiveColor/float(actualNumLights))))*ssao ,1.0); // Added Diffuse
	}else{
		finalColor = vec4((0.2*(vColor.rgb))*ssao,1.0); // Ambient only
	}
	//finalColor = pow(finalColor, vec4(vec3(1.0/2.2),1.0));
	//fColor = clamp(finalColor, darkenFactor,1.0)-vec4(darkenFactor,darkenFactor,darkenFactor,0.0);
	fColor = finalColor;
	//fColor = vec4(vec3(1.0,0.0,0.0), 1.0);

}
