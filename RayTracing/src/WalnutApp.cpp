#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

class ExampleLayer : public Walnut::Layer
{
private:
	std::shared_ptr<Walnut::Image> _Image;
	uint32_t* _ImageData = nullptr;
	uint32_t _ViewportWidth = 0, _ViewportHeight = 0;

	float _LastRenderTime = 0.f;
public:
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Render Time: %.3fms", _LastRenderTime);
		if (ImGui::Button("Render")) 
		{
			Render();
		};
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f,0.f));
		ImGui::Begin("Viewport");

		_ViewportWidth = ImGui::GetContentRegionAvail().x;
		_ViewportHeight = ImGui::GetContentRegionAvail().y;

		if(_Image)
			ImGui::Image(_Image->GetDescriptorSet(), {(float)_Image->GetWidth(), (float)_Image->GetHeight()});

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void Render() 
	{
		Walnut::Timer timer;

		if(!_Image || _ViewportHeight != _Image->GetHeight() || _ViewportWidth != _Image->GetWidth())
		{
			_Image = std::make_shared<Walnut::Image>(_ViewportWidth, _ViewportHeight, Walnut::ImageFormat::RGBA);
			delete[] _ImageData;
			_ImageData = new uint32_t[_ViewportWidth * _ViewportHeight];
		}

		for(uint32_t i = 0; i < _ViewportWidth * _ViewportHeight; i++)
		{
			_ImageData[i] = Walnut::Random::UInt();
			_ImageData[i] |= 0xff000000;
		}

		_Image->SetData(_ImageData);

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