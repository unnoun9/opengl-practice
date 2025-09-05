#version 430

layout(location=0) in vec3 position;
layout(location=1) in vec3 color;

uniform mat4 model_transform;
uniform mat4 projection;

out vec3 the_color;

void main()
{
   vec4 pos = vec4(position, 1.0);
   vec4 new_pos = model_transform * pos;
   vec4 projected_pos = projection * new_pos;
   gl_Position = projected_pos;
   // gl_Position = projection * (model_transform * vec4(position_attrib, 1.0));
   the_color = color;
}