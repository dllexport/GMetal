#version 450

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inAlbedo;
layout (location = 2) in vec3 inPosition;

layout (location = 0) out vec4 outNormal;
layout (location = 1) out vec4 outAlbedo;
layout (location = 2) out vec4 outPosition;

const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 256.0f;

float linearDepth(float depth)
{
	float z = depth * 2.0f - 1.0f; 
	return (2.0f * NEAR_PLANE * FAR_PLANE) / (FAR_PLANE + NEAR_PLANE - z * (FAR_PLANE - NEAR_PLANE));	
}

void main() 
{
	vec3 N = normalize(inNormal);
	N.y = -N.y;
	outNormal = vec4(N, 1.0);

	outAlbedo.rgb = inAlbedo;
	outPosition = vec4(inPosition, 1.0);
}