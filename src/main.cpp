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

using glm::vec3;
using glm::mat4;

int window_width, window_height;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE); // other memory deallocations?
}

void window_resize_callback(GLFWwindow *window, int width, int height)
{
    window_width = width;
    window_height = height;
}

unsigned int compile_shader(int unsigned type, const char *src)
{
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);
    
    int compile_status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE)
    {
        int info_log_length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &info_log_length);
        char *buffer = (char*)alloca(info_log_length * sizeof(char));
        glGetShaderInfoLog(id, info_log_length, &info_log_length, buffer);
        printf("Failed to compile %s shader := %s\n", 
               (type == GL_VERTEX_SHADER ? "vertex" : "fragment"), buffer);
        glDeleteShader(id);
        return 0;
    }

    return id;
}

void link_program(int unsigned program_id, int unsigned vertex_shader_id, int unsigned fragment_shader_id)
{
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);

    int link_status;
    glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE)
    {
        int info_log_length;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
        char *buffer = (char*)alloca(info_log_length);
        glGetProgramInfoLog(program_id, info_log_length, &info_log_length, buffer);
        printf("Shader Linking Error := %s\n", buffer);
    }

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);
}

char* read_code(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Failed to load file %s\n", filename);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char*)malloc(file_size + 1);
    if (!buffer)
    {
        printf("Failed to allocate memory for file %s\n", filename);
        fclose(file);
        exit(1);
    }

    size_t bytes_read = fread(buffer, 1, file_size, file);
    buffer[bytes_read] = '\0';

    fclose(file);
    return buffer;
}

struct vertex
{
    vec3 position, color;
};

struct shape_data
{
    vertex *vertices;
    int unsigned num_vertices;
    short unsigned *indices;
    int unsigned num_indices;
};

shape_data make_triangle()
{
    shape_data tri;

    vertex vertices[] = {
        vec3(0.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0),
        vec3(-1.0, -1.0, 0.0), vec3(0.0, 1.0, 0.0),
        vec3(1.0, -1.0, 0.0), vec3(0.0, 0.0, 1.0),
    };

    short unsigned indices[] = { 0, 1, 2 };

    tri.num_vertices = sizeof(vertices) / sizeof(*vertices);
    tri.vertices = (vertex*)malloc(tri.num_vertices * sizeof(vertex));
    memcpy(tri.vertices, vertices, sizeof(vertices));
    
    tri.num_indices = sizeof(indices) / sizeof(*indices);
    tri.indices = (short unsigned*)malloc(tri.num_indices * sizeof(short unsigned));
    memcpy(tri.indices, indices, sizeof(indices));

    return tri;
}

shape_data make_cube()
{
    shape_data cube;
    
    vertex verts[] = {
        vec3(-1.0,  1.0,  1.0), vec3(1.0, 0.0, 0.0),
        vec3( 1.0,  1.0,  1.0), vec3(0.0, 1.0, 0.0),
        vec3( 1.0,  1.0, -1.0), vec3(0.0, 0.0, 1.0),
        vec3(-1.0,  1.0, -1.0), vec3(1.0, 1.0, 1.0),

        vec3(-1.0,  1.0, -1.0), vec3(1.0, 0.0, 1.0),
        vec3( 1.0,  1.0, -1.0), vec3(0.0, 0.5, 0.2),
        vec3( 1.0, -1.0, -1.0), vec3(0.8, 0.6, 0.4),
        vec3(-1.0, -1.0, -1.0), vec3(0.3, 1.0, 0.5),

        vec3( 1.0,  1.0, -1.0), vec3(0.2, 0.5, 0.2),
        vec3( 1.0,  1.0,  1.0), vec3(0.9, 0.3, 0.7),
        vec3( 1.0, -1.0,  1.0), vec3(0.3, 0.7, 0.5),
        vec3( 1.0, -1.0, -1.0), vec3(0.5, 0.7, 0.5),

        vec3(-1.0,  1.0,  1.0), vec3(0.7, 0.8, 0.2),
        vec3(-1.0,  1.0, -1.0), vec3(0.5, 0.7, 0.3),
        vec3(-1.0, -1.0, -1.0), vec3(0.4, 0.7, 0.7),
        vec3(-1.0, -1.0,  1.0), vec3(0.2, 0.5, 1.0),

        vec3( 1.0,  1.0,  1.0), vec3(0.6, 1.0, 0.7),
        vec3(-1.0,  1.0,  1.0), vec3(0.6, 0.4, 0.8),
        vec3(-1.0, -1.0,  1.0), vec3(0.2, 0.8, 0.7),
        vec3( 1.0, -1.0,  1.0), vec3(0.2, 0.7, 1.0),

        vec3( 1.0, -1.0, -1.0), vec3(0.8, 0.3, 0.7),
        vec3(-1.0, -1.0, -1.0), vec3(0.8, 0.9, 0.5),
        vec3(-1.0, -1.0,  1.0), vec3(0.5, 0.8, 0.5),
        vec3( 1.0, -1.0,  1.0), vec3(0.9, 1.0, 0.2),
    };

    short unsigned inds[] = {
         0,  1,  2,  0,  2,  3,     // top
         4,  5,  6,  4,  6,  7,     // front
         8,  9, 10,  8, 10, 11,     // right
         12, 13, 14, 12, 14, 15,    // left
         16, 17, 18, 16, 18, 19,    // back
         20, 22, 21, 20, 23, 22,    // bottom
    };

    cube.num_vertices = sizeof(verts) / sizeof(*verts);
    cube.vertices = (vertex*)malloc(cube.num_vertices * sizeof(vertex));
    memcpy(cube.vertices, verts, sizeof(verts));
    
    cube.num_indices = sizeof(inds) / sizeof(*inds);
    cube.indices = (short unsigned*)malloc(cube.num_indices * sizeof(short unsigned));
    memcpy(cube.indices, inds, sizeof(inds));

    return cube;
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
    glfwMakeContextCurrent(window);
    
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
    int unsigned vertex_buffer_id;
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, shape.num_vertices * sizeof(vertex), shape.vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));

    int unsigned index_buffer_id;
    glGenBuffers(1, &index_buffer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape.num_indices * sizeof(short unsigned), shape.indices, GL_STATIC_DRAW);

    free(shape.vertices);
    free(shape.indices);

    char *vertex_shader_src = read_code("shaders/vertex.glsl");
    char *fragment_shader_src = read_code("shaders/fragment.glsl");

    int unsigned vertex_shader_id = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
    int unsigned fragment_shader_id = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);
    int unsigned program_id = glCreateProgram();
    link_program(program_id, vertex_shader_id, fragment_shader_id);
    glUseProgram(program_id);

    free(vertex_shader_src);
    free(fragment_shader_src);

    float angle_x = 36.0f;
    float angle_y = 0.0f;
    float angle_z = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        // boilerplate code to tell opengl that a new frame is about to begin
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // render
        glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, window_width, window_height);

        int full_transform_mat_uniform_location = glGetUniformLocation(program_id, "full_transform_mat");

        mat4 projection_mat = glm::perspective(glm::radians(80.0f), ((float)window_width / window_height), 0.1f, 10.0f);
        mat4 proj_transform_mat = glm::translate(projection_mat, vec3(-1.0f, 0.0f, -3.0f));
        proj_transform_mat = glm::rotate(proj_transform_mat, glm::radians(angle_x), vec3(1.0f, 0.0f, 0.0f));
        proj_transform_mat = glm::rotate(proj_transform_mat, glm::radians(angle_y), vec3(0.0f, 1.0f, 0.0f));
        proj_transform_mat = glm::rotate(proj_transform_mat, glm::radians(angle_z), vec3(0.0f, 0.0f, 1.0f));

        glUniformMatrix4fv(full_transform_mat_uniform_location, 1, GL_FALSE, &proj_transform_mat[0][0]);
        glDrawElements(GL_TRIANGLES, shape.num_indices, GL_UNSIGNED_SHORT, 0);


        proj_transform_mat = glm::translate(projection_mat, vec3(1.0f, 0.0f, -3.75f));
        proj_transform_mat = glm::rotate(proj_transform_mat, glm::radians(126.0f), vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(full_transform_mat_uniform_location, 1, GL_FALSE, &proj_transform_mat[0][0]);
        glDrawElements(GL_TRIANGLES, shape.num_indices, GL_UNSIGNED_SHORT, 0);



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