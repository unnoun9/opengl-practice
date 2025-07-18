#include "vertex_buffer_layout.h"

unsigned int vertex_buffer_element::get_size_of_type(unsigned int type)
{
    switch (type)
    {
    case GL_FLOAT: return 4;
    case GL_UNSIGNED_INT: return 4;
    case GL_UNSIGNED_BYTE: return 1;
    }
    ASSERT(false && "Unknown type in vertex buffer element `get_size_of_type`");
    return 0;
}

vertex_buffer_layout::vertex_buffer_layout()
    : stride(0)
{
}