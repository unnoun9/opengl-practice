#pragma once

#include <GL/glew.h>

#include "debug.h"

struct vertex_buffer
{
    vertex_buffer(const void* data, unsigned int size)
    {
        CALL(glGenBuffers(1, &renderer_id));                                // create a "buffer" (just bunch of memory data) and get it's id back
        CALL(glBindBuffer(GL_ARRAY_BUFFER, renderer_id));                   // use the id of a "buffer" to select it to work on it; GL_ARRAY_BUFFER just tells opengl to assign this data to the "array buffer" slot of the state
        CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));    // give the details of the buffer; the size in bytes, the data itself, and how it'll be used
    }

    ~vertex_buffer()
    {
        CALL(glDeleteBuffers(1, &renderer_id));
    }

    void bind() const
    {
        CALL(glBindBuffer(GL_ARRAY_BUFFER, renderer_id));
    }

    void unbind() const
    {
        CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

private:
    unsigned int renderer_id;
};