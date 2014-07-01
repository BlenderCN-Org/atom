#version 410

uniform vec3 sun_pos;
uniform mat4 mvp;
uniform mat4 model;

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec4 normal;

out vec4 n;

void main(void)
{
  gl_Position = mvp * vertex_position;
  n = model * vec4(normal.xyz, 0);
}
