#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

int window_width, window_height;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE); // other memory deallocations?
}

void window_resize_callback(GLFWwindow* window, int width, int height)
{
    window_width = width;
    window_height = height;
}

unsigned int compile_shader(int unsigned type, const char* src)
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
        char* buffer = (char*)alloca(info_log_length * sizeof(char));
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
        char* buffer = (char*)alloca(info_log_length);
        glGetProgramInfoLog(program_id, info_log_length, &info_log_length, buffer);
        std::cout << "Shader Linking Error := " << buffer << std::endl;
    }
}

std::string read_code(const char* filename)
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

int main(void)
{
    GLFWwindow* window;

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

    float x_delta = 0.1f;
    int num_tris = 0;
    int num_verts_per_tri = 3;
    int num_floats_per_vert = 6;
    int tri_byte_size = num_verts_per_tri * num_floats_per_vert * sizeof(float);
    int max_tris = 20;

    int unsigned vertex_buffer_id;
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, max_tris * tri_byte_size, 0, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
    // note: `stride` is number of bytes from the beginning of an attribute of one vertex, to the beginning of the attribute of the next vertex
    // but stride being zero is a bit different, it just means attributes are tightly packed or contiguous
    // while `pointer` is the number of bytes from the beginning of a vertex to where the attribute's data starts
    // enabling means sending the attrubute to the graphics processing pipeline
    // also opengl assumes that the 0th attribute is the position by default

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
        
        // send another triangle to opengl
        if (num_tris < max_tris)
        {
            float this_tri_x = -1.0 + num_tris * x_delta;
            float this_tri[] = {
                this_tri_x, 1.0, 0.0,               1.0, 0.0, 0.0,
                this_tri_x + x_delta, 1.0, 0.0,     1.0, 0.0, 0.0,
                this_tri_x, 0.0, 0.0,               1.0, 0.0, 0.0,
            };
            glBufferSubData(GL_ARRAY_BUFFER, num_tris * tri_byte_size, tri_byte_size, this_tri); // instead of tri_byte_suze, probably sizeof(this_tri) woul work
            ++num_tris;
        }

        glDrawArrays(GL_TRIANGLES, (num_tris - 1) * num_verts_per_tri, num_verts_per_tri);

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