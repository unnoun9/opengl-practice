#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "shader.h"
#include "debug.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "vertex_array.h"
#include "renderer.h"

int main(void)
{
    GLFWwindow* window;

    // initialize the library
    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create a windowed mode window and its opengl context
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    // make the window's context current
    glfwMakeContextCurrent(window);
    
    // pass the number of screen refreshed to wait before swapping the frong and back buffers; passing 1 enables vertical synchronization (v-sync)
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "Error calling glewInit()" << std::endl;
    
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n" << std::endl;

    { // start scope
    float positions[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f,
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    // create a vertex array, associate it with a vertex buffer and it's layout, then create an index buffer
    vertex_array va;
    vertex_buffer vb(positions, 4 * 2 * sizeof(float));
    vertex_buffer_layout layout;
    layout.push<float>(2);
    va.add_buffer(vb, layout);
    index_buffer ib(indices, 6);

    // tell the gpu how to use the data we gave it: shaders! we compile and shaders (vertex and fragment in this case) in to one program, which instructs the gpu
    shader shader("shaders/basic.glsl");
    shader.bind();  // bind the shader
    shader.set_uniform_4f("u_color", 0.8f, 0.3f, 0.8f, 1.0f);   // uniforms: data sent from cpu to gpu at the correct location

    va.unbind();
    vb.unbind();
    ib.unbind();
    shader.unbind();

    renderer renderer;

    float r = 0.0f;
    float dr = 0.05f;

    // loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // render here
        renderer.clear();
        
        shader.bind();
        shader.set_uniform_4f("u_color", r, 0.3f, 0.8f, 1.0f);
        renderer.draw(va, ib, shader);

        if (r > 1.0f) dr = -0.05f;
        else if (r < 0.0f) dr = 0.05f;
        r += dr;

        // swap front and back buffers 
        glfwSwapBuffers(window);

        // poll for and process events
        glfwPollEvents();
    }
    } // end scope
    glfwTerminate();
    return 0;
}