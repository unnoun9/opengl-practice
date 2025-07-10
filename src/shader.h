#pragma once

#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>

#include "debug.h"

struct shader_program_src { std::string vertex_src, fragment_src; };

struct shader
{
    std::string filepath;

    shader(const std::string& filepath)
        : filepath(filepath), renderer_id(0)
    {
        shader_program_src src = parse_shader(filepath);
        renderer_id = create_shader(src.vertex_src, src.fragment_src);
    }

    ~shader()
    {
        CALL(glDeleteProgram(renderer_id));
    }

    void bind() const
    {
        CALL(glUseProgram(renderer_id));
    }
    
    void unbind() const
    {
        CALL(glUseProgram(0));
    }

    // set uniforms
    void set_uniform_4f(const std::string& name, float v0, float v1, float v2, float v3)
    {
        CALL(glUniform4f(get_uniform_location(name), v0, v1, v2, v3));
    }

    void set_uniform_1f(const std::string& name, float value)
    {
        CALL(glUniform1f(get_uniform_location(name), value));
    }

private:
    unsigned int renderer_id;
    std::unordered_map<std::string, int> uniform_location_cache;

    unsigned int create_shader(const std::string& vertex_shader_src, const std::string& fragment_shader_src)
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

    unsigned int compile_shader(unsigned int type, const std::string& src)
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

    shader_program_src parse_shader(const std::string& filepath)
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

    unsigned int get_uniform_location(const std::string& name)
    {
        // location is more like an id that opengl assigns to each uniform in the shader program
        // returned location is -1 if we made a typo or something or uniform is unused

        // see if we already have location cached
        if (uniform_location_cache.find(name) != uniform_location_cache.end())
            return uniform_location_cache[name];

        CALL(int location = glGetUniformLocation(renderer_id, name.c_str()));
        if (location == -1)
            std::cout << "Warning: uniform '" << name << "' doesn't exist or isn't used!" << std::endl;

        uniform_location_cache[name] = location;
        return location;
    }
};