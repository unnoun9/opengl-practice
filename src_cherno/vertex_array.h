#pragma once

#include "vertex_buffer.h"
#include "vertex_buffer_layout.h"

struct vertex_array
{
    unsigned int renderer_id;

    vertex_array();
    ~vertex_array();
    void add_buffer(const vertex_buffer& vb, const vertex_buffer_layout& layout);
    void bind() const;
    void unbind() const;
};