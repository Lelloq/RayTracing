#include "Rendering/Renderer.h"
#include "Walnut/Random.h"
#include <iostream>

void Renderer::Render()
{
	//Every pixel going left to right, up to down
	for (uint32_t y = 0; y < _FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < _FinalImage->GetWidth(); x++)
		{
			//Bringing it to 0 to 1 range
			glm::vec2 coord = {static_cast<float>(x) / _FinalImage->GetWidth(), static_cast<float>(y) / _FinalImage->GetHeight() };
			coord = coord * 2.f - 1.f; //-1 to 1
			_ImageData[x + y * _FinalImage->GetWidth()] = PerPixel(coord);
		}
	}
	_FinalImage->SetData(_ImageData);
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{
	glm::vec3 rayDirection(coord.x, coord.y, -1.f);
	glm::vec3 rayOrigin(0.0f,0.0,2.0f);
	float radius = 0.5f;
	//rayDirection = glm::normalize(rayDirection);

	//Solving for t
	//(bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ay^2 + ax^2 - r^2) = 0
	//a = ray origin
	//b = ray direction
	//r = radius
	//t = hit distance

	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.f * glm::dot(rayOrigin, rayDirection);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	float discriminant = b * b - 4.f * a * c;

	if (discriminant >= 0)
	{
		//std::cout << discriminant << " at " << rayDirection.x << "," << rayDirection.y << "\n";
		return 0xffff0000 | (uint8_t)(discriminant * 255.f) << 8;
	}
	return 0xff000000;
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