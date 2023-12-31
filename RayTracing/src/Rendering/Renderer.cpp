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

	static uint32_t PCG_Hash(uint32_t input)
	{
		uint32_t state = input * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	static float RandomFloat(uint32_t& seed)
	{
		seed = PCG_Hash(seed);
		return (float)seed / (float)std::numeric_limits<uint32_t>().max();
	}

	static glm::vec3 InUnitSphere(uint32_t& seed)
	{
		return glm::normalize(glm::vec3(RandomFloat(seed) * 2.f - 1.f,
			RandomFloat(seed) * 2.f - 1.f,
			RandomFloat(seed) * 2.f - 1.f));
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
	uint32_t seed = x + y * _FinalImage->GetWidth();
	seed *= _FrameIndex;

	for (int i = 0; i < _Settings.Bounces; i++)
	{
		seed += i;

		HitPayload payload = TraceRay(ray);
		if (payload.HitDistance < 0)
		{
			light += _Settings.SkyColour * contribution;
			break;
		}

		//Change this so it can render other primitives
		const Shape* shape = _ActiveScene->Shapes.at(payload.ObjectIndex);
		const Material& material = _ActiveScene->Materials.at(shape->GetMaterialIndex());

		//Light calculations go here
		contribution *= material.Albedo;
		light += material.GetEmission();

		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
		ray.Direction = glm::normalize(payload.WorldNormal + Utils::InUnitSphere(seed));
	}

	return glm::vec4(light, 1.0f);
}

HitPayload Renderer::TraceRay(const Ray& ray)
{
	float aspectRatio = (float)_FinalImage->GetWidth() / (float)_FinalImage->GetHeight();

	int closestShape = -1;
	float hitDist = std::numeric_limits<float>().max();

	for(uint32_t i = 0; i < _ActiveScene->Shapes.size(); i++)
	{
		const Shape* shape = _ActiveScene->Shapes.at(i);
		float hitResult = shape->Hit(ray);

		if (hitResult < 0.f)
		{
			continue;
		}

		//(-b +- sqrt(disc)) / 2a
		//float t0 = (-b + glm::sqrt(discriminant)) / (2.f * a);
		if(hitResult > 0.f && hitResult < hitDist)
		{
			hitDist = hitResult;
			closestShape = i;
		}
	}

	if(closestShape < 0)
	{
		return Miss(ray);
	}

	return ClosestHit(ray, hitDist, closestShape);
}

HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
{
	HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;

	const Shape* closestShape = _ActiveScene->Shapes.at(objectIndex);

	//The coordinate in which the ray Hit the shape
	closestShape->CalculateNormals(payload, ray);

	return payload;
}


HitPayload Renderer::Miss(const Ray& ray)
{
	HitPayload payload;
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