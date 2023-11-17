#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Shape.h"

struct Material
{
	glm::vec3 Albedo{ 1.0f };
	float Roughness = 1.f;
	float Metallic = 0.f;
	glm::vec3 EmissionColour{ 0.f };
	float EmissionPower = 0.f;

	inline glm::vec3 GetEmission() const { return EmissionColour * EmissionPower; }
};

struct Scene
{
	std::vector<Shape*> Shapes;//For hitting only
	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;
};
