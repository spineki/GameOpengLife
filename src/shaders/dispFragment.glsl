#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{   

  // int textureColor = int(textureOffset(texture1, TexCoord, ivec2(0.0, 0.0)).r)

  float textureColorRed = float(texture(texture1, TexCoord).r);

  FragColor = vec4(textureColorRed, textureColorRed, textureColorRed, 1.0f);

}