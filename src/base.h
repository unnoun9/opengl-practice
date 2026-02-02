#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace glm;

struct vertex
{
    vec3 position, color;
};

struct shape_data
{
    vertex *vertices;
    unsigned int num_vertices;
    short unsigned *indices;
    unsigned int num_indices;
};

struct camera
{
    vec3 position;
    vec3 direction;
    vec3 up;
    vec3 right;
    vec2 old_mouse_pos;
};

struct input
{
    vec2 mouse_pos;
    bool cursor_captured;
    bool esc_pressed;
    bool w_pressed;
    bool s_pressed;
    bool a_pressed;
    bool d_pressed;
    bool ctrl_pressed;
    bool space_pressed;
};

void camera_update_wrt_input(camera *cam, input *inp)
{
    // mouse updates
    float sensitivity = 0.5f;
    float threshold_to_prevent_jumping = 50.0f;
    vec2 delta = (inp->mouse_pos - cam->old_mouse_pos) * sensitivity;

    cam->right = normalize(cross(cam->direction, cam->up));
    mat4 rotationx = rotate(mat4(1.0f), radians(-delta.x), cam->up);
    mat4 rotationy = rotate(mat4(1.0f), radians(-delta.y), cam->right);
    vec3 new_direction = mat3(rotationx*rotationy) * cam->direction;
    
    // prevent gimbal lock (don't look straight up or down)
    float angle_with_up = degrees(acos(dot(normalize(new_direction), cam->up)));
    if (angle_with_up > 10.0f && angle_with_up < 170.0f)
        cam->direction = new_direction;
    
    cam->old_mouse_pos = inp->mouse_pos;

    /*  better camera mouse movement i suppose:
        cam->yaw += -delta.x;
        cam->pitch += -delta.y;
        
        // prevent gimbal lock but still allow yaw rotation
        cam->pitch = clamp(cam->pitch, -89.0f, 89.0f);
        
        vec3 new_direction;
        new_direction.x = cos(radians(cam->yaw)) * cos(radians(cam->pitch));
        new_direction.y = sin(radians(cam->pitch));
        new_direction.z = sin(radians(cam->yaw)) * cos(radians(cam->pitch));
        cam->direction = normalize(new_direction);
        
        cam->right = normalize(cross(cam->direction, cam->up));
        cam->old_mouse_pos = inp->mouse_pos;
    */

    // keyboard updates - forward/backward, left/right, up/down
    float movement_speed = 0.1f;
    if (inp->w_pressed)
        cam->position +=  movement_speed * cam->direction;
    if (inp->s_pressed)
        cam->position += -movement_speed * cam->direction;
    if (inp->a_pressed)
        cam->position += -movement_speed * cam->right;
    if (inp->d_pressed)
        cam->position +=  movement_speed * cam->right;
    if (inp->ctrl_pressed)
        cam->position += -movement_speed * cam->up;
    if (inp->space_pressed)
        cam->position +=  movement_speed * cam->up;
}