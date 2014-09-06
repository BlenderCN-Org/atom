#version 410

uniform mat4 mvp;

layout(location = 0) in vec4 vertex_position;

void main(void)
{
  gl_Position = mvp * vertex_position;
}
