#version 410

uniform mat4 model_view_projection;

layout(location = 0) in vec4 vertex_position;

void main(void)
{
  gl_Position = model_view_projection * vertex_position;
}
