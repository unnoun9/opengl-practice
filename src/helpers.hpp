#pragma once
#include "common.h"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 450
#define NUM_ELEMENTS(a) sizeof(a) / sizeof(*a)

////////////////////// DEBUGGING //////////////////////
#define DEBUG
#define ASSERT(x) if (!(x)) __debugbreak();
#ifdef DEBUG
    #define glcall(x) GL_clear_error(); \
        x;\
        ASSERT(GL_log_call(#x, __FILE__, __LINE__))
#else
    #define glcall(x) x
#endif

////////////////////// STRUCTS AND CLASSES //////////////////////
struct Program_source
{
    std::string vertex_source;
    std::string fragment_source;
};

struct Vertex 
{
    glm::vec3 pos;
    glm::vec3 color;
};

struct Shape_data
{
    Vertex* verts;
    int vcount;
    unsigned short* indices;
    int icount;

    Shape_data() : verts(NULL), vcount(0), indices(NULL), icount(0) {}
    int vbuffersize() { return vcount * sizeof(Vertex); }
    int ibuffersize() { return icount * sizeof(unsigned short); }
    void cleanup() { delete[] verts; delete[] indices; }
};

////////////////////// FUNCTION DEFINITIONS //////////////////////
Shape_data make_tri()
{
    Shape_data triangle;

    Vertex tri[] = {
        glm::vec3( 0.0f,  1.0f,  0.0f),
        glm::vec3( 1.0f,  0.0f,  0.0f),

        glm::vec3(-1.0f, -1.0f,  0.0f),
        glm::vec3( 0.0f,  1.0f,  0.0f),

        glm::vec3( 1.0f, -1.0f,  0.0f),
        glm::vec3( 0.0f,  0.0f,  1.0f)
    };
    triangle.vcount = NUM_ELEMENTS(tri);
    triangle.verts = new Vertex[triangle.vcount];
    memcpy(triangle.verts, tri, sizeof(tri));

    unsigned short indices[] = {
        0, 1, 2
    };
    triangle.icount = NUM_ELEMENTS(indices);
    triangle.indices = new unsigned short[triangle.icount];
    memcpy(triangle.indices, indices, sizeof(indices));

    return triangle;
}

void GL_clear_error()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GL_log_call(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (0x" << std::hex << error << "):\n\t{" << file << "}::{line " << line << "}::{" << function << "}\n";
        return false;
    }
    return true;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

unsigned int compile_shader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);

    // compilation errors
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER? "vertex": "fragment") << " shader!\n";
        std::cout << message << std::endl;
        glDeleteShader(id);

        return 0;
    }

    return id;
}

unsigned int create_shader(const std::string& vertex_shader, const std::string& fragment_shader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = compile_shader(GL_VERTEX_SHADER, vertex_shader);
    unsigned int fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    // linking errors
    if (success == GL_FALSE)
    {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetProgramInfoLog(program, length, &length, message);
        std::cout << "Failed to link shader program!\n";
        std::cout << message << std::endl;

        return 0;
    }
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

Program_source parse_shader(const char* filepath)
{
    std::ifstream stream(filepath);
    if (!stream.good())
    {
        std::cout << "File failed to load: " << filepath << std::endl;
        exit(1);
    }
    enum Shader_type
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };
    std::string line;
    std::stringstream ss[2];
    Shader_type type = Shader_type::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = Shader_type::VERTEX;
            if (line.find("fragment") != std::string::npos)
                type = Shader_type::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << "\n";
        }
    }

    return {ss[0].str(), ss[1].str()};
}