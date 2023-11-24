#pragma once
#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Rendering/Renderer.h"
#include "Camera.h"

#include <glm/gtc/type_ptr.hpp>

class RayTraceLayer : public Walnut::Layer
{
private:
	Renderer _Renderer;
	Camera _Camera;
	Scene _Scene;
	uint32_t _ViewportWidth = 0, _ViewportHeight = 0;

	bool _KeepRendering = false;

	float _LastRenderTime = 0.f;
public:
	RayTraceLayer() : _Camera(45.f, 0.1f, 100.f)
	{
		_Scene.Spheres.push_back(Sphere{ glm::vec3(0.0f,2.0f,0.f), 1.0f, 0 });
		_Scene.Spheres.push_back(Sphere{ glm::vec3(0.f,-101.5f,0.f), 100.f, 1 });
		_Scene.Spheres.push_back(Sphere{ glm::vec3(1.5f, 0.0f,1.f), .5f, 2 });

		_Scene.Boxes.push_back(Box{ glm::vec3(1.0f, 0.f, 5.f), glm::vec3(.5f,0.5f,0.5f), 0 });

		_Scene.Materials.push_back(Material{ glm::vec3(1.f,0.f,1.f), 1.f, 0.f });
		_Scene.Materials.push_back(Material{ glm::vec3(1.f), 0.f, 1.f });
		_Scene.Materials.push_back(Material{ glm::vec4(1.f,0.f,0.f,1.f), 1.f, 0.f, glm::vec3(1.f,0.f,0.f), 1.0f });

		Shape::AddShapes(_Scene.Shapes, _Scene.Spheres);
		Shape::AddShapes(_Scene.Shapes, _Scene.Boxes);
	};

	virtual void OnUpdate(float ts) override
	{
		if (_KeepRendering)
		{
			bool moved = _Camera.OnUpdate(ts);
			if (moved) { _Renderer.ResetFrameIndex(); }
		}
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Render Time: %.3fms", _LastRenderTime);
		if (ImGui::Button("Render"))
		{
			Render();
		};

		ImGui::Checkbox("Accumulate", &_Renderer.GetSettings().Accumulate);
		ImGui::Checkbox("Keep Rendering", &_KeepRendering);
		ImGui::DragInt("Bounces", &_Renderer.GetSettings().Bounces, 1.f, 0, 10);
		ImGui::ColorEdit3("Sky Colour", glm::value_ptr(_Renderer.GetSettings().SkyColour));

		if (ImGui::Button("Reset"))
		{
			_Renderer.ResetFrameIndex();
		};
		ImGui::End();

		ImGui::Begin("Scene");
		for (size_t i = 0; i < _Scene.Spheres.size(); i++)
		{
			ImGui::PushID(i);

			Sphere& sphere = _Scene.Spheres.at(i);
			std::string name = "Sphere " + std::to_string(i + 1) + ":";
			ImGui::Text(name.c_str());
			bool s1 = ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
			bool s2 = ImGui::DragFloat("Radius", &sphere.Radius, 0.1f);
			bool s3 = ImGui::DragInt("Material Index", &sphere.MaterialIndex, 1.f, 0.f, (int)_Scene.Materials.size() - 1);

			if (s1 || s2 || s3) { _Renderer.ResetFrameIndex(); }

			ImGui::Separator();
			ImGui::PopID();
		}
		ImGui::End();

		ImGui::Begin("Materials");
		for (size_t i = 0; i < _Scene.Materials.size(); i++)
		{
			ImGui::PushID(i);

			Material& mat = _Scene.Materials.at(i);
			std::string name = "Material " + std::to_string(i + 1) + ":";
			ImGui::Text(name.c_str());
			bool s1 = ImGui::ColorEdit3("Albedo", glm::value_ptr(mat.Albedo), 0.1f);
			bool s2 = ImGui::DragFloat("Roughness", &mat.Roughness, 0.01f, 0.f, 1.f);
			bool s3 = ImGui::DragFloat("Metallic", &mat.Metallic, 0.01f, 0.f, 1.f);
			bool s4 = ImGui::ColorEdit3("Emissive Colour", glm::value_ptr(mat.EmissionColour));
			bool s5 = ImGui::DragFloat("Emission Power", &mat.EmissionPower, 0.01f, 0.f, std::numeric_limits<float>().max());

			if (s1 || s2 || s3 || s4 || s5) { _Renderer.ResetFrameIndex(); }

			ImGui::Separator();
			ImGui::PopID();
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
		ImGui::Begin("Viewport");

		_ViewportWidth = ImGui::GetContentRegionAvail().x;
		_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = _Renderer.GetFinalImage();
		if (image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() }, ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();
		ImGui::PopStyleVar();

		if (_KeepRendering)
		{
			Render();
		}
	}

	void Render()
	{
		Walnut::Timer timer;

		_Renderer.OnResize(_ViewportWidth, _ViewportHeight);
		_Camera.OnResize(_ViewportWidth, _ViewportHeight);
		_Renderer.Render(_Scene, _Camera);

		_LastRenderTime = timer.ElapsedMillis();
	};
};