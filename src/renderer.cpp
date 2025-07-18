#include "renderer.h"

void clear()
{
    CALL(glClear(GL_COLOR_BUFFER_BIT));
}

void draw(const vertex_array& va, const index_buffer& ib, const shader& shader)
{
    shader.bind();
    va.bind();
    ib.bind();

    // issue draw call for to draw the index buffer we specified; the selected/bound buffer is drawn; here we tell gpu what primitive to draw (triangle in this case), number of indices to draw, type of indices, and the a pointer to the indices which isn't required here since we have bound it already above
    CALL(glDrawElements(GL_TRIANGLES, ib.count, GL_UNSIGNED_INT, nullptr));  // by contrast, glDrawArrays draws the vertices specified in the vertex buffer sequentially, while glDrawElements draws vertices in the order specified by the index buffer
}