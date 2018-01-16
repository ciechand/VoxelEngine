#version 430 core

in vec4 lightDirection;
in vec4 vNormal;
in vec3 vColor;
in vec4 shaderCoord;
out vec4 fColor;
out vec4 norm;

uniform sampler2DShadow shadowMap;


float bias = clamp(0.000025*tan(acos(clamp(dot(vNormal, lightDirection),0.0,1.0))),0.0,0.003);
float percentPass = 0.0;
float visibility = 1.0;

float lightDist = 0.65;


vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

void main()
{
	if(clamp(dot(vNormal, lightDirection),0.0,1.0) > 0.0){
		for(int i=0; i<4; i++){
			percentPass = texture(shadowMap,vec3(shaderCoord.xy, (shaderCoord.z-bias))); 
			visibility -= (0.15*(1.0-percentPass));
		}
	}
	
	fColor = 	vec4(0.4*(vColor/255.0f), 1.0) + //Ambient
				vec4(((vColor/255.0f)*visibility), 1.0f)*clamp(dot(vNormal, lightDirection),0.3,1.0); //diffuse
	//fColor = (vec4(vColor,1.0)/100000000)+vec4((shaderCoord.z-bias),(shaderCoord.z-bias),(shaderCoord.z-bias), 1.0);
	norm = vNormal;
}