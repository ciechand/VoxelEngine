 #version 430 core

in vec2 texCoords;

out vec4 fColor;

uniform sampler2D ColorBuffer;
uniform sampler2D NormalBuffer;
uniform sampler2D PositionBuffer;

uniform sampler2DArrayShadow shadowMap;
uniform sampler2D SSAOBlurOutput;

//layout(location=12) uniform mat4 P;
layout(location=8) uniform mat4 V;
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
uint actualNumLights = 0;
float percentPass = 0.0;
float visibility = 0.0;

mat4 biasMatrix = mat4(0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.5,0.5,0.5,1.0);

void main(){
	//Extract things from the textures
	vec4 vColor = texture(ColorBuffer, texCoords);
	vec4 vNormal = normalize(texture(NormalBuffer, texCoords)*(transpose(inverse(V))));
	vec4 shaderCoord = texture(PositionBuffer, texCoords)*(transpose(inverse(V)));
	//vec2 texes = texture(TextureCoordBuffer,texCoords).xy;
	float ssao = texture(SSAOBlurOutput, texCoords).r;
	//vec3 ssao = texture(SSAOBlurOutput, texCoords).xyz;
	//Process basic lighting
	float bias = 0.005;
	vec3 additiveColor = vec3(0.0,0.0,0.0);
	vec4 lightDirection;
	for(int i=1; i<numLights; i++){
		vec4 posInLightSpace = (DepthVP[i] * shaderCoord);
		vec4 finalShadowCoord = (posInLightSpace);

		finalShadowCoord = finalShadowCoord/finalShadowCoord.w;
		finalShadowCoord = biasMatrix * finalShadowCoord;
		float posVecLength = length(posInLightSpace-(DepthVP[i]*lights[i].Position));
		if(posVecLength > lights[i].Color.a) continue;
		actualNumLights++;
		lightDirection = normalize(lights[i].Position-shaderCoord);
		float clampedLightDir = clamp(dot(vNormal, lightDirection),0.0,1.0);
		bias = clamp(0.002 *tan(acos(clampedLightDir)),0.00,0.002);
 		visibility = 0.0;
			if(clampedLightDir > 0.0){
				percentPass = texture(shadowMap, vec4(finalShadowCoord.xy, i, finalShadowCoord.z-bias)); 
				//percentPass = 0.5;
				visibility += (percentPass);
			}
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
		finalColor = vec4((0.2*(vColor.rgb)) + //Ambient
				((additiveColor/float(actualNumLights))*ssao) ,1.0); // Added Diffuse
	}else{
		finalColor = vec4((0.2*(vColor.rgb)),1.0); // Diffuse Only
	}
	//finalColor = pow(finalColor, vec4(vec3(1.0/2.2),1.0));
	//fColor = clamp(finalColor, darkenFactor,1.0)-vec4(darkenFactor,darkenFactor,darkenFactor,0.0);
	fColor = finalColor;
	//fColor = vec4(vec3(ssao), 1.0);

}
