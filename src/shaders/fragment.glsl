#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D source_texture;


/**
  This shader is supposed to create the game of life iteration from a sourceTexture
  The final goal is to output this to a virtual frame that will be used to feed the next iteration, alongside display purpose
*/
void main()
{

  // int textureColor = int(textureOffset(texture1, TexCoord, ivec2(0.0, 0.0)).r)



  FragColor = vec4(TexCoord.r , 0.0f, 0.0f, 1.0f);
}