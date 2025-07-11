#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "shader.h"
#include "debug.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "vertex_array.h"
#include "renderer.h"
#include "texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

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
        -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    // blending - similar to blend modes in image manipulation where we blend top and bottom layer's color
    CALL(glEnable(GL_BLEND));
    // first param is what we multiply each r,g,b component of the source (output from fragment) color
    // second param is what we multiply each r,g,b component of the destination (the target buffer) color
    // both are then added to get the final blended color or according the the mode in glBlendEquation(mode)
    // default behaviour is glBlendFunc(GL_ONE, GL_ZERO); basically we throw away destination color and only consider source color
    CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    // create a vertex array, associate it with a vertex buffer and it's layout, then create an index buffer
    vertex_array va;
    vertex_buffer vb(positions, 4 * 4 * sizeof(float));
    vertex_buffer_layout layout;
    layout.push<float>(2);
    layout.push<float>(2);
    va.add_buffer(vb, layout);
    index_buffer ib(indices, 6);

    // an orthographic projection matrix
    glm::mat4 ortho = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f); // 4 units wide, 3 units tall because our current window aspect ratio is 4:3

    // tell the gpu how to use the data we gave it: shaders! we compile and shaders (vertex and fragment in this case) in to one program, which instructs the gpu
    shader shader("shaders/basic.glsl");
    shader.bind();  // bind the shader
    shader.set_uniform_4f("u_color", 0.8f, 0.3f, 0.8f, 1.0f);   // uniforms: data sent from cpu to gpu at the correct location
    shader.set_uniform_mat4f("u_projection", ortho);
    
    // texture
    texture texture("textures/miku.jpg");
    texture.bind();
    shader.set_uniform_1i("u_texture", 0);  // tell the shader the slot of the texture to sample from

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