#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base.h"
#include "gl_abstraction.cpp"

int window_width, window_height;
static input input_state;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE)
    {
        input_state.esc_pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
        if (action == GLFW_PRESS)
        {
            input_state.cursor_captured = !input_state.cursor_captured;
            if (input_state.cursor_captured)
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            else
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
    }
    if (key == GLFW_KEY_W)
        input_state.w_pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    if (key == GLFW_KEY_S)
        input_state.s_pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    if (key == GLFW_KEY_A)
        input_state.a_pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    if (key == GLFW_KEY_D)
        input_state.d_pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    if (key == GLFW_KEY_SPACE)
        input_state.space_pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    if (key == GLFW_KEY_LEFT_CONTROL)
        input_state.ctrl_pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
}

void window_resize_callback(GLFWwindow *window, int width, int height)
{
    window_width = width;
    window_height = height;
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    input_state.mouse_pos.x = xpos;
    input_state.mouse_pos.y = ypos;
}

int main(void)
{
    GLFWwindow *window;

    if (!glfwInit())
        return -1;
    
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    float aspect_ratio = 16.0f / 9.0f;
    window_width = 1000;
    window_height = int(window_width / aspect_ratio);
    window_height = (window_height < 1) ? 1 : window_height;
    window = glfwCreateWindow(window_width, window_height, "Graphics Pad", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, window_resize_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwMakeContextCurrent(window);
    input_state.cursor_captured = true;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // enable v-sync
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Error loading glad\n");
        return -1;
    }
    
    // imgui initialization boiler plate code
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glEnable(GL_DEPTH_TEST);

    shape_data shape = make_cube();
    unsigned int vertex_buffer_id;
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, shape.num_vertices * sizeof(vertex), shape.vertices, GL_STATIC_DRAW);
    // glVertexAttrib can be used if we don't wanna call glEnableVertexAttribArray with glVertex Attrib, the attrib
    // will have the same value given as the arguments, and not vary per vertex as per the vertex buffer
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));

    unsigned int index_buffer_id;
    glGenBuffers(1, &index_buffer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape.num_indices * sizeof(short unsigned), shape.indices, GL_STATIC_DRAW);

    free(shape.vertices);
    free(shape.indices);

    char *vertex_shader_src = read_code("shaders/vertex.glsl");
    char *fragment_shader_src = read_code("shaders/fragment.glsl");

    unsigned int vertex_shader_id = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
    unsigned int fragment_shader_id = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);
    unsigned int program_id = glCreateProgram();
    link_program(program_id, vertex_shader_id, fragment_shader_id);
    glUseProgram(program_id);

    free(vertex_shader_src);
    free(fragment_shader_src);

    float angle_x = 36.0f;
    float angle_y = 0.0f;
    float angle_z = 0.0f;

    camera cam;
    cam.position = {0, 0, 0};
    cam.direction = {0, 0, -1};
    cam.up = {0, 1, 0};
    cam.right = {1, 0, 0};
    double initial_mouse_posx;
    double initial_mouse_posy;
    glfwGetCursorPos(window, &initial_mouse_posx, &initial_mouse_posy);
    cam.old_mouse_pos = {initial_mouse_posx, initial_mouse_posy};

    unsigned int transformation_mat_buffer_id;
    glGenBuffers(1, &transformation_mat_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, transformation_mat_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4)*2, NULL, GL_DYNAMIC_DRAW);
    // size is 4 because atmost 4 floats can be sent as attrib at once, so we send all 4 rows, one-by-one as attribs
    // and then in the shader it's implied because we use mat4 type there so we don't have to do layout(location > 2) there
    // and thus, using mat4 in the shader at location=2 consumes location=3,4,5 as well because each attrib can be at max 4 floats
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void *)(sizeof(float) * 0));
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void *)(sizeof(float) * 4));
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void *)(sizeof(float) * 8));
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void *)(sizeof(float) * 12));
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
    // divides indices of the instance data by the second argument, so say if it was 2, then each two instace would have same instance data
    // another way to think is if second argument is 0, the attrib advances per-vertex, if it is 1, it advances per-instance
    // if it is N, it advances N instances
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);

    while (!glfwWindowShouldClose(window))
    {
        if (input_state.cursor_captured)
            camera_update_wrt_input(&cam, &input_state);
        else
            cam.old_mouse_pos = input_state.mouse_pos;

        mat4 projection_mat = perspective(radians(80.0f), ((float)window_width / window_height), 0.1f, 10.0f);
        mat4 view_mat = lookAt(cam.position, cam.position+cam.direction, cam.up);

        mat4 full_transforms[2];
        full_transforms[0] = translate(projection_mat*view_mat, vec3(-1.0f, 0.0f, -3.0f));
        full_transforms[0] = rotate(full_transforms[0], radians(angle_x), vec3(1.0f, 0.0f, 0.0f));
        full_transforms[0] = rotate(full_transforms[0], radians(angle_y), vec3(0.0f, 1.0f, 0.0f));
        full_transforms[0] = rotate(full_transforms[0], radians(angle_z), vec3(0.0f, 0.0f, 1.0f));
        full_transforms[1] = translate(projection_mat*view_mat, vec3(1.0f, 0.0f, -3.75f));
        full_transforms[1] = rotate(full_transforms[1], radians(126.0f), vec3(0.0f, 1.0f, 0.0f));
        full_transforms[1] = full_transforms[1];
        glBufferData(GL_ARRAY_BUFFER, sizeof(mat4)*2, full_transforms, GL_DYNAMIC_DRAW);

        // render
        glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, window_width, window_height);
        glDrawElementsInstanced(GL_TRIANGLES, shape.num_indices, GL_UNSIGNED_SHORT, 0, 2);

        // boilerplate code to tell opengl that a new frame is about to begin
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (!input_state.cursor_captured)
        {
            ImGui::Begin("Rotate");
            ImGui::SliderFloat("Rotation X", &angle_x, 0, 360, "%.0f");
            ImGui::SliderFloat("Rotation Y", &angle_y, 0, 360, "%.0f");
            ImGui::SliderFloat("Rotation Z", &angle_z, 0, 360, "%.0f");
            ImGui::NewLine();
            ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
            ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
            ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
            ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION));
            ImGui::Text("Shading Language: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
            ImGui::End();
        }

        // render the imgui elements
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // boilerplate code to delete all imgui stuff
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glUseProgram(0);
    glDeleteProgram(program_id);
    glfwTerminate();
    return 0;
}