#version 410

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 mvp;
uniform mat4 model;

struct VSOut {
  vec3 vertex;
  vec4 bw;
};

in VSOut vsout[];

struct GSOut {
  vec3 vertex;
  vec3 normal;
  vec4 bonew;
};

out GSOut gsout;

void main()
{
  vec3 a = vsout[0].vertex - vsout[1].vertex;
  vec3 b = vsout[2].vertex - vsout[1].vertex;
  vec3 n = (model * vec4(normalize(cross(a, b)), 0)).xyz;
  gl_Position = mvp * vec4(vsout[0].vertex, 1);
  gsout.normal = n;
  gsout.bonew = vsout[0].bw;
  EmitVertex();
  gl_Position = mvp * vec4(vsout[1].vertex, 1);
  gsout.normal = n;
  gsout.bonew = vsout[0].bw;
  EmitVertex();
  gl_Position = mvp * vec4(vsout[2].vertex, 1);
  gsout.normal = n;
  gsout.bonew = vsout[0].bw;
  EmitVertex();
}
