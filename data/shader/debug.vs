#version 410

uniform mat4 mvp;

layout(location = 0) in vec4 vertex_position;
layout(location = 2) in vec3 vertex_color;

out vec3 color;

void main(void)
{
  color = vertex_color;
  gl_Position = mvp * vertex_position;
}
