#include "common.h"
#include "helpers.hpp"

int main(void)
{
    ////////////////////// INITIALIZATION //////////////////////
    if (!glfwInit())
    {
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "learning", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    std::cout << glGetString(GL_VERSION) << std::endl;
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glEnable(GL_DEPTH_TEST);

    ////////////////////// BUFFERS //////////////////////
    Shape_data tri = make_tri();
    unsigned int vert_id;
    glGenBuffers(1, &vert_id);
    glBindBuffer(GL_ARRAY_BUFFER, vert_id);
    glBufferData(GL_ARRAY_BUFFER, tri.vbuffersize(), tri.verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (char*)(sizeof(float) * 3));

    unsigned int index_id;
    glGenBuffers(1, &index_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, tri.ibuffersize(), tri.indices, GL_STATIC_DRAW);
    tri.cleanup();
    
    ////////////////////// SHADERS //////////////////////
    Program_source source_code = parse_shader("res/shaders/basic.shader");
    unsigned int shader = create_shader(source_code.vertex_source, source_code.fragment_source);
    glUseProgram(shader);
    
    ////////////////////// MAIN LOOP //////////////////////
    while (!glfwWindowShouldClose(window))
    {
        // inputs
        process_input(window);

        // rendering commands
        glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);
        glcall(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0));

        // check and call events and swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}