#version 330 core
// output color for the current fragment
out vec4 FragColor; // should be available through gl_FragColor

// passing texture for game of life
uniform sampler2D u_source_texture;

// parameters necessary to find the viewport
// uniform float center_x;
// uniform float center_y;
// uniform float screen_height;
// uniform float screen_width;
// uniform float zoom;

int getCell(int col, int row) {
  // need to get texture information
  vec2 tex_coord = vec2(0.0f, 0.0f);
  return int( texture2D( u_source_texture, tex_coord + vec2(col, row) ).r);
} 

// This Shader
void main() {
  vec2 tex_coord = vec2(0.5f, 0.5f);

  if ( int( texture2D( u_source_texture, tex_coord ).r) == 0) {
    FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
  } else {
    FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
  }
  return;




  // summing adjacent neighbours' colour
  int nb_neighbour =  getCell(-1, -1) +
                      getCell( 0, -1) +
                      getCell( 1, -1) +
                      getCell( 1,  0) +
                      getCell( 1,  1) +
                      getCell( 0,  1) +
                      getCell(-1,  1) +
                      getCell(-1,  0);

  // creation
  if (nb_neighbour == 3) {
    // white
    FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
  }
  // still alive or still dead
  else if (nb_neighbour == 2) {
    // keeping same color
    float current_cell = float(getCell(0, 0));
    FragColor = vec4(current_cell, current_cell, current_cell, 1.0f); // 1.0f for alpha canal
  } else {
    // back to dead
    FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
  }
}
