#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D destination_texture;

// this shader is supposed to only display destination_texture to the screen
// because destination_texture is a Red-only texture, we only use the red coordinate
// and we display a picture as levels of gray with respect to this red color

void main()
{   
  float textureColorRed = float(texture(destination_texture, TexCoord).r);

  FragColor = vec4(textureColorRed, textureColorRed, textureColorRed, 1.0f);
}