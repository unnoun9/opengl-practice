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

int window_width, window_height;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE); // also free allocated memory, from for instance the tests
}

void window_resize_callback(GLFWwindow* window, int width, int height)
{
    window_width = width;
    window_height = height;
}

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    float aspect_ratio = 4.0f / 3.0f;
    window_width = 960;
    window_height = int(window_width / aspect_ratio);
    window_height = (window_height < 1) ? 1 : window_height;
    window = glfwCreateWindow(window_width, window_height, "Cherno OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, window_resize_callback);
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

    { // start scope

    CALL(glEnable(GL_BLEND));
    CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    
    void* current_test = nullptr;
    bool test_window_cross = true;

    while (!glfwWindowShouldClose(window))
    {
        CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        clear();

        // boilerplate code to tell opengl that a new frame is about to begin
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // the menu for tests, which really are like scenes to be frank
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Clear Color"))
            {
                // without this destroy, there is a memory leak when x button isn't pressed but menu buttons are pressed
                tests::destroy(current_test);
                current_test = tests::init(tests::CLEAR_COLOR);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Two 2D Textures"))
            {
                // without this destroy, there is a memory leak when x button isn't pressed but menu buttons are pressed
                tests::destroy(current_test);
                current_test = tests::init(tests::TWO_2D_TEXTURES);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("IDK"))
            {
                // without this destroy, there is a memory leak when x button isn't pressed but menu buttons are pressed
                tests::destroy(current_test);
                current_test = tests::init(tests::IDK);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // render the test
        tests::render(current_test, &current_test, &test_window_cross);

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