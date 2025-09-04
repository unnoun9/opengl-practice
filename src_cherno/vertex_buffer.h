#pragma once

#include <GL/glew.h>

#include "debug.h"

struct vertex_buffer
{
    unsigned int renderer_id;

    vertex_buffer(const void* data, unsigned int size);
    ~vertex_buffer();
    void bind() const;
    void unbind() const;
};