#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "shader.h"
#include "debug.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "vertex_array.h"

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
    shader_src src = parse_shader("shaders/basic.glsl");
    unsigned int shader = create_shader(src.vertex_src, src.fragment_src);
    CALL(glUseProgram(shader));   // bind the shader

    // uniforms: data sent from cpu to gpu at the correct location
    CALL(int location = glGetUniformLocation(shader, "u_color")); // location is more like an id that opengl assigns to each uniform in the shader program
    CALL(ASSERT(location != -1)); // returned location is -1 if we made a type or something or uniform is unused
    CALL(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));

    float r = 0.0f;
    float dr = 0.05f;

    // loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // render here
        CALL(glClear(GL_COLOR_BUFFER_BIT));
        
        CALL(glUseProgram(shader));
        CALL(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

        va.bind();

        // 5. issue draw call for to draw the index buffer we specified; the selected/bound buffer is drawn; here we tell gpu what primitive to draw (triangle in this case), number of indices to draw, type of indices, and the a pointer to the indices which isn't required here since we have bound it already above
        CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));  // by contrast, glDrawArrays draws the vertices specified in the vertex buffer sequentially, while glDrawElements draws vertices in the order specified by the index buffer
        
        if (r > 1.0f) dr = -0.05f;
        else if (r < 0.0f) dr = 0.05f;
        r += dr;

        // swap front and back buffers 
        glfwSwapBuffers(window);

        // poll for and process events
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    } // end scope
    glfwTerminate();
    return 0;
}