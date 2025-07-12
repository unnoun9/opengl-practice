#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "shader.h"
#include "debug.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "vertex_array.h"
#include "renderer.h"
#include "texture.h"
#include "tests/tests.h"

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    float aspect_ratio = 16.0f / 9.0f;
    int window_width = 1280;
    int window_height = int(window_width / aspect_ratio);
    window_height = (window_height < 1) ? 1 : window_height;
    window = glfwCreateWindow(window_width, window_height, "asfkljashdjklfashdf", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    // enable v-sync
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "Error calling glewInit()" << std::endl;
    
    // imgui initialization boiler plate code
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // apparently i had to add this to fix the stretching of the square when aspect ratio was 1; idk why it was happening, since it doesnt make sense because orthographic projection matrix is correctly setup and was supposed to prevent this stretching
    glViewport(0,0,window_width,window_height);

    { // start scope

    CALL(glEnable(GL_BLEND));
    CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    renderer renderer;
    
    void* current_test = nullptr;
    bool test_window_cross = true;

    while (!glfwWindowShouldClose(window))
    {
        CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        renderer.clear();

        // boilerplate code to tell opengl that a new frame is about to begin
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // tests menu
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Clear Color"))
            {
                test::destroy(current_test); // without this, there is a memory leak when x button isn't pressedd but menu buttons are pressed
                current_test = new test::clear_color();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("2D Texture"))
            {
                test::destroy(current_test); // without this, there is a memory leak when x button isn't pressedd but menu buttons are pressed
                current_test = new test::texture_2d(window_width, window_height);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // show the current test; this can be abstracted to use either a templated function with templated argument as the test's type,
        // or by an array of function pointers where the index would be the current_test->name
        if (current_test)
        {
            switch (((test::clear_color*)current_test)->name)
            {
                case test::CLEAR_COLOR:
                {
                    test::clear_color* clear_color = (test::clear_color*)current_test;
                    clear_color->on_render();
                    ImGui::Begin("Clear Color Test", &test_window_cross);
                    clear_color->on_imgui_render();
                    ImGui::End();
                    if (!test_window_cross)
                    {
                        delete clear_color;
                        current_test = nullptr;
                        test_window_cross = true;
                    }
                    break;
                }
                case test::TEXTURE_2D:
                {
                    test::texture_2d* texture_2d = (test::texture_2d*)current_test;
                    texture_2d->on_render();
                    ImGui::Begin("2D Textures Test", &test_window_cross);
                    texture_2d->on_imgui_render();
                    ImGui::End();
                    if (!test_window_cross)
                    {
                        delete texture_2d;
                        current_test = nullptr;
                        test_window_cross = true;
                    }
                    break;
                }
            }
        }

        // render the imgui elements
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    } // end scope

    // boilerplate code to delete all imgui stuff
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}