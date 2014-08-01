#version 410

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in uint bone_index;
layout(location = 2) in vec4 bone_weight;

uniform mat4 bones[256];

out vec4 bw;

void main(void)
{
  uint bi0 = (int(bone_index)      ) & 0xFF;
  uint bi1 = (int(bone_index) >>  8) & 0xFF;
  uint bi2 = (int(bone_index) >> 16) & 0xFF;
  uint bi3 = (int(bone_index) >> 24) & 0xFF;

  mat4 transform = (bones[bi0] * bone_weight[0])
                 + (bones[bi1] * bone_weight[1])
                 + (bones[bi2] * bone_weight[2])
                 + (bones[bi3] * bone_weight[3]);

//  transform = bones[bi0] * bone_weight[0];


//  transform = mat4(1.0);
//  transform = (bones[0] * 0) * mat4(1.0);

  gl_Position = transform * vec4(vertex_position, 1);
//  gl_Position = mvp * vec4(vertex_position, 1);
}
