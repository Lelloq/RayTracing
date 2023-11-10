#include "Rendering/Renderer.h"
#include "Walnut/Random.h"
#include <iostream>
#include "Globals.h"

namespace Utils
{
	static uint32_t ConvertToRGBA(glm::vec4 colour)
	{
		uint8_t r = (uint8_t)(colour.r * 255.f);
		uint8_t g = (uint8_t)(colour.g * 255.f);
		uint8_t b = (uint8_t)(colour.b * 255.f);
		uint8_t a = (uint8_t)(colour.a * 255.f);

		uint32_t res = (a << 24) | (b << 16) | (g << 8) | r;
		return res;
	}	
}

void Renderer::Render(const Camera& camera)
{
	Ray ray;
	ray.Origin = camera.GetPosition();

	//Every pixel going left to right, up to down
	for (uint32_t y = 0; y < _FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < _FinalImage->GetWidth(); x++)
		{
			//The camera class already makes puts the coordinates into NDC
			ray.Direction = camera.GetRayDirections()[x + y * _FinalImage->GetWidth()];

			glm::vec4 col = TraceRay(ray);
			col = glm::clamp(col, glm::vec4(0.f), glm::vec4(1.f));
			_ImageData[x + y * _FinalImage->GetWidth()] = Utils::ConvertToRGBA(col);
		}
	}
	_FinalImage->SetData(_ImageData);
}

glm::vec4 Renderer::TraceRay(const Ray& ray)
{
	float aspectRatio = (float)_FinalImage->GetWidth() / (float)_FinalImage->GetHeight();
	float radius = 0.5f;
	//rayDirection = glm::normalize(rayDirection);

	//Solving for t
	//(bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ay^2 + ax^2 - r^2) = 0
	//a = ray origin
	//b = ray direction
	//r = radius
	//t = hit distance

	float a = glm::dot(ray.Direction, ray.Direction);
	float b = 2.f * glm::dot(ray.Origin, ray.Direction);
	float c = glm::dot(ray.Origin, ray.Origin) - radius * radius;

	float discriminant = b * b - 4.f * a * c;
	if (discriminant < 0.f)
	{
		return glm::vec4(0.f,0.f,0.f,1.f);
	}

	//(-b +- sqrt(disc)) / 2a
	float t0 = (-b + glm::sqrt(discriminant)) / (2.f * a);
	float closestT = (-b - glm::sqrt(discriminant)) / (2.f * a);

	//The coordinate in which the ray hit the sphere
	glm::vec3 hitpoint = ray.Origin + ray.Direction * closestT;
	glm::vec3 normal = glm::normalize(hitpoint);

	glm::vec3 lightDirection = glm::normalize(glm::vec3((-1.f, -1.f, -1.f)));

	float d = glm::max(glm::dot(normal, -lightDirection), 0.f);// cos(angle)

	glm::vec3 sphereColour(gSphereColour.x, gSphereColour.y, gSphereColour.z);
	sphereColour *= d + glm::vec3(0.2f);
	return glm::vec4(sphereColour ,gSphereColour.w);
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (_FinalImage)
	{
		//If there is nothing needed to be resized
		if (_FinalImage->GetWidth() == width && _FinalImage->GetHeight() == height)
			return;

		_FinalImage->Resize(width, height);
	}
	else
	{
		_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] _ImageData;
	_ImageData = new uint32_t[width * height];
}