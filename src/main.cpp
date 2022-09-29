#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// namespace related to screen positioning
// NB: This could be refactored as a singleton class or a "static-like" class, but for the sake of quick prototyping, I went with a namespace
namespace screen
{

    const float zooming_translation_factor = 0.01f;
    const float zoom_scaling_factor = 0.02f;
    unsigned int width = 1080;
    unsigned int height = 1080;
    float center_x{0.0f};
    float center_y{0.0f};
    float zoom{1.0};

    void set_dimensions(int _width, int _height)
    {
        width = _width;
        height = _height;
    }

    void move_up()
    {
        center_y = center_y + zooming_translation_factor * zoom;
        // preventing center from exiting screeen
        if (center_y > 1.0f)
        {
            center_y = 1.0f;
        }
    }

    void move_down()
    {
        center_y = center_y - zooming_translation_factor * zoom;
        // preventing center from exiting screeen
        if (center_y < -1.0f)
        {
            center_y = -1.0f;
        }
    }

    void move_left()
    {
        center_x = center_x - zooming_translation_factor * zoom;
        // preventing center from exiting screeen
        if (center_x < -1.0f)
        {
            center_x = -1.0f;
        }
    }

    void move_right()
    {
        center_x = center_x + zooming_translation_factor * zoom;
        // preventing center from exiting screeen
        if (center_x > 1.0f)
        {
            center_x = 1.0f;
        }
    }

    void zoom_in()
    {
        zoom = zoom * (1.0f - zoom_scaling_factor);
        // minimal zoom
        if (zoom < 0.00001f)
        {
            zoom = 0.00001f;
        }
    }

    void zoom_out()
    {

        zoom = zoom * (1.0f + zoom_scaling_factor);
        // max zoom
        if (zoom > 1.0f)
        {
            zoom = 1.0f;
        }
    }
}

// namespace related to frame per second measurement
// NB: see namespace fps for design-decision explanation
namespace fps
{

    const int time_beteen_fps_display = 1.0; // duration between which no fps is shown to avoid stdout throttling
    // number of frame since last iteration
    int num_frame{0};
    float last_frame_time{0.0f};

    /**
     * Update the current time and display fps and spf
     * */
    void countFPS()
    {
        double current_time = glfwGetTime();
        num_frame++;

        double interval{current_time - last_frame_time};
        // we don't want to print the fps all the time. We sample the number of frames during one second
        if (interval >= 1.0)
        {
            std::cout << "fps " << num_frame << " | " << 1000.0 * interval / num_frame << "ms / frame\n";
            num_frame = 0;
            last_frame_time = current_time;
        }
    }

}

int main()
{

    // Loading shaders files content
    // ------------------------------------------
    std::ifstream vertexShaderFile("src/shaders/vertex.glsl");
    if (!vertexShaderFile.is_open())
    {
        std::cout << "could not open vertexShaderFile" << std::endl;
        std::exit(1);
    }
    std::stringstream vertexStrStream;
    vertexStrStream << vertexShaderFile.rdbuf();
    std::string vertexShaderFileString = vertexStrStream.str();
    const char *vertexShaderSource = vertexShaderFileString.c_str();
    vertexShaderFile.close();

    std::ifstream fragmentShaderFile("src/shaders/fragment.glsl");
    if (!fragmentShaderFile.is_open())
    {
        std::cout << "could not open fragmentShaderFile" << std::endl;
        std::exit(1);
    }
    std::stringstream fragmentStrStream;
    fragmentStrStream << fragmentShaderFile.rdbuf();
    std::string fragmentShaderFileString = fragmentStrStream.str();
    const char *fragmentShaderSource = fragmentShaderFileString.c_str();
    fragmentShaderFile.close();

    // Initialize and configure glfw
    // ------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create glfw window
    // ------------------------------------------
    GLFWwindow *window = glfwCreateWindow(screen::width, screen::height, "Mandelbrot", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // load all OpenGL function pointers for glad
    // ------------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Create vertex shader
    // ------------------------------------------
    unsigned int vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderId, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShaderId);

    // checking if shader was successfully built
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // Create fragment shader
    // ------------------------------------------
    unsigned int fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderId, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShaderId);

    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // linking shader within a common Shader program
    unsigned int shaderProgramId = glCreateProgram();
    glAttachShader(shaderProgramId, vertexShaderId);
    glAttachShader(shaderProgramId, fragmentShaderId);
    glLinkProgram(shaderProgramId);
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderProgramId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    // once they are loaded into the shader program, we can get rid of initial shaders
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    // setting up vertex data, configuring vertex attributes

    // each "line" has this shape
    /// x, y, z, r, g, b
    float vertices[] = {
        -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f};

    /** Creating a square from 2 triangles
        2 -- 1
        |  / |
        | /  |
        0 -- 3
    */
    unsigned int indices[] = {
        0, 1, 2, // first triangle
        0, 3, 1  // second triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bindings
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // FRAGMENT SHADER
    // ------------------------------------------
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(shaderProgramId);

    std::cout << "launching main loop" << std::endl;
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        fps::countFPS();
        // input
        // -----
        processInput(window);

        // provinding parameters with uniform buffer
        glUniform1f(glGetUniformLocation(shaderProgramId, "zoom"), screen::zoom);
        glUniform1f(glGetUniformLocation(shaderProgramId, "center_x"), screen::center_x);
        glUniform1f(glGetUniformLocation(shaderProgramId, "center_y"), screen::center_y);
        glUniform1f(glGetUniformLocation(shaderProgramId, "screen_width"), screen::width);
        glUniform1f(glGetUniformLocation(shaderProgramId, "screen_height"), screen::height);

        // render
        // --------------------------------------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // swap buffer to display the painted frame
        glfwSwapBuffers(window);
        // poll IO events (mouse, keyboard)
        glfwPollEvents();
    }

    // cleaning up remaining objects
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgramId);

    // freeing GLFW ressources
    glfwTerminate();
    return 0;
}

/**
 * Process GLFW inputs (pressed/released keys) that happened during the current frame
 * */
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // going up
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        screen::move_up();
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        screen::move_down();
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        screen::move_left();
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        screen::move_right();
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        screen::zoom_in();
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        screen::zoom_out();
    }
}

/**
 * If the window is resized, this function will be triggered
 * */
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // updating glViewPort dimensions
    glViewport(0, 0, width, height);
    // Saving current dimension
    screen::set_dimensions(width, height);
}
