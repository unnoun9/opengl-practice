#pragma once

#include <GL/glew.h>
#include <imgui.h>

#include "tests.h"
#include "debug.h"

namespace test {

struct clear_color
{
    name name;
    float color[4];

    clear_color(float r=0.2f, float g=0.3f, float b=0.8f, float a=1.0f)
        : name(CLEAR_COLOR), color {r, g, b, a}
    {
    }

    void on_render()
    {
        CALL(glClearColor(color[0], color[1], color[2], color[3]));
        CALL(glClear(GL_COLOR_BUFFER_BIT));
    }

    void on_imgui_render()
    {
        ImGui::ColorEdit4("Clear Color", color);
    }
};

}