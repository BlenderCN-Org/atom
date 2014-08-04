#version 410

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in uvec4 bone_index;
layout(location = 2) in vec4 bone_weight;

uniform mat4 bones[256];

struct VSOut {
  vec3 vertex;
  vec4 bw;
};

out VSOut vsout;

void main(void)
{
  uint bi0 = bone_index.x;
  uint bi1 = bone_index.y;
  uint bi2 = bone_index.z;
  uint bi3 = bone_index.w;

  vec4 v = vec4(vertex_position, 1);

  vec3 v0 = (bones[bi0] * v).xyz * bone_weight.x;
  vec3 v1 = (bones[bi1] * v).xyz * bone_weight.y;
    vec3 final = v0 + v1;

  vsout.bw = bone_weight;
  vsout.vertex = final;
}
