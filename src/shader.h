#pragma once

#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "debug.h"

struct shader_src { std::string vertex_src, fragment_src; };

static unsigned int create_shader(const std::string& vertex_shader_src, const std::string& fragment_shader_src);
static unsigned int compile_shader(unsigned int type, const std::string& src);
static shader_src parse_shader(const std::string& filepath);

static unsigned int create_shader(const std::string& vertex_shader_src, const std::string& fragment_shader_src)
{
    CALL(unsigned int program = glCreateProgram());
    unsigned int vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
    unsigned int fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);
    CALL(glAttachShader(program, vertex_shader));
    CALL(glAttachShader(program, fragment_shader));
    CALL(glLinkProgram(program));
    // add glDetachShader calls here?
    CALL(glValidateProgram(program));
    CALL(glDeleteShader(vertex_shader));
    CALL(glDeleteShader(fragment_shader));
    return program;
}

static unsigned int compile_shader(unsigned int type, const std::string& src)
{
    CALL(unsigned int id = glCreateShader(type));
    const char* src_ptr = src.c_str();
    CALL(glShaderSource(id, 1, &src_ptr, nullptr));
    CALL(glCompileShader(id));
    
    // syntax error etc checking
    int result;
    CALL(glGetShaderiv(id, GL_COMPILE_STATUS, &result));  // 2nd argument specifies which parameter we want to query; iv in the name means integer and vector
    if (result == GL_FALSE)
    {
        int message_length;
        CALL(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &message_length));
        char* message = (char*)alloca(message_length * sizeof(char));
        CALL(glGetShaderInfoLog(id, message_length, &message_length, message));
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
        std::cout << message << std::endl;
        CALL(glDeleteShader(id));
        return 0;
    }

    return id;
}

static shader_src parse_shader(const std::string& filepath)
{
    std::ifstream file(filepath);

    enum shader_type
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1,
        NUM_TYPES
    };

    std::string line;
    std::stringstream shader_streams[NUM_TYPES];
    shader_type current_type = NONE;

    while (getline(file, line))
    {
        // if we have "#shader" in the line
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                current_type = VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                current_type = FRAGMENT;
            }
        }
        else
        {
            shader_streams[(int)current_type] << line << '\n';
        }
    }

    return { shader_streams[VERTEX].str(), shader_streams[FRAGMENT].str() };
}