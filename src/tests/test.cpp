#include "test.h"
#include "clear_color.h"
#include "texture_2d.h"

const char* test::name_to_str(name test_name)
{
    int index = (int)test_name;
    if (index < 0 || index > ((int)NUM_TEST_NAMES - 1))
        std::cout << "`test_name` passed to `name_to_str` is out of range: " << index << std::endl;
    static const char* name_str[NUM_TEST_NAMES] = {"Clear Color", "2D Texture"};
    return name_str[index];
}

void test::destroy(void* current_test)
{
    if (!current_test)
    {
        return;
    }

    // cast it to any arbitrary test to *only* access the name member
    name name = ((clear_color*)current_test)->name;

    // find out the type of the test and delete accordingly
    switch (name)
    {
        case CLEAR_COLOR: delete (clear_color*)current_test; return;
        case TEXTURE_2D: delete (texture_2d*)current_test; return;
        default: std::cout << "Unknown test type/name passed to `delete_name`" << std::endl;
    }
}