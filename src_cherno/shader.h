#pragma once

#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>

#include "debug.h"

#include "glm/glm.hpp"

struct shader_program_src { std::string vertex_src, fragment_src; };

struct shader
{
    std::string filepath;
    unsigned int renderer_id;
    std::unordered_map<std::string, int> uniform_location_cache;


    shader(const std::string& filepath);
    ~shader();
    void bind() const;
    void unbind() const;

    // set uniforms
    void set_uniform_1i(const std::string& name, int value);
    void set_uniform_1f(const std::string& name, float value);
    void set_uniform_4f(const std::string& name, float v0, float v1, float v2, float v3);
    void set_uniform_mat4f(const std::string& name, const glm::mat4& matrix);

    // helper functions
    unsigned int create_shader(const std::string& vertex_shader_src, const std::string& fragment_shader_src);
    unsigned int compile_shader(unsigned int type, const std::string& src);
    shader_program_src parse_shader(const std::string& filepath);
    int get_uniform_location(const std::string& name);
};