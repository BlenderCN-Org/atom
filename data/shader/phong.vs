#version 410

layout(location = 0) in vec3 vertex_position;

void main(void)
{
  gl_Position = vec4(vertex_position, 1);
//  gl_Position = mvp * vec4(vertex_position, 1);
}
