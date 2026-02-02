#pragma once

#include <glad/glad.h>

#include "debug.h"

struct index_buffer
{
    unsigned int renderer_id;
    unsigned int count;

    index_buffer(const unsigned int* data, unsigned int count);
    ~index_buffer();
    void bind() const;
    void unbind() const;
};