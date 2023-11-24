#pragma once
#include <glm/glm.hpp>

struct Ray
{
	glm::vec3 Origin;
	glm::vec3 Direction;
};

struct HitPayload
{
	float HitDistance;
	glm::vec3 WorldNormal;
	glm::vec3 WorldPosition;

	int ObjectIndex;
};
