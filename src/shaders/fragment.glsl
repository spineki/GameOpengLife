#version 330 core
// not useful in the mandelbrot example anymore because we compute the color ourselves
in vec3 vertexColor;

// output color for the current fragment
out vec4 FragColor;

// parameters necessary to find the viewport
uniform float center_x;
uniform float center_y;
uniform float screen_height;
uniform float screen_width;
uniform float zoom;

// max number of iteration to test if an initial complex number diverge when iterating mandeldbrot equation 
#define MAX_NB_ITERATION 500

int get_nb_iteration() {

  // getting current coordinates according to zoom
  // we split real and img part of the corresponding complex number
  float initial_real  = ((gl_FragCoord.x / screen_width - 0.5) * zoom + center_x) * 4.0f;
  float initial_img   = ((gl_FragCoord.y / screen_height - 0.5) * zoom + center_y) * 4.0f;

  int nb_iteration = 0;
  // creating real and img that will be changed through iterations
  float real = initial_real;
  float img = initial_img;

  while (nb_iteration < MAX_NB_ITERATION) {
    float tmp_real = real;
    real = (real * real - img * img) + initial_real;
    img = (2.0 * tmp_real * img) + initial_img;
      
    // we use squared magnitude to avoid a sqrt
    float squared_magnitude = real * real + img * img;
    
    // if the point diverge (go out of the 2-unit-diameter-circle), we return direclty the nb of iterations
    if (squared_magnitude > 4.0) {  // 4.0 because 2^2
      break;
    }

    ++nb_iteration;
  }
  return nb_iteration;
}


// This Shader
void main() {

  int nb_iteration = get_nb_iteration();

  // the point did not diverge
  if (nb_iteration == MAX_NB_ITERATION) {
    FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    return;
  }

  // normalizing the number of iteration
  float iteration_percent = float(nb_iteration) / MAX_NB_ITERATION;    
  FragColor =  vec4(0.0f, iteration_percent, 0.0f, 1.0f);
}
