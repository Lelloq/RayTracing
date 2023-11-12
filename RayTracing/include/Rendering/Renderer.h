#pragma once
#include "Walnut/Image.h"
#include <memory>
#include <glm/glm.hpp>

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

class Renderer
{
public:
	struct Settings
	{
		bool Accumulate = true;
	};
private:
	std::shared_ptr<Walnut::Image> _FinalImage;
	uint32_t* _ImageData = nullptr;
	glm::vec4* _AccumulatedData = nullptr;
	Settings _Settings;

	uint32_t _FrameIndex = 1;

	const Scene* _ActiveScene = nullptr;
	const Camera* _ActiveCamera = nullptr;

	struct HitPayload
	{
		float HitDistance;
		glm::vec3 WorldNormal;
		glm::vec3 WorldPosition;

		int ObjectIndex;
	};

	//This is where the light bounce calculations are done and the colour of the pixel is determined
	glm::vec4 PerPixel(uint32_t x, uint32_t y);

	//Firing a ray on and seeing if it hits an object
	HitPayload TraceRay(const Ray& ray);

	//The closest hit object returns the data of where it has been hit and its normal
	HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex);

	//Returns a payload of -1 indicating the ray did not hit anything
	HitPayload Miss(const Ray& ray);
public:
	Renderer() = default;

	//Executes changes the image size whenever the viewport size has changed
	void OnResize(uint32_t width, uint32_t height);

	//Renders the scene
	void Render(const Scene& scene,const Camera& camera);

	//Resets the frame index for the accumulated frames
	inline void ResetFrameIndex() { _FrameIndex = 1; }

	//Renderer settings
	inline Settings& GetSettings() { return _Settings; }

	inline const std::shared_ptr<Walnut::Image> GetFinalImage() { return _FinalImage; }
};
