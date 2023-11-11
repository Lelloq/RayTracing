#pragma once
#include <vector>
#include <glm/glm.hpp>

struct Sphere
{
	glm::vec3 Position{0.f};
	float Radius = 0.5;

	glm::vec4 Albedo{1.f};
};

struct Scene
{
	std::vector<Sphere> Spheres;
};
