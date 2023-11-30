#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Rendering/Ray.h"

class Shape
{
public:
	virtual float Hit(const Ray& ray) const = 0;
	virtual glm::vec3 GetPosition() const = 0;
	virtual int GetMaterialIndex() const = 0;
	virtual void CalculateNormals(HitPayload& payload, const Ray& ray) const = 0;

	template<typename T>
	static void AddShapes(std::vector<Shape*>& shapeContainer, std::vector<T>& shapes)
	{
		for (Shape& s : shapes)
		{
			shapeContainer.push_back(&s);
		}
	}
};
