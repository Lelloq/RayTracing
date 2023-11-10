#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Globals.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Rendering/Renderer.h"
#include "Camera.h"

class ExampleLayer : public Walnut::Layer
{
private:
	Renderer _Renderer;
	Camera _Camera;
	uint32_t _ViewportWidth = 0, _ViewportHeight = 0;

	std::string _RealTimeOrSnapshot = "Snapshot";

	float _LastRenderTime = 0.f;
public:
	ExampleLayer() : _Camera(45.f, 0.1f, 100.f) {};

	virtual void OnUpdate(float ts) override
	{
		if(_RealTimeOrSnapshot == "Real Time")
		{
			_Camera.OnUpdate(ts);
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
		ImGui::ColorEdit4("Sphere Colour", (float*)&gSphereColour);
		if (ImGui::Button(_RealTimeOrSnapshot.c_str()))
		{
			_RealTimeOrSnapshot = (_RealTimeOrSnapshot == "Real Time") ? "Snapshot" : "Real Time";
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

		if(_RealTimeOrSnapshot == "Real Time")
		{
			Render();
		}
	}

	void Render() 
	{
		Walnut::Timer timer;

		_Renderer.OnResize(_ViewportWidth, _ViewportHeight);
		_Camera.OnResize(_ViewportWidth, _ViewportHeight);
		_Renderer.Render(_Camera);

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