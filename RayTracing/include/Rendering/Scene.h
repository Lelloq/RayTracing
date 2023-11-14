#pragma once
#include <vector>
#include <glm/glm.hpp>

struct Material
{
	glm::vec3 Albedo{ 1.0f };
	float Roughness = 1.f;
	float Metallic = 0.f;
	glm::vec3 EmissionColour{ 0.f };
	float EmissionPower = 0.f;

	inline glm::vec3 GetEmission() const { return EmissionColour * EmissionPower; }
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
