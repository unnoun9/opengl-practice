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

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

int main(void)
{
    GLFWwindow* window;

    // initialize the library
    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create a windowed mode window and its opengl context
    float aspect_ratio = 16.0f / 9.0f;
    int window_width = 960;
    int window_height = int(window_width / aspect_ratio);
    window_height = (window_height < 1) ? 1 : window_height;
    window = glfwCreateWindow(window_width, window_height, "asfkljashdjklfashdf", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    // make the window's context current
    glfwMakeContextCurrent(window);
    
    // pass the number of screen refreshed to wait before swapping the frong and back buffers; passing 1 enables vertical synchronization (v-sync)
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
    float centerx = window_width / 2.0f, centery = window_height / 2.0f;
    float halfw = 200 / 2.0f, halfh = 200 / 2.0f;
    float positions[] = {
        centerx - halfw, centery - halfh, 0.0f, 0.0f,
        centerx + halfw, centery - halfh, 1.0f, 0.0f,
        centerx + halfw, centery + halfh, 1.0f, 1.0f,
        centerx - halfw, centery + halfh, 0.0f, 1.0f
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
    vertex_array va;
    vertex_buffer vb(positions, 4 * 4 * sizeof(float));
    vertex_buffer_layout layout;
    layout.push<float>(2);
    layout.push<float>(2);
    va.add_buffer(vb, layout);
    index_buffer ib(indices, 6);

    // model-view-projection matrices
    glm::mat4 proj = glm::ortho(0.0f, (float)window_width, 0.0f, (float)window_height, -1.0f, 1.0f); // size of our window
    glm::mat4 view = glm::translate(glm::mat4(1.0), glm::vec3(-100.0f, 0.0f, 0.0f)); // translate everything 100px to the left to simulate camera moving 100px to the right
    glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(200.0f, 200.0f, 0.0f)); // translate everything 100px to the left to simulate camera moving 100px to the right
    glm::mat4 mvp = proj * view * model; // multiplication is in "m * v * p" order when the matrices are row-major, but here since they are colum-major, the order is reverse (opengl and glm work with column-major); when rewatching 3b1b's LA videos, think about mvp vs pvm order -- order of transformation

    // tell the gpu how to use the data we gave it: shaders! we compile and shaders (vertex and fragment in this case) in to one program, which instructs the gpu
    shader shader("shaders/basic.glsl");
    shader.bind();  // bind the shader
    shader.set_uniform_mat4f("u_mvp", mvp);
    
    // texture
    texture texture("textures/miku.jpg");
    texture.bind();
    shader.set_uniform_1i("u_texture", 0);  // tell the shader the slot of the texture to sample from

    va.unbind();
    vb.unbind();
    ib.unbind();
    shader.unbind();

    renderer renderer;

    // loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // render here
        renderer.clear();
        
        // boilerplate code to tell opengl that a new frame is about to begin
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (!io.WantCaptureMouse)
        {
            // input handling
        }

        shader.bind();
        renderer.draw(va, ib, shader);

        // imgui window
        ImGui::Begin("asdjklfhjklhasljka");
        ImGui::Text("%.1fFPS", ImGui::GetIO().Framerate);
        ImGui::Separator();
        ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
        ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
        ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION));
        ImGui::Text("Shading Language: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
        ImGui::End();

        // render the imgui elements
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // swap front and back buffers 
        glfwSwapBuffers(window);

        // poll for and process events
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