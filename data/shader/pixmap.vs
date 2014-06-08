#version 430

uniform mat4 model_view_projection;

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec2 vertex_texcoord;

smooth out vec2 uv;


void main(void)
{
  uv = vertex_texcoord;
  gl_Position = model_view_projection * vertex_position;
}
