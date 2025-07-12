#pragma once

#include <GL/glew.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

#include "tests.h"
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

    texture_2d(const int window_width, const int window_height)
        : name(TEXTURE_2D), window_width(window_width), window_height(window_height),
          proj(glm::ortho(0.0f, (float)window_width, 0.0f, (float)window_height, -1.0f, 1.0f)), // size of our window
          view(glm::translate(glm::mat4(1.0), glm::vec3(0,0,0))), // translates everything according to the 2nd param vector to simulate camera moving opposite to the that vector
          translationa(window_width / 2.0f - 100, window_height / 2.0f, 0.0f), translationb(window_width / 2.0f + 100, window_height / 2.0f, 0.0f)
    {
        float positions[] = {
            -50.0f, -50.0f, 0.0f, 0.0f,
             50.0f, -50.0f, 1.0f, 0.0f,
             50.0f,  50.0f, 1.0f, 1.0f,
            -50.0f,  50.0f, 0.0f, 1.0f
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
        va = std::make_unique<vertex_array>();
        vb = std::make_unique<vertex_buffer>(positions, 4 * 4 * sizeof(float));
        vertex_buffer_layout layout;
        layout.push<float>(2);
        layout.push<float>(2);
        va->add_buffer(*vb, layout);
        ib = std::make_unique<index_buffer>(indices, 6);
        
        // shader and texture
        shader_program = std::make_unique<shader>("shaders/basic.glsl");
        tex = std::make_unique<texture>("textures/miku.jpg"); // perhaps make this a unique ptr too?
        tex->bind();
        shader_program->bind();
        shader_program->set_uniform_1i("u_texture", 0); // tell the shader the slot of the texture to sample from
    }

    void on_render()
    {
        CALL(glClearColor(0, 0, 0, 1));
        CALL(glClear(GL_COLOR_BUFFER_BIT));

        // fine to make it because it's size is literally zero right now
        renderer renderer;

        { // draw object A, using translationa
            // translate everything by the translation vector and then rotate everything by rotation_angle
            // multiplication is in "m * v * p" order when the matrices are row-major, but here since they are colum-major,
            // the order is reverse (opengl and glm work with column-major)
            glm::mat4 model = glm::translate(glm::mat4(1.0), translationa); 
            glm::mat4 mvp = proj * view * model; 
            shader_program->set_uniform_mat4f("u_mvp", mvp);
            renderer.draw(*va, *ib, *shader_program);
        }

        { // draw object B, using translationb
            glm::mat4 model = glm::translate(glm::mat4(1.0), translationb);
            glm::mat4 mvp = proj * view * model; 
            shader_program->set_uniform_mat4f("u_mvp", mvp);
            renderer.draw(*va, *ib, *shader_program);
        }
    }

    void on_imgui_render()
    {
        ImGui::SliderFloat3("Translate object A", &translationa.x, 0, window_width, "%.0f");
        ImGui::SliderFloat3("Translate object B", &translationb.x, 0, window_width, "%.0f");
        ImGui::NewLine();
        ImGui::Text("%.0fFPS", ImGui::GetIO().Framerate);
        ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
        ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
        ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION));
        ImGui::Text("Shading Language: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    }
};

}