#version 330 core
in vec3 vertexColor;

out vec4 FragColor;

#define MAX_NB_ITERATION 500

int get_nb_iteration() {
  // TODO: pass this as a parameter
  float real = (gl_FragCoord.x / 1080.0 - 0.7) * 4.0;
  float img = (gl_FragCoord.y / 1080.0 - 0.5) * 4.0;

  int nb_iteration = 0;
  float const_real = real;
  float const_img = img;

  while (nb_iteration < MAX_NB_ITERATION) {
    float tmp_real = real;
    real = (real * real - img * img) + const_real;
    img = (2.0 * tmp_real * img) + const_img;
      
    float magnitude = real * real + img * img;
    
    // if the point diverge (go out of the 2-unit-diameter-circle), we return direclty the nb of iterations
    if (magnitude > 4.0) { // we compare squared magnitude to avoid a sqrt
      break;
    }

    ++nb_iteration;
  }
  return nb_iteration;
}


void main() {

  int nb_iteration = get_nb_iteration();

  // the point did not diverge
  if (nb_iteration == MAX_NB_ITERATION) {
    gl_FragDepth = 0.0f;
    FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    return;
  }

  float iteration_percent = float(nb_iteration) / MAX_NB_ITERATION;    
  FragColor =  vec4(0.0f, iteration_percent, 0.0f, 1.0f);

  // FragColor = vec4(vertexColor, 1.0f);
}
