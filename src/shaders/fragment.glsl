#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

void main()
{
  // only red 
	FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}