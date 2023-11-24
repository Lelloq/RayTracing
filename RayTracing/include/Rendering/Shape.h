#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Ray.h"

class Shape
{
public:
	virtual float Hit(const Ray& ray) const = 0;
	virtual glm::vec3 GetPosition() const = 0;
	virtual int GetMaterialIndex() const = 0;
	virtual void CalculateNormals(HitPayload& payload, const Ray& ray) = 0;

	template<typename T>
	static void AddShapes(std::vector<Shape*>& shapeContainer, std::vector<T>& shapes)
	{
		for (Shape& s : shapes)
		{
			shapeContainer.push_back(&s);
		}
	}
};

struct Sphere : public Shape
{
	//It's a struct since I can edit the values directly through ImGui or by other means
	glm::vec3 Position{ 0.f };
	float Radius = 0.5;

	int MaterialIndex;

	Sphere(glm::vec3 pos, float radius, int mat) : Position(pos), Radius(radius), MaterialIndex(mat) {}

	//These are only implemented so that the base class can retrieve those values
	glm::vec3 GetPosition() const override { return Position; };
	int GetMaterialIndex() const override { return MaterialIndex; }

	float Hit(const Ray& ray) const override
	{
		glm::vec3 origin = ray.Origin - Position;

		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.f * glm::dot(origin, ray.Direction);
		float c = glm::dot(origin, origin) - Radius * Radius;

		float discriminant = b * b - 4.f * a * c;
		if (discriminant < 0.f)
		{
			return -1;
		}

		//(-b +- sqrt(disc)) / 2a
		//float t0 = (-b + glm::sqrt(discriminant)) / (2.f * a);
		float closestT = (-b - glm::sqrt(discriminant)) / (2.f * a);

		return closestT;
	}

	void CalculateNormals(HitPayload& payload, const Ray& ray) override
	{
		glm::vec3 origin = ray.Origin - Position;
		//The coordinate in which the ray Hit the sphere
		payload.WorldPosition = origin + ray.Direction * payload.HitDistance;
		payload.WorldNormal = glm::normalize(payload.WorldPosition);

		payload.WorldPosition += Position;
	}
};
