#version 430 core

in vec2 texCoords;

out vec4 FragColor;
  
uniform sampler2D SSAOInput;

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(SSAOInput, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(SSAOInput, texCoords + offset).r;
        }
    }
    FragColor = vec4(vec3(result / (4.0 * 4.0)),1.0);
   // FragColor = texture(SSAOInput, texCoords);
}  