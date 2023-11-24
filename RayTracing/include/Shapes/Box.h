#pragma once
#include "Shape.h"

struct Box : public Shape
{
	glm::vec3 Position{ 0.f };
	glm::vec3 BoxSize{ 1.f };

	int MaterialIndex;

	Box(glm::vec3 pos, glm::vec3 boxSize, int mat) : Position(pos), BoxSize(boxSize), MaterialIndex(mat) {};

	glm::vec3 GetPosition() const override { return Position; };
	int GetMaterialIndex() const override { return MaterialIndex; }

public:
	float Hit(const Ray& ray) const override
	{
		glm::vec3 m = 1.0f / ray.Direction;
		glm::vec3 n = m * ray.Origin;
		glm::vec3 k = glm::abs(m) * BoxSize;
		glm::vec3 t1 = -n - k;
		glm::vec3 t2 = -n + k;
		
		float tN = glm::max(glm::max(t1.x, t1.y), t1.z);
		float tF = glm::min(glm::min(t2.x, t2.y), t2.z);
		if (tN > tF || tF < 0.0f) return -1;

		return tF;
	}
	
	void CalculateNormals(HitPayload& payload, const Ray& ray)
	{
		glm::vec3 m = 1.0f / ray.Direction;
		glm::vec3 n = m * ray.Origin;
		glm::vec3 k = glm::abs(m) * BoxSize;
		glm::vec3 t1 = -n - k;

		float tN = glm::max(glm::max(t1.x, t1.y), t1.z);

		payload.WorldPosition = glm::step(glm::vec3(tN),t1);
		payload.WorldNormal = glm::normalize(payload.WorldPosition);

		payload.WorldPosition += Position;
	}
};
