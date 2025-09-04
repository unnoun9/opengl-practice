#include "vertex_array.h"

vertex_array::vertex_array()
{
    CALL(glGenVertexArrays(1, &renderer_id));
}

vertex_array::~vertex_array()
{
    CALL(glDeleteVertexArrays(1, &renderer_id));
}

void vertex_array::add_buffer(const vertex_buffer& vb, const vertex_buffer_layout& layout)
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

void vertex_array::bind() const
{
    CALL(glBindVertexArray(renderer_id));
}

void vertex_array::unbind() const
{
    CALL(glBindVertexArray(0));
}