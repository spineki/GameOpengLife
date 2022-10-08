#include <glad/glad.h>  // needed to handle opengl function pointers
#include <GLFW/glfw3.h> // needed for windowing management
#include <fstream>      // needed to read shaders from file
#include <iostream>     // needed for std::cout
#include <sstream>      // needed to simply get strings from files
#include <cmath>        //! TODO needed ?
#include <random>       // needed to have random number for grid initialisation
#include <string_view>

//
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// namespace related to screen positioning
// NB: This could be refactored as a singleton class or a "static-like" class, but for the sake of quick prototyping, I went with a namespace
namespace screen
{

    unsigned int width = 512;
    unsigned int height = 512;

    void set_dimensions(int _width, int _height)
    {
        width = _width;
        height = _height;
    }
}

// namespace related to frame per second measurement
// NB: see namespace fps for design-decision explanation
namespace fps
{

    const int time_between_fps_display = 1.0; // duration between which no fps is shown to avoid stdout throttling
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
        if (interval >= time_between_fps_display)
        {
            std::cout << "fps " << num_frame << " | " << 1000.0 * interval / num_frame << "ms / frame\n";
            num_frame = 0;
            last_frame_time = current_time;
        }
    }

}
/**
 * Return the content of a shader file as a string, exit if no shader file found
 **/
std::string tryGetShaderContent(const char *path)
{

    std::ifstream shaderFile(path);
    if (!shaderFile.is_open())
    {
        std::cout << "could not open shader at " << path << std::endl;
        std::exit(1);
    }
    std::stringstream shaderStrStream;
    shaderStrStream << shaderFile.rdbuf();
    shaderFile.close();

    std::string shaderFileString = shaderStrStream.str();
    return shaderFileString;
}

int main()
{

    std::random_device rd;
    // mersen twister random number generator
    std::mt19937 mt(rd());
    // we want to generatre random number as 0 or 1, (2 cell states)
    std::uniform_int_distribution<> dist(0, 1);

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
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // load all OpenGL function pointers for glad
    // ------------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Create vertex shader
    // ------------------------------------------
    std::string vertex_shader_content = tryGetShaderContent("src/shaders/vertex.glsl");
    const char *vertex_shader_source = vertex_shader_content.c_str();
    unsigned int vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_id, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader_id);

    // checking if shader was successfully built
    int success;
    char infoLog[512];
    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // Create fragment shader
    // ------------------------------------------
    std::string fragment_shader_content = tryGetShaderContent("src/shaders/fragment.glsl");
    const char *fragment_shader_source = fragment_shader_content.c_str();

    unsigned int fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_id, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader_id);

    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragment_shader_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // Create fragment shader
    // ------------------------------------------
    std::string disp_fragment_shader_content = tryGetShaderContent("src/shaders/dispFragment.glsl");
    const char *disp_fragment_shader_source = disp_fragment_shader_content.c_str();

    unsigned int disp_fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(disp_fragment_shader_id, 1, &disp_fragment_shader_source, NULL);
    glCompileShader(disp_fragment_shader_id);

    glGetShaderiv(disp_fragment_shader_id, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(disp_fragment_shader_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::DISP_FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    unsigned int disp_shader_program_id = glCreateProgram();
    glAttachShader(disp_shader_program_id, vertex_shader_id);
    glAttachShader(disp_shader_program_id, disp_fragment_shader_id);
    glLinkProgram(disp_shader_program_id);
    glGetProgramiv(disp_shader_program_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(disp_shader_program_id, 512, NULL, infoLog);
        std::cout << "ERROR::DISP_SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    // linking shader within a common Shader program
    unsigned int shader_program_id = glCreateProgram();
    glAttachShader(shader_program_id, vertex_shader_id);
    glAttachShader(shader_program_id, fragment_shader_id);
    glLinkProgram(shader_program_id);
    glGetProgramiv(shader_program_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader_program_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    // once they are loaded into the shader program, we can get rid of initial shaders
    glDeleteShader(vertex_shader_id);
    glDeleteShader(disp_fragment_shader_id);
    glDeleteShader(fragment_shader_id);

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

    unsigned int
        VBO, // VBO = vertex buffer object
        VAO, // VAO = vertex array object
        EBO, // EBO = Element buffer object
        FBO; // FBO = Frame buffer object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // framebuffer
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    /**
     * creating a first texture. Initially, it will contain random data to initialise the simulation
     * */
    unsigned int first_texture;
    glGenTextures(1, &first_texture);
    glBindTexture(GL_TEXTURE_2D, first_texture);

    // Creating random data
    int nb_pixel = screen::width * screen::height;
    float *data = {new float[nb_pixel]{}};
    for (int i = 0; i < nb_pixel; ++i)
    {

        data[i] = dist(mt);
    }
    // loading data into texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, screen::width, screen::height, 0, GL_RED, GL_HALF_FLOAT, data);
    // freeing data because texture makes an inner copy
    delete[] data;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // linking first_texture to the first color entry of the framebuffer
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, first_texture, 0);

    unsigned int secondTexture;
    glGenTextures(1, &secondTexture);
    glBindTexture(GL_TEXTURE_2D, secondTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, screen::width, screen::height, 0, GL_RED, GL_HALF_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // linking secondTexture to the first color entry of the framebuffer
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, secondTexture, 0);

    GLenum status;
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (status)
    {
    case GL_FRAMEBUFFER_COMPLETE:
        break;
    default:
        std::cout << "error while checking framebuffer" << status << std::endl;
    }

    int compiling_error = glGetError();

    if (compiling_error != GL_NO_ERROR)
    {

        std::cout << "glGetError :" << compiling_error << std::endl;
    }

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

    glUseProgram(shader_program_id);

    std::cout << "launching main loop" << std::endl;

    int current_color_attachment = GL_COLOR_ATTACHMENT1;

    int current_source_texture = first_texture;
    int current_destination_texture = secondTexture;

    glBindVertexArray(VAO);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        fps::countFPS();

        // input
        // -----
        processInput(window);

        // render
        // --------------------------------------
        // selecting the framebuffer not to write on the screen
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        // selecting the current colorAttachment to draw to (will select the output texture)
        glDrawBuffer(current_color_attachment);
        // use the game-of-life related shader
        glUseProgram(shader_program_id);
        // cleaning previous frame
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // cleaning color buffer
        glClear(GL_COLOR_BUFFER_BIT);

        // now, we need to pass the source texture as parameter as uniform
        // --------------------------------------
        // for location 0
        glActiveTexture(GL_TEXTURE0); // Texture unit 0
        // bind source texture
        glBindTexture(GL_TEXTURE_2D, current_source_texture); // setting the associated texture
        // pass it to the shader
        int source_texture_location = glGetUniformLocation(shader_program_id, "source_texture"); // setting the associated texture
        glUniform1i(source_texture_location, 0);                                                 // 0 first uniform value

        // Now, rendering to the screen to use the shader
        // --------------------------------------
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // writting now back in default frame buffer
        // --------------------------------------
        // using the display shader, that will only display stored texture
        glUseProgram(disp_shader_program_id);
        // going back to default framebuffer

        // for location 0
        glActiveTexture(GL_TEXTURE0); // Texture unit 0
        // bind source texture
        glBindTexture(GL_TEXTURE_2D, current_source_texture); // setting the associated texture

        int previous_texture_location = glGetUniformLocation(disp_shader_program_id, "previous_texture"); // setting the associated texture
        glUniform1i(previous_texture_location, 0);                                                        // 0 first uniform value

        // pass it to the shader
        // for location 1
        glActiveTexture(GL_TEXTURE1); // Texture unit 1
        //  bind destination texture
        glBindTexture(GL_TEXTURE_2D, current_destination_texture); // setting the associated texture
        int current_texture_location = glGetUniformLocation(disp_shader_program_id, "current_texture");
        glUniform1i(current_texture_location, 1);

        // going back to default framebuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // swaping with framebuffer color is going to receive next iteration
        if (current_color_attachment == GL_COLOR_ATTACHMENT1)
        {
            current_color_attachment = GL_COLOR_ATTACHMENT0;
        }
        else
        {
            current_color_attachment = GL_COLOR_ATTACHMENT1;
        }

        // but now, we need to use the new texture as the next source

        std::swap(current_source_texture, current_destination_texture);

        // swap buffer to display the painted frame
        glfwSwapBuffers(window);
        // poll IO events (mouse, keyboard)
        glfwPollEvents();
    }

    // cleaning up remaining objects
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &first_texture);
    glDeleteTextures(1, &secondTexture);
    glDeleteProgram(shader_program_id);
    glDeleteProgram(disp_shader_program_id);

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
}

/**
 * If the window is resized, this function will be triggered
 * */
void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    // updating glViewPort dimensions
    glViewport(0, 0, width, height);
    // Saving current dimension
    //! Currently, the windows is resize, but the texture are not reloaded, so this will lead to strange effects
    screen::set_dimensions(width, height);
}
