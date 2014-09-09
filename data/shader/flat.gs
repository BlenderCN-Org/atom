#version 410

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 mvp;
uniform mat4 model;

out vec3 vertex;
out vec3 normal;

void main()
{
  vec3 a = gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz;
  vec3 b = gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz;
  vec3 n = (model * vec4(normalize(cross(a, b)), 0)).xyz;
  gl_Position = mvp * gl_in[0].gl_Position;
  normal = n;
  EmitVertex();
  gl_Position = mvp * gl_in[1].gl_Position;
  normal = n;
  EmitVertex();
  gl_Position = mvp * gl_in[2].gl_Position;
  normal = n;
  EmitVertex();
}
