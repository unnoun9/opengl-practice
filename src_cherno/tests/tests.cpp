#include <GL/glew.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdio.h>

#include "tests.h"
#include "renderer.h"

// both init and render functions intitialize and render tests by checking first which test it is by name,
// but this can be optimized using array of function pointers where array is indexed by the name of the test
// and the function pointer is different and unique for each test

extern int window_width, window_height;

void* tests::init(name test_name)
{
    switch (test_name)
    {
        case CLEAR_COLOR:
        {
            clear_color* test = new clear_color;
            if (test == NULL)
            {
                fprintf(stderr, "`clear_color` test could not be allocated\n");
                return NULL;
            }

            test->name = CLEAR_COLOR;
            test->color[0] = 0.2f;
            test->color[1] = 0.3f;
            test->color[2] = 0.7f;
            test->color[3] = 1.0f;

            return test;
        }

        case TWO_2D_TEXTURES:
        {
            two_2d_textures* test = new two_2d_textures;
            if (test == NULL)
            {
                fprintf(stderr, "`two_2d_textures` test could not be allocated\n");
                return NULL;
            }

            test->name = TWO_2D_TEXTURES;
            test->window_width = window_width;
            test->window_height = window_height;
            // size of our window
            test->proj = glm::ortho(0.0f, (float)test->window_width, 0.0f, (float)test->window_height, -1.0f, 1.0f);
            // translates everything according to the 2nd param vector to simulate camera moving opposite to that vector
            test->view = glm::translate(glm::mat4(1.0), glm::vec3(0,0,0));
            test->translationa = glm::vec3(test->window_width / 2.0f - 100, test->window_height / 2.0f, 0.0f);
            test->translationb = glm::vec3(test->window_width / 2.0f + 100, test->window_height / 2.0f, 0.0f);

            // blending - similar to blend modes in image manipulation where we blend top and bottom layer's color
            CALL(glEnable(GL_BLEND));
            // first param is what we multiply each r,g,b component of the source (output from fragment) color
            // second param is what we multiply each r,g,b component of the destination (the target buffer) color
            // both are then added to get the final blended color or according the the mode in glBlendEquation(mode)
            // default behaviour is glBlendFunc(GL_ONE, GL_ZERO); basically we throw away destination color and only consider source color
            CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

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

            // create a vertex array, associate it with a vertex buffer and it's layout, then create an index buffer
            test->va = new vertex_array();
            test->vb = new vertex_buffer(positions, 4 * 4 * sizeof(float));
            vertex_buffer_layout layout;
            layout.push<float>(2);
            layout.push<float>(2);
            test->va->add_buffer(*test->vb, layout);
            test->ib = new index_buffer(indices, 6);
            
            // shader and texture
            test->shader_program = new shader("shaders/basic.glsl");
            test->tex = new texture("textures/miku.jpg");
            test->tex->bind();
            test->shader_program->bind();
            // tell the shader the slot of the texture to sample from
            test->shader_program->set_uniform_1i("u_texture", 0);

            return test;
        }

        case IDK:
        {
            idk* test = new idk;
            if (test == NULL)
            {
                fprintf(stderr, "`idk` test could not be allocated\n");
                return NULL;
            }

            test->name = IDK;

            float verts[] = {
                 0.0f,  0.0f,
                 1.0f,  1.0f,
                -1.0f,  1.0f,
                -1.0f, -1.0f,
                 1.0f, -1.0f,
            };

            unsigned int va;
            glGenVertexArrays(1, &va);
            glBindVertexArray(va);
            unsigned int vb;
            glGenBuffers(1, &vb);
            glBindBuffer(GL_ARRAY_BUFFER, vb);
            glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

            unsigned short indices[] = { 0,1,2, 0,3,4};
            unsigned int ib;
            glGenBuffers(1, &ib);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            return test;
        }

        default:
        {
            fprintf(stderr, "Error: Unknown test type %d\n", test_name);
            return NULL;
        }
    }

    return NULL;
}

void tests::render(void* test, void** test_loc, bool* window_cross)
{
    if (test == NULL)
    {
        return;
    }

    // this assumes that each test struct's first field is the name
    name test_name = *(name*)test;

    switch (test_name)
    {
        case CLEAR_COLOR:
        {
            clear_color* col = (clear_color*)test;
            
            CALL(glClearColor(col->color[0], col->color[1], col->color[2], col->color[3]));
            CALL(glClear(GL_COLOR_BUFFER_BIT));

            ImGui::Begin("Clear Color Test", window_cross);
            ImGui::ColorEdit4("Clear Color", col->color);
            ImGui::End();

            // delete the test-related memory if the imgui window's cross button is pressed
            if (*window_cross == false)
            {
                destroy(col);
                *test_loc = NULL;
                *window_cross = true;
            }
            break;
        }

        case TWO_2D_TEXTURES:
        {
            two_2d_textures* two = (two_2d_textures*)test;

            CALL(glClearColor(0, 0, 0, 1));
            CALL(glClear(GL_COLOR_BUFFER_BIT));

            { // draw object A, using translationa
                // translate everything by the translation vector and then rotate everything by rotation_angle
                // multiplication is in "m * v * p" order when the matrices are row-major, but here since they are colum-major,
                // the order is reverse (opengl and glm work with column-major)
                two->model = glm::translate(glm::mat4(1.0), two->translationa); 
                glm::mat4 mvp = two->proj * two->view * two->model; 
                two->shader_program->set_uniform_mat4f("u_mvp", mvp);
                draw(*two->va, *two->ib, *two->shader_program);
            }

            { // draw object B, using translationb
                two->model = glm::translate(glm::mat4(1.0), two->translationb);
                glm::mat4 mvp = two->proj * two->view * two->model; 
                two->shader_program->set_uniform_mat4f("u_mvp", mvp);
                draw(*two->va, *two->ib, *two->shader_program);
            }

            ImGui::Begin("Two 2D Textures Test", window_cross);
            ImGui::SetNextItemWidth(200.0f); 
            ImGui::SliderFloat("##ax", &two->translationa.x, 0, two->window_width, "%.0f"); ImGui::SameLine();
            ImGui::SetNextItemWidth(200.0f); 
            ImGui::SliderFloat("##ay", &two->translationa.y, 0, two->window_height, "%.0f");
            ImGui::SetNextItemWidth(200.0f); 
            ImGui::SliderFloat("##bx", &two->translationb.x, 0, two->window_width, "%.0f"); ImGui::SameLine();
            ImGui::SetNextItemWidth(200.0f); 
            ImGui::SliderFloat("##by", &two->translationb.y, 0, two->window_height, "%.0f");
            ImGui::NewLine();
            ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
            ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
            ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
            ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION));
            ImGui::Text("Shading Language: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
            ImGui::End();

            // delete the test-related memory if the imgui window's cross button is pressed
            if (*window_cross == false)
            {
                destroy(two);
                *test_loc = NULL;
                *window_cross = true;
            }
            break;
        }

        case IDK:
        {
            idk* idek = (idk*)test;
            
            glViewport(0, 0, window_width, window_height);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
            
            ImGui::Begin("IDK Test", window_cross);
            ImGui::End();

            // delete the test-related memory if the imgui window's cross button is pressed
            if (*window_cross == false)
            {
                destroy(idek);
                *test_loc = NULL;
                *window_cross = true;
            }
            break;
        }
    }
}

void tests::destroy(void* test)
{
    using namespace tests;

    if (!test)
    {
        return;
    }

    // cast it to any arbitrary test to *only* access the name member
    name name = ((clear_color*)test)->name;

    // find out the type of the test and delete accordingly
    switch (name)
    {
        case CLEAR_COLOR: delete (clear_color*)test; return;

        case TWO_2D_TEXTURES:
        {
            two_2d_textures* two = (two_2d_textures*)test;
            delete two->vb;
            delete two->ib;
            delete two->va;
            delete two->shader_program;
            delete two->tex;
            delete two;
            return;
        }

        case IDK: delete (idk*)test; return;

        default: fprintf(stderr, "Unknown test type/name passed to `delete_name`\n");
    }
}