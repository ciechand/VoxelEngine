#version 430 core

in vec3 colorOut;
in vec2 textureCoordinatesOut;
in vec4 Normal;
in vec2 texPosOut;
in vec2 texSizeOut;
out vec4 fColor;
out vec4 norm;

uniform int TotalTexSize;
uniform sampler2D textureSample;

void main()
{
	float positionx = texPosOut.x/(TotalTexSize);
	float positiony = texPosOut.y/(TotalTexSize);
	vec2 offset = vec2(((textureCoordinatesOut.x*texSizeOut.x)/(TotalTexSize)),((textureCoordinatesOut.y*texSizeOut.y)/(TotalTexSize)));
	vec2 trueTexCoords = vec2(positionx+offset.x, positiony+offset.y);

	vec4 texColor = texture(textureSample, trueTexCoords);

	if(texColor.a == 0.0) discard;

	vec4 colorPer = vec4(texColor.r/255.0f,texColor.g/255.0f,texColor.b/255.0f,texColor.a);
	vec4 transformColor = vec4(colorOut.r,colorOut.g,colorOut.b,1.0f);

	fColor  = transformColor*colorPer;
	norm = Normal;
}