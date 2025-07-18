#pragma once

#include <string>

namespace test {

enum name
{
    CLEAR_COLOR, TEXTURE_2D,
    NUM_TEST_NAMES
};

const char* name_to_str(name test_name);
void destroy(void* current_test);
}