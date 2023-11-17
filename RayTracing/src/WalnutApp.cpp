#include <glm/gtc/type_ptr.hpp>
#include "RayTraceLayer.h"

#define TESTING 0

#if TESTING
#include "Tests.h"
#endif

#if !TESTING
Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<RayTraceLayer>();
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
#else
Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec{ "Test", 200, 100 };
	Walnut::Application* app = new Walnut::Application(spec);

	ShapeCreationTest();

	return app;
}
#endif