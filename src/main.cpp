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

using namespace glm;

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

unsigned int compile_shader(unsigned int type, const char *src)
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

void link_program(unsigned int program_id, unsigned int vertex_shader_id, unsigned int fragment_shader_id)
{
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    // can do glBindAttribLocation here, must be before linking coz linker resolves attrib locations
    glLinkProgram(program_id);
    // after linking can query location of an attrib via glGetAttribLocation

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
    unsigned int num_vertices;
    short unsigned *indices;
    unsigned int num_indices;
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

    mat4 full_transforms[2];
    mat4 projection_mat = perspective(radians(80.0f), ((float)window_width / window_height), 0.1f, 10.0f);
    full_transforms[0] = translate(projection_mat, vec3(-1.0f, 0.0f, -3.0f));
    full_transforms[0] = rotate(full_transforms[0], radians(angle_x), vec3(1.0f, 0.0f, 0.0f));
    full_transforms[0] = rotate(full_transforms[0], radians(angle_y), vec3(0.0f, 1.0f, 0.0f));
    full_transforms[0] = rotate(full_transforms[0], radians(angle_z), vec3(0.0f, 0.0f, 1.0f));

    full_transforms[1] = translate(projection_mat, vec3(1.0f, 0.0f, -3.75f));
    full_transforms[1] = rotate(full_transforms[1], radians(126.0f), vec3(0.0f, 1.0f, 0.0f));
    full_transforms[1] = full_transforms[1];

    unsigned int transformation_mat_buffer_id;
    glGenBuffers(1, &transformation_mat_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, transformation_mat_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(full_transforms), full_transforms, GL_STATIC_DRAW);
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
        // boilerplate code to tell opengl that a new frame is about to begin
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // render
        glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, window_width, window_height);
        glDrawElementsInstanced(GL_TRIANGLES, shape.num_indices, GL_UNSIGNED_SHORT, 0, 2);

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