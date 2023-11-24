#pragma once
#include "Rendering/Shape.h"

struct TestShape : public Shape
{
	glm::vec3 Position{ 0.f };
	float Radius = 0.5;

	int MaterialIndex;

	TestShape(glm::vec3 pos, float radius, int mat) : Position(pos), Radius(radius), MaterialIndex(mat) {}
	glm::vec3 GetPosition() const override { return Position; };

	float Hit(const Ray& ray) const override
	{
		return 1.f;
	};

	void CalculateNormals(HitPayload& payload, const Ray& ray) override {};
};

struct TestScene
{
	std::vector<Shape*> Shapes;
	std::vector<Sphere> Spheres;
	std::vector<TestShape> TestShape;
};
