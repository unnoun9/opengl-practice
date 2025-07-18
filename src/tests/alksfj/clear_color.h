#pragma once

#include <GL/glew.h>
#include <imgui.h>

#include "test.h"
#include "debug.h"

namespace test {

struct clear_color
{
    name name;
    float color[4];

    clear_color(float r=0.2f, float g=0.3f, float b=0.8f, float a=1.0f);
    void on_render();
    void on_imgui_render();
};

}