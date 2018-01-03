#version 430 core

in vec4 vNormal;
in vec3 vColor;
in vec4 shaderCoord;
out vec4 fColor;
out vec4 norm;

uniform sampler2D shadowMap;

float bias = 0.005f;
float tempColor = 0.0f;
float visibility = 1.0f;


vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

void main()
{
	// float zval = gl_FragCoord.z*gl_FragCoord.w;
	// fColor = vec4(zval,zval,zval, 1.0f);
	// for(int i=0; i<4; i++){
	// 	tempColor = texture(shadowMap, shaderCoord.xy+ poissonDisk[i]/2000.0).z;
	// 	if(tempColor < shaderCoord.z-bias)
	// 		visibility -= 0.2f;
	// }
	tempColor = texture(shadowMap, shaderCoord.xy).z;
	if(tempColor < shaderCoord.z-bias)
		visibility = 0.5f;
	fColor = vec4(((vColor/255.0f)*visibility*(tempColor)),1.0f);
	norm = vNormal;
}