#pragma once
#include <vector>
#include <glm/glm.hpp>

struct Material
{
	glm::vec4 Albedo{ 1.0f };
	float Roughness = 1.f;
	float Metallic = 0.f;
};

struct Sphere
{
	glm::vec3 Position{0.f};
	float Radius = 0.5;

	int MaterialIndex;
};

struct Scene
{
	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;
};
