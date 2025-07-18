#pragma once

#include <GL/glew.h>

#include <iostream>
#include <cassert>

#define ASSERT(x) if (!(x)) exit(-1);
#define CALL(x) \
    gl_clear_error();\
    x;\
    assert(gl_log_call() && #x);

void gl_clear_error();

bool gl_log_call();