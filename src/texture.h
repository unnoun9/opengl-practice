#pragma once

#include <GL/glew.h>
#include <stb_image.h>
#include <string>

#include "debug.h"

struct texture
{
    unsigned int renderer_id;
    unsigned char* local_buffer;
    int width, height, bytes_per_pixel;
    std::string filepath;

    texture(const std::string& path);
    ~texture();
    void bind(unsigned int slot = 0) const;
    void unbind() const;
};