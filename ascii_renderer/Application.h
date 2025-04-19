#pragma once

#include "Renderer.h"
#include "HiddenWindow.h"
#include "Timer.h"
#include "Camera.h"

#include <glm\glm.hpp>

struct Sphere {
	float r = 0.5f;
	glm::vec3 pos = { 0.0f, 0.0f, 0.0f };
};

class Application {
public:
	Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
	~Application();
	int Run();
	void DrawSphere(const Sphere& sphere);
private:
	void Update_(float dt);
	void ComposeFrame_();
private:
	bool running_ = true;
	Timer timer_;
	HiddenWindow consoleWnd_;
	Renderer renderer_;
	Camera camera_;
	Sphere sphere_;
};