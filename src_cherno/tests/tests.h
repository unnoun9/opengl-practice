#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vertex_buffer.h"
#include "index_buffer.h"
#include "vertex_array.h"
#include "shader.h"
#include "texture.h"

namespace tests {

enum name
{
    CLEAR_COLOR, TWO_2D_TEXTURES, IDK,
    NUM_TEST_NAMES
};

struct clear_color
{
    name name;
    float color[4];
};

struct two_2d_textures
{
    name name;
    int window_width, window_height;
    glm::mat4 proj, view, model;                // model view projection matrices
    glm::vec3 translationa, translationb;       // translation vectors to translate objects A and B
    
    vertex_buffer* vb;
    index_buffer* ib;
    vertex_array* va;
    shader* shader_program;
    texture* tex;
};

struct idk
{
    name name;
};

void* init(name test_name);
void render(void* test, void** test_loc, bool* window_cross);
void destroy(void* test);

}