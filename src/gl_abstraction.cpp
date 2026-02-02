#include "base.h"

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