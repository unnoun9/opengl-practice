#pragma once

#include <GL/glew.h>

#include "debug.h"

struct index_buffer
{
    unsigned int count;

    index_buffer(const unsigned int* data, unsigned int count)
        : count(count)
    {
        ASSERT(sizeof(unsigned int) == sizeof(GLuint));
        CALL(glGenBuffers(1, &renderer_id));                                                                // create a "buffer" for index buffer and get it's id back
        CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id));                                           // use the id of a "buffer" to select it to work on it; GL_ELEMENT_ARRAY_BUFFER just tells opengl to assign this data to the "element array buffer" slot of the state
        CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));    // give the details of the buffer; the size in bytes, the data itself, and how it'll be used
    }

    ~index_buffer()
    {
        CALL(glDeleteBuffers(1, &renderer_id));
    }

    void bind() const
    {
        CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id));
    }

    void unbind() const
    {
        CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

private:
    unsigned int renderer_id;
};