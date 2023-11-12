#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Rendering/Renderer.h"
#include "Camera.h"

#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Walnut::Layer
{
private:
	Renderer _Renderer;
	Camera _Camera;
	Scene _Scene;
	uint32_t _ViewportWidth = 0, _ViewportHeight = 0;

	bool _RealTime = false;

	float _LastRenderTime = 0.f;
public:
	ExampleLayer() : _Camera(45.f, 0.1f, 100.f) 
	{
		_Scene.Spheres.push_back(Sphere{ glm::vec3(0.0f,0.0f,0.f), 1.0f, 0 });
		_Scene.Spheres.push_back(Sphere{ glm::vec3(0.f,-101.5f,0.f), 100.f, 1 });

		_Scene.Materials.push_back(Material{glm::vec4(1.f,0.f,1.f,1.f), 1.f, 0.f});
		_Scene.Materials.push_back(Material{glm::vec4(1.f), 0.f, 1.f});
	};

	virtual void OnUpdate(float ts) override
	{
		if(_RealTime)
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
		ImGui::Checkbox("Real Time", &_RealTime);

		if (ImGui::Button("Reset"))
		{
			_Renderer.ResetFrameIndex();
		};
		ImGui::End(); 
		
		ImGui::Begin("Scene");
		for(size_t i = 0; i < _Scene.Spheres.size(); i++)
		{
			ImGui::PushID(i);

			Sphere& sphere = _Scene.Spheres.at(i);
			std::string name = "Sphere " + std::to_string(i + 1) + ":";
			ImGui::Text(name.c_str());
			bool s1 = ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
			bool s2 = ImGui::DragFloat("Radius", &sphere.Radius, 0.1f);
			bool s3 = ImGui::DragInt("Material Index", &sphere.MaterialIndex, 1.f, 0.f,(int)_Scene.Materials.size() - 1);

			if (s1 || s2 || s3) { _Renderer.ResetFrameIndex(); }

			ImGui::Separator();
			ImGui::PopID();
		}
		for (size_t i = 0; i < _Scene.Materials.size(); i++)
		{
			ImGui::PushID(i);

			Material& mat = _Scene.Materials.at(i);
			std::string name = "Material " + std::to_string(i + 1) + ":";
			ImGui::Text(name.c_str());
			bool s1 = ImGui::ColorEdit4("Albedo", glm::value_ptr(mat.Albedo), 0.1f);
			bool s2 = ImGui::DragFloat("Roughness", &mat.Roughness, 0.01f, 0.f, 1.f);
			bool s3 = ImGui::DragFloat("Mettalic", &mat.Metallic, 0.01f, 0.f, 1.f);

			if (s1 || s2 || s3) { _Renderer.ResetFrameIndex(); }

			ImGui::Separator();
			ImGui::PopID();
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f,0.f));
		ImGui::Begin("Viewport");

		_ViewportWidth = ImGui::GetContentRegionAvail().x;
		_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = _Renderer.GetFinalImage();
		if(image)
			ImGui::Image(image->GetDescriptorSet(), {(float)image->GetWidth(), (float)image->GetHeight()}, ImVec2(0,1), ImVec2(1, 0));

		ImGui::End();
		ImGui::PopStyleVar();

		if(_RealTime)
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

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}