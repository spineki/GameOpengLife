#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;
// texture of previous iteration
uniform sampler2D previous_texture;
// texture of current iteration
uniform sampler2D current_texture;

// this shader is supposed to only display current_texture to the screen
// because current_texture is a Red-only texture, we only use the red coordinate
// and we display a picture as levels of gray with respect to this red color

// in order to better highlight active parts of the screen, we will display with a brighter color nely created cells

void main()
{   
  float previous_texture_color_red  = float(texture(previous_texture, TexCoord).r);
  float current_texture_color_red   = float(texture(current_texture, TexCoord).r);

  // We tint the color of the current point
  vec3 base_color = vec3(0.0, 1.0, 1.0) * current_texture_color_red; // wil be 0 if the cell is dead
  // if the texture changed, we want to display it in brighter color, else, we want to display it in blue

  float dark_coefficient = 0.3; // dark coefficient applied to cells that were not created between previous and current iteration

  if (previous_texture_color_red != current_texture_color_red) {
    FragColor = vec4(base_color, 1.0);
  } else {
    FragColor = vec4(base_color * dark_coefficient, 1.0);
  }
}