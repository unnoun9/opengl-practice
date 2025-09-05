#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 color;

uniform vec3 dominating_color;
uniform float y_flip;

out vec3 the_color;

void main()
{
   gl_Position = vec4(position, 1.0);
   gl_Position.y = gl_Position.y * y_flip;
   the_color = dominating_color;
}