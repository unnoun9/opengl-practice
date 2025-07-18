#pragma once

#include <GL/glew.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

#include "test.h"
#include "debug.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "vertex_buffer_layout.h"
#include "vertex_array.h"
#include "shader.h"
#include "texture.h"
#include "renderer.h"

namespace test {

struct texture_2d
{
    name name;
    std::unique_ptr<vertex_array> va;
    std::unique_ptr<vertex_buffer> vb;
    std::unique_ptr<index_buffer> ib;
    std::unique_ptr<shader> shader_program;
    std::unique_ptr<texture> tex;
    glm::mat4 proj, view, model;                // model view projection matrices
    glm::vec3 translationa, translationb;       // translation vectors to translate objects A and B
    int window_width, window_height;

    texture_2d(const int window_width, const int window_height);
    void on_render();
    void on_imgui_render();
};

}