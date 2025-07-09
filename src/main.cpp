#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

static unsigned int compile_shader(unsigned int type, const std::string& src)
{
    unsigned int id = glCreateShader(type);
    const char* src_ptr = src.c_str();
    glShaderSource(id, 1, &src_ptr, nullptr);
    glCompileShader(id);
    
    // syntax error etc checking
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);  // 2nd argument specifies which parameter we want to query; iv in the name means integer and vector
    if (result == GL_FALSE)
    {
        int message_length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &message_length);
        char* message = (char*)alloca(message_length * sizeof(char));
        glGetShaderInfoLog(id, message_length, &message_length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int create_shader(const std::string& vertex_shader_src, const std::string& fragment_shader_src)
{
    unsigned int program = glCreateProgram();
    unsigned int vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
    unsigned int fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    // add glDetachShader calls here?
    glValidateProgram(program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}

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

    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };

    unsigned int buffer_id;
    glGenBuffers(1, &buffer_id);    // 1. create a "buffer" (just bunch of memory data) and get it's id back
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);   // 2. use the id of a "buffer" to select it to work on it; GL_ARRAY_BUFFER just tell gpu that hey it's an array, expect it to be one
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);    // 3. give the details of the buffer; the size in bytes, the data itself, and how it'll be used
    // 4. now gpu has the data, but we need to tell it how to interpret the data; so this call instructs gpu on that; this call is to be made for each logical attribute (position, color, normals, uv, etc) of the vertex
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);  // 0 assigns an index to this attribute; 2 tells number of coordinates of this attribute; GL_FLOAT just tells that the coordinates are floats; sizeof(float) * 2 tells number of bytes to get to the next vertex, starting from the current vertex's first byte; 0 tells offset of this attribute in the vertex
    glEnableVertexAttribArray(0);   // 5. "enable" the attribute of the vertex whose buffer is bound; we specify the index of the attribute; it doesn't matter whether this call is before glVertexAttribPointer or after because remember, opengl is a state machine -- you give it a state and you performs operations afterwards

    // 6. tell the gpu how to use the data we gave it: shaders!
    std::string vertex_shader =
        "#version 330 core\n"
        ""
        "layout(location = 0) in vec4 position;\n"
        ""
        "void main()\n"
        "{\n"
        "   gl_Position = position;\n"
        "}\n"
    ;
    std::string fragment_shader =
        "#version 330 core\n"
        ""
        "layout(location = 0) out vec4 color;\n"
        ""
        "void main()\n"
        "{\n"
        "   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n"
    ;
    unsigned int shader = create_shader(vertex_shader, fragment_shader);
    glUseProgram(shader);   // 7. bind the shader

    // loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // render here
        glClear(GL_COLOR_BUFFER_BIT);
        
        // 5. issue draw call for the buffer we specified; the selected/bound buffer is drawn; here we tell gpu what primitive to draw (triangle in this case), the starting index in the array, and the count of vertices
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // swap front and back buffers 
        glfwSwapBuffers(window);

        // poll for and process events
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}