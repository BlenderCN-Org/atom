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
  float sum = bone_weight.x + bone_weight.y + bone_weight.y + bone_weight.z;
  float w = 1.0 - sum;

  vec3 v0 = (bones[bi0] * v).xyz * bone_weight.x;
  vec3 v1 = (bones[bi1] * v).xyz * bone_weight.y;
  vec3 v2 = (bones[bi2] * v).xyz * bone_weight.z;
  vec3 v3 = (bones[bi3] * v).xyz * bone_weight.w;
  vec3 vf = (mat4(1.0) * v).xyz * w;
  
  vec3 final = v0 + v1 + v2 + v3;

  vsout.bw = bone_weight;
  vsout.vertex = final;
}
