#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inNormal;

layout (binding = 0) uniform UBO 
{
	mat4 projection;
	mat4 model;
	mat4 view;
} ubo;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outAlbedo;
layout (location = 2) out vec3 outPosition;

void main() 
{
	outPosition = vec3(ubo.model * vec4(inPos, 1.0f));
	outAlbedo = inColor;
	outNormal = transpose(inverse(mat3(ubo.model))) * normalize(inNormal);
}
