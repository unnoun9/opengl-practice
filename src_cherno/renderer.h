#pragma once

#include <GL/glew.h>

#include "vertex_array.h"
#include "index_buffer.h"
#include "shader.h"

void clear();
void draw(const vertex_array& va, const index_buffer& ib, const shader& shader);