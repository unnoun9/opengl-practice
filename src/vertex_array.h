#pragma once

#include "vertex_buffer.h"
#include "vertex_buffer_layout.h"

struct vertex_array
{
    vertex_array()
    {
        CALL(glGenVertexArrays(1, &renderer_id));
    }

    ~vertex_array()
    {
        CALL(glDeleteVertexArrays(1, &renderer_id));
    }

    void add_buffer(const vertex_buffer& vb, const vertex_buffer_layout& layout)
    {
        bind();
        vb.bind();
        unsigned int offset = 0;
        for (unsigned int i = 0; i < layout.elements.size(); i++)
        {
            const auto& element = layout.elements[i];
            CALL(glEnableVertexAttribArray(i));
            CALL(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.stride, (void*)(uintptr_t)offset));
            offset += element.count * vertex_buffer_element::get_size_of_type(element.type);
        }
    }

    void bind() const
    {
        CALL(glBindVertexArray(renderer_id));
    }
    
    void unbind() const
    {
        CALL(glBindVertexArray(0));
    }

private:
    unsigned int renderer_id;
};