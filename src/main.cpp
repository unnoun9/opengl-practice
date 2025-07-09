#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "shader.h"

int main(void)
{
    GLFWwindow* window;

    // initialize the library
    if (!glfwInit())
        return -1;
    
    // create a windowed mode window and its opengl context
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    // make the window's context current
    glfwMakeContextCurrent(window);
    
    if (glewInit() != GLEW_OK)
        std::cout << "Error calling glewInit()" << std::endl;
    
    std::cout << glGetString(GL_VERSION) << std::endl;

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

    unsigned int buffer_id;
    glGenBuffers(1, &buffer_id);    // 1. create a "buffer" (just bunch of memory data) and get it's id back
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);   // 2. use the id of a "buffer" to select it to work on it; GL_ARRAY_BUFFER just tells opengl to assign this data to the "array buffer" slot of the state
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);    // 3. give the details of the buffer; the size in bytes, the data itself, and how it'll be used
    // 4. now gpu has the data, but we need to tell it how to interpret the data; so this call instructs gpu on that; this call is to be made for each logical attribute (position, color, normals, uv, etc) of the vertex
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);  // 0 assigns an index to this attribute; 2 tells number of coordinates of this attribute; GL_FLOAT just tells that the coordinates are floats; sizeof(float) * 2 tells number of bytes to get to the next vertex, starting from the current vertex's first byte; 0 tells offset of this attribute in the vertex
    glEnableVertexAttribArray(0);   // 5. "enable" the attribute of the vertex whose buffer is bound; we specify the index of the attribute; it doesn't matter whether this call is before glVertexAttribPointer or after because remember, opengl is a state machine -- you give it a state and you performs operations afterwards

    unsigned int index_buffer;
    glGenBuffers(1, &index_buffer);    // 6. create a "buffer" for index buffer and get it's id back
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);   // 7. use the id of a "buffer" to select it to work on it; GL_ELEMENT_ARRAY_BUFFER just tells opengl to assign this data to the "element array buffer" slot of the state
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);    // 8. give the details of the buffer; the size in bytes, the data itself, and how it'll be used

    // 9. tell the gpu how to use the data we gave it: shaders! we compile and shaders (vertex and fragment in this case) in to one program, which instructs the gpu
    shader_src src = parse_shader("shaders/basic.glsl");
    unsigned int shader = create_shader(src.vertex_src, src.fragment_src);
    glUseProgram(shader);   // 10. bind the shader

    // loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // render here
        glClear(GL_COLOR_BUFFER_BIT);
        
        // 5. issue draw call for to draw the index buffer we specified; the selected/bound buffer is drawn; here we tell gpu what primitive to draw (triangle in this case), number of indices to draw, type of indices, and the a pointer to the indices which isn't required here since we have bound it already above
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);  // by contrast, glDrawArrays draws the vertices specified in the vertex buffer sequentially, while glDrawElements draws vertices in the order specified by the index buffer

        // swap front and back buffers 
        glfwSwapBuffers(window);

        // poll for and process events
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}