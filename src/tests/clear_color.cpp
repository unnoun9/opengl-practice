#include "clear_color.h"

test::clear_color::clear_color(float r, float g, float b, float a)
    : name(CLEAR_COLOR), color {r, g, b, a}
{
}

void test::clear_color::on_render()
{
    CALL(glClearColor(color[0], color[1], color[2], color[3]));
    CALL(glClear(GL_COLOR_BUFFER_BIT));
}

void test::clear_color::on_imgui_render()
{
    ImGui::ColorEdit4("Clear Color", color);
}