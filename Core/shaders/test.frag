#version 450

layout (location = 0) in vec3 inColor;

layout (location = 0) out vec4 outFragColor;
layout (location = 1) out vec4 out1;
layout (location = 2) out vec4 out2;

void main() 
{
  outFragColor = vec4(inColor, 1.0);
  out1 = vec4(inColor, 1.0);
  out2 = vec4(inColor, 1.0);
}