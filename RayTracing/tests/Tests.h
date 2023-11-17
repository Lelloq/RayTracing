#pragma once
#include <iostream>
#include "ShapeTests.h"

void ShapeCreationTest()
{
	TestScene scene;

	scene.Spheres.push_back(Sphere{ glm::vec3(0.f), 5.f, 0 });
	scene.Spheres.push_back(Sphere{ glm::vec3(0.f), 5.f, 0 });
	scene.Spheres.push_back(Sphere{ glm::vec3(0.f), 5.f, 0 });

	Shape::AddShapes(scene.Shapes, scene.Spheres);

	std::cout << scene.Shapes.size() << "\n";

	scene.TestShape.push_back(TestShape{ glm::vec3(0.f), 5, 0 });
	scene.TestShape.push_back(TestShape{ glm::vec3(0.f), 5, 0 });
	scene.TestShape.push_back(TestShape{ glm::vec3(0.f), 5, 0 });

	Shape::AddShapes(scene.Shapes, scene.TestShape);

	std::cout << scene.Shapes.size() << "\n";

	for(Shape* s : scene.Shapes)
	{
		std::cout << s << "\n";
	}

	Ray testRay;
	testRay.Direction = glm::vec3(1.f);
	testRay.Origin = glm::vec3(0.f);

	for(Shape* s : scene.Shapes)
	{
		float res = s->Hit(testRay);
		std::cout << res << "\n";
	}
}
