#pragma once
#include "Walnut/Image.h"
#include <memory>
#include <glm/glm.hpp>
#include "Camera.h"
#include "Ray.h"

class Renderer
{
private:
	std::shared_ptr<Walnut::Image> _FinalImage;
	uint32_t* _ImageData = nullptr;

	glm::vec4 TraceRay(const Ray& ray);
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Camera& camera);

	inline const std::shared_ptr<Walnut::Image> GetFinalImage() { return _FinalImage; }
};
