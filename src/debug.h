#pragma once

#include <GL/glew.h>

#include <iostream>
#include <cassert>

#define ASSERT(x) if (!(x)) exit(-1);
#define CALL(x) \
    gl_clear_error();\
    x;\
    assert(gl_log_call() && #x);

static void gl_clear_error()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool gl_log_call()
{
    while (GLenum error = glGetError())
    {
        std::cout << "OpenGL Error 0x" << std::hex << error << " -- ";
        return false;
    }
    return true;
}