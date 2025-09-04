#pragma once

#include <GL/glew.h>
#include <vector>

#include "debug.h"

struct vertex_buffer_element
{
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int get_size_of_type(unsigned int type);
};

struct vertex_buffer_layout
{
    std::vector<vertex_buffer_element> elements; // each element contains info on one attribute of the buffer this layout targets
    unsigned int stride;

    vertex_buffer_layout();

    template <typename T>
    void push(unsigned int count)
    {
        static_assert(false, "Unsupported type for vertex buffer layout");
    }
    
};

template<>
inline void vertex_buffer_layout::push<float>(unsigned int count)
{
    elements.push_back({ GL_FLOAT, count, GL_FALSE });
    stride += count * vertex_buffer_element::get_size_of_type(GL_FLOAT);
}

template<>
inline void vertex_buffer_layout::push<unsigned int>(unsigned int count)
{
    elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
    stride += count * vertex_buffer_element::get_size_of_type(GL_UNSIGNED_INT);
}

template<>
inline void vertex_buffer_layout::push<unsigned char>(unsigned int count)
{
    elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE});
    stride += count * vertex_buffer_element::get_size_of_type(GL_UNSIGNED_BYTE);
}