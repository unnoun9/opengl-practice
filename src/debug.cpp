#include "debug.h"

void gl_clear_error()
{
    while (glGetError() != GL_NO_ERROR);
}

bool gl_log_call()
{
    while (GLenum error = glGetError())
    {
        std::cout << "OpenGL Error 0x" << std::hex << error << " -- ";
        return false;
    }
    return true;
}