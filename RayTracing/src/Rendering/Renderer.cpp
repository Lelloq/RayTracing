#include "Rendering/Renderer.h"
#include "Walnut/Random.h"
#include <iostream>
#include <numeric>
#include <execution>

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

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	_ActiveScene = &scene;
	_ActiveCamera = &camera;

	if(_FrameIndex == 1)
	{
		memset(_AccumulatedData, 0, (size_t)(_FinalImage->GetWidth() * _FinalImage->GetHeight() * sizeof(glm::vec4)));
	}

#define MT 1
#if MT
	std::for_each(std::execution::par, _ImageVerticalIter.begin(), _ImageVerticalIter.end(), 
		[this](uint32_t y) 
		{
			std::for_each(std::execution::par ,_ImageHorizontalIter.begin(), _ImageHorizontalIter.end(),
			[this, y](uint32_t x)
				{
					glm::vec4 col = PerPixel(x, y);
					_AccumulatedData[x + y * _FinalImage->GetWidth()] += col;

					glm::vec4 accumlatedColour = _AccumulatedData[x + y * _FinalImage->GetWidth()];
					accumlatedColour /= (float)_FrameIndex;//Avaraging out so there isn't unexpected bright colours

					accumlatedColour = glm::clamp(accumlatedColour, glm::vec4(0.f), glm::vec4(1.f));
					_ImageData[x + y * _FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumlatedColour);
				});
		});
#else
	for (uint32_t y = 0; y < _FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < _FinalImage->GetWidth(); x++)
		{
			glm::vec4 col = PerPixel(x, y);
			_AccumulatedData[x + y * _FinalImage->GetWidth()] += col;

			glm::vec4 accumlatedColour = _AccumulatedData[x + y * _FinalImage->GetWidth()];
			accumlatedColour /= (float)_FrameIndex;//Avaraging out so there isn't unexpected bright colours

			accumlatedColour = glm::clamp(accumlatedColour, glm::vec4(0.f), glm::vec4(1.f));
			_ImageData[x + y * _FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumlatedColour);
		}
	}
#endif
	_FinalImage->SetData(_ImageData);

	if(_Settings.Accumulate)
	{
		_FrameIndex++;
	}
	else
	{
		_FrameIndex = 1;
	}
}


glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = _ActiveCamera->GetPosition();
	ray.Direction = _ActiveCamera->GetRayDirections()[x + y * _FinalImage->GetWidth()];

	glm::vec3 light(0.f);
	glm::vec3 contribution(1.f);

	for (int i = 0; i < _Settings.Bounces; i++)
	{
		Renderer::HitPayload payload = TraceRay(ray);
		if (payload.HitDistance < 0)
		{
			light += _Settings.SkyColour * contribution;
			break;
		}

		const Sphere& sphere = _ActiveScene->Spheres.at(payload.ObjectIndex);
		const Material& material = _ActiveScene->Materials.at(sphere.MaterialIndex);

		contribution *= material.Albedo;
		light += material.GetEmission();

		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
		ray.Direction = glm::normalize(payload.WorldNormal + Walnut::Random::InUnitSphere());
	}

	return glm::vec4(light, 1.0f);
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
	float aspectRatio = (float)_FinalImage->GetWidth() / (float)_FinalImage->GetHeight();

	//rayDirection = glm::normalize(rayDirection);

	//Solving for t
	//(bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ay^2 + ax^2 - r^2) = 0
	//a = ray origin
	//b = ray direction
	//r = radius
	//t = hit distance

	int closestSphere = -1;
	float hitDist = std::numeric_limits<float>().max();

	for(uint32_t i = 0; i < _ActiveScene->Spheres.size(); i++)
	{
		const Sphere& sphere = _ActiveScene->Spheres[i];
		glm::vec3 origin = ray.Origin - sphere.Position;

		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.f * glm::dot(origin, ray.Direction);
		float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

		float discriminant = b * b - 4.f * a * c;
		if (discriminant < 0.f)
		{
			continue;
		}

		//(-b +- sqrt(disc)) / 2a
		//float t0 = (-b + glm::sqrt(discriminant)) / (2.f * a);
		float closestT = (-b - glm::sqrt(discriminant)) / (2.f * a);
		if(closestT > 0.f && closestT < hitDist)
		{
			hitDist = closestT;
			closestSphere = i;
		}
	}

	if(closestSphere < 0)
	{
		return Miss(ray);
	}

	return ClosestHit(ray, hitDist, closestSphere);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
{
	Renderer::HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;

	const Sphere& closestSphere = _ActiveScene->Spheres.at(objectIndex);

	glm::vec3 origin = ray.Origin - closestSphere.Position;
	//The coordinate in which the ray hit the sphere
	payload.WorldPosition = origin + ray.Direction * hitDistance;
	payload.WorldNormal = glm::normalize(payload.WorldPosition);

	payload.WorldPosition += closestSphere.Position;

	return payload;
}


Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
	Renderer::HitPayload payload;
	payload.HitDistance = -1.f;
	return payload;
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
	
	delete[] _AccumulatedData;
	_AccumulatedData = new glm::vec4[width * height];

	_ImageHorizontalIter.resize(width);
	_ImageVerticalIter.resize(height);

	std::iota(_ImageHorizontalIter.begin(), _ImageHorizontalIter.end(), 0);
	std::iota(_ImageVerticalIter.begin(), _ImageVerticalIter.end(), 0);
}