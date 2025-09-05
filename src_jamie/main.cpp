#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

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
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    
    int compile_status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE)
    {
        int info_log_length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &info_log_length);
        char *buffer = (char*)alloca(info_log_length * sizeof(char));
        glGetShaderInfoLog(id, info_log_length, &info_log_length, buffer);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader := ";
        std::cout << buffer << std::endl;
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
        std::cout << "Shader Linking Error := " << buffer << std::endl;
    }
}

std::string read_code(const char *filename)
{
    std::ifstream file(filename);
    if (!file.good())
    {
        std::cout << "Failed to load file " << filename << std::endl;
        exit(1);
    }

    std::string line;
    std::stringstream string_stream;
    while (getline(file, line))
    {
        string_stream << line << '\n';
    }

    file.close();
    return string_stream.str();
}

struct vertex
{
    glm::vec3 position, color;
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
    tri.vertices = new vertex[tri.num_vertices];
    memcpy(tri.vertices, vertices, sizeof(vertices));
    
    tri.num_indices = sizeof(indices) / sizeof(*indices);
    tri.indices = new short unsigned[tri.num_indices];
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
    cube.vertices = new vertex[cube.num_vertices];
    memcpy(cube.vertices, verts, sizeof(verts));
    
    cube.num_indices = sizeof(inds) / sizeof(*inds);
    cube.indices = new short unsigned[cube.num_indices];
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

    float aspect_ratio = 4.0f / 3.0f;
    window_width = 600;
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

    if (glewInit() != GLEW_OK)
        std::cout << "Error calling glewInit()" << std::endl;
    
    // imgui initialization boiler plate code
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glEnable(GL_DEPTH_TEST);

    shape_data cube = make_cube();
    int unsigned vertex_buffer_id;
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, cube.num_vertices * sizeof(vertex), cube.vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));

    int unsigned index_buffer_id;
    glGenBuffers(1, &index_buffer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.num_indices * sizeof(short unsigned), cube.indices, GL_STATIC_DRAW);

    delete[] cube.vertices;
    delete[] cube.indices;

    std::string vertex_shader_src = read_code("shaders/vertex.glsl");
    std::string fragment_shader_src = read_code("shaders/fragment.glsl");

    int unsigned vertex_shader_id = compile_shader(GL_VERTEX_SHADER, vertex_shader_src.c_str());
    int unsigned fragment_shader_id = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src.c_str());
    int unsigned program_id = glCreateProgram();
    link_program(program_id, vertex_shader_id, fragment_shader_id);
    glUseProgram(program_id);

    while (!glfwWindowShouldClose(window))
    {
        // boilerplate code to tell opengl that a new frame is about to begin
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // render
        glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, window_width, window_height);

        mat4 model_transform = glm::rotate(
            glm::rotate(
                glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, -4.0f)),
                glm::radians(25.0f), vec3(0.0f, 1.0f, 0.0f)
            ),
            glm::radians(25.0f), vec3(1.0f, 0.0f, 0.0f)
        );
        mat4 projection = glm::perspective(glm::radians(60.0f), ((float)window_width / window_height), 0.1f, 10.0f);
        int model_transform_uniform_location = glGetUniformLocation(program_id, "model_transform");
        int projection_uniform_location = glGetUniformLocation(program_id, "projection");
        glUniformMatrix4fv(model_transform_uniform_location, 1, GL_FALSE, &model_transform[0][0]);
        glUniformMatrix4fv(projection_uniform_location, 1, GL_FALSE, &projection[0][0]);
        glDrawElements(GL_TRIANGLES, cube.num_indices, GL_UNSIGNED_SHORT, 0);

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

    glfwTerminate();
    return 0;
}