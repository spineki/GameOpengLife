#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <random>
#include <unistd.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// namespace related to screen positioning
// NB: This could be refactored as a singleton class or a "static-like" class, but for the sake of quick prototyping, I went with a namespace
namespace screen
{

    const float zooming_translation_factor = 0.01f;
    const float zoom_scaling_factor = 0.02f;
    unsigned int width = 512;
    unsigned int height = 512;
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

    std::random_device rd;
    // mersen twister
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 255.0);

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

    std::ifstream dispFragmentShaderFile("src/shaders/dispFragment.glsl");
    if (!dispFragmentShaderFile.is_open())
    {
        std::cout << "could not open dispFragmentShaderFile" << std::endl;
        std::exit(1);
    }
    std::stringstream dispFragmentStrStream;
    dispFragmentStrStream << dispFragmentShaderFile.rdbuf();
    std::string dispFragmentShaderFileString = dispFragmentStrStream.str();
    const char *dispFragmentShaderSource = dispFragmentShaderFileString.c_str();
    dispFragmentShaderFile.close();

    // Initialize and configure glfw
    // ------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create glfw window
    // ------------------------------------------
    GLFWwindow *window = glfwCreateWindow(screen::width, screen::height, "Game of no life", NULL, NULL);
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

    // Create fragment shader
    // ------------------------------------------
    unsigned int dispFragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(dispFragmentShaderId, 1, &dispFragmentShaderSource, NULL);
    glCompileShader(dispFragmentShaderId);

    glGetShaderiv(dispFragmentShaderId, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(dispFragmentShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::DISP_FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    unsigned int dispShaderProgramId = glCreateProgram();
    glAttachShader(dispShaderProgramId, vertexShaderId);
    glAttachShader(dispShaderProgramId, dispFragmentShaderId);
    glLinkProgram(dispShaderProgramId);
    glGetProgramiv(dispShaderProgramId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(dispShaderProgramId, 512, NULL, infoLog);
        std::cout << "ERROR::DISP_SHADER::PROGRAM::LINKING_FAILED\n"
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
    glDeleteShader(dispFragmentShaderId);
    glDeleteShader(fragmentShaderId);

    // setting up vertex data, configuring vertex attributes
    // 4 vertices to create 2 triangles
    // each "line" has this shape
    /// x, y, z
    float vertices[] = {
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,   // x, y, z, tx, ty
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // x, y, z, tx, ty
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // x, y, z, tx, ty
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f}; // x, y, z, tx, ty

    /** Creating a square from 2 triangles
        2 -- 1
        |  / |
        | /  |
        3 -- 0
    */
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    // VBO = vertex buffer object
    // VAO = vertex array object
    // EBO = Element buffer object
    unsigned int VBO, VAO, EBO, frameBuffer;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // framebuffer
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    // creating a texture
    unsigned int sourceTexture;
    glGenTextures(1, &sourceTexture);
    glBindTexture(GL_TEXTURE_2D, sourceTexture);

    int nb_pixel = screen::width * screen::height;
    char *data = {new char[nb_pixel]{}};
    for (int i = 0; i < nb_pixel; ++i)
    {

        data[i] = static_cast<unsigned int>(dist(mt)) % 2;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, screen::width, screen::height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // linking sourceTexture to the first color entry of the framebuffer
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, sourceTexture, 0);

    unsigned int destinationTexture;
    glGenTextures(1, &destinationTexture);
    glBindTexture(GL_TEXTURE_2D, destinationTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, screen::width, screen::height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // linking destinationTexture to the first color entry of the framebuffer
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, destinationTexture, 0);

    GLenum status;
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (status)
    {
    case GL_FRAMEBUFFER_COMPLETE:
        std::cout << "good" << std::endl;
        break;
    default:
        std::cout << "error while checking" << std::endl;
    }

    std::cout << "glGetError" << glGetError() << std::endl;

    // bindings
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // FRAGMENT SHADER
    // ------------------------------------------
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // texture position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(shaderProgramId);

    std::cout << "launching main loop" << std::endl;

    int colorAttachment = GL_COLOR_ATTACHMENT1;

    int currentSourceTexture = sourceTexture;
    int currentDestinationTexture = destinationTexture;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        fps::countFPS();
        // input
        // -----
        processInput(window);

        // provinding parameters with uniform buffer
        //! reinit this later

        // render
        // --------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glDrawBuffer(colorAttachment);
        glUseProgram(shaderProgramId);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);                                            // Texture unit 0
        glBindTexture(GL_TEXTURE_2D, currentSourceTexture);                      // setting the associated texture
        glUniform1f(glGetUniformLocation(shaderProgramId, "source_texture"), 0); // 0first uniform value

        glBindVertexArray(VAO); // try to remove this after (only used once)
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // writting in default frame buffer
        // --------------------------------------

        glUseProgram(dispShaderProgramId);                                                // why?
        glActiveTexture(GL_TEXTURE0);                                                     // Texture unit 0
        glBindTexture(GL_TEXTURE_2D, currentDestinationTexture);                          // setting the associated texture
        glUniform1f(glGetUniformLocation(dispShaderProgramId, "destination_texture"), 0); // 0first uniform value

        // going back to default framebuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // swaping with framebuffer color is going to receive next iteration
        if (colorAttachment == GL_COLOR_ATTACHMENT1)
        {
            colorAttachment = GL_COLOR_ATTACHMENT0;
        }
        else
        {
            colorAttachment = GL_COLOR_ATTACHMENT1;
        }

        // but now, we need to use the new texture as the next source
        int swap = currentDestinationTexture;
        currentDestinationTexture = currentSourceTexture;
        currentSourceTexture = swap;

        // swap buffer to display the painted frame
        glfwSwapBuffers(window);
        // poll IO events (mouse, keyboard)
        glfwPollEvents();
        // usleep(1'000 * 1000);
    }

    // cleaning up remaining objects
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &sourceTexture);
    glDeleteTextures(1, &destinationTexture);
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
