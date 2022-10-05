#version 330 core
out vec4 FragColor;

in vec2 TexCoord;


/**
  This shader is supposed to create the game of life iteration from a sourceTexture
  The final goal is to output this to a virtual frame that will be used to feed the next iteration, alongside display purpose
*/
void main()
{

  FragColor = vec4(TexCoord.r , 0.0f, 0.0f, 1.0f);
  // if (TexCoord.r > 0.5) {
	//   FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
  // } else {
	//   FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
  // }
}