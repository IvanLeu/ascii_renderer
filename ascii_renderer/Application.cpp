#include "Application.h"

static int lastMouseX = 0;
static int lastMouseY = 0;

Application::Application()
	:
	camera_(renderer_.GetWidth(), renderer_.GetHeight(), renderer_.GetFontAspectRatio(), 45.0f, 0.1f, 100.0f)
{
}

Application::~Application()
{
}

void Application::Run()
{
	while (true) {
		eventHandler_.ProcessEvents();
		float dt = timer.Mark();
		Update_(dt);
		ComposeFrame_();
	}
}

void Application::Update_(float dt)
{ 
	auto optSz = eventHandler_.GetConsoleSize();
	if (optSz.has_value()) {
		auto [width, height] = *optSz;
		renderer_.Resize(width, height);
		camera_.Resize(width, height, renderer_.GetAspectRatio());
	}

	glm::vec3 dir{ 0.0f };
	if (eventHandler_.KeyIsPressed('W')) {
		dir.z += 1.0f;
	}
	if (eventHandler_.KeyIsPressed('A')) {
		dir.x -= 1.0f;
	}
	if (eventHandler_.KeyIsPressed('S')) {
		dir.z -= 1.0f;
	}
	if (eventHandler_.KeyIsPressed('D')) {
		dir.x += 1.0f;
	}
	if (eventHandler_.KeyIsPressed('E')) {
		dir.y -= 1.0f;
	}
	if (eventHandler_.KeyIsPressed('Q')) {
		dir.y += 1.0f;
	}

	int deltaX = eventHandler_.GetMousePosition().first - lastMouseX;
	int deltaY = eventHandler_.GetMousePosition().second - lastMouseY;

	camera_.Move(dt, dir);
	camera_.Rotate(dt, deltaX, deltaY);
	camera_.Update();

	lastMouseX = eventHandler_.GetMousePosition().first;
	lastMouseY = eventHandler_.GetMousePosition().second;
}

static char GetShadeCharacter(float distance, float radius) {
	std::vector<char> shades = { '@', '%', '#', '*', '+', '=', '-', '.' };
	const float maxDistance = radius;
	if (distance < 0.0f)
		distance = 0.0f;
	if (distance > maxDistance)
		distance = maxDistance;

	float t = distance / maxDistance;
	int index = static_cast<int>(t * (shades.size() - 1));
	return shades[index];
}

void Application::DrawSphere(const Sphere& sphere) {
	for (int y = 0; y < renderer_.GetHeight(); ++y) {
		for (int x = 0; x < renderer_.GetWidth(); ++x) {

			glm::vec3 dir = camera_.GetRayDirections()[x + y * renderer_.GetWidth()];

			glm::vec3 origin = camera_.GetPosition() - sphere.pos;

			constexpr const float a = 1.0f;
			const float b = 2.0f * glm::dot(origin, dir);
			const float c = glm::dot(origin, origin) - sphere.r * sphere.r;

			const float D = b * b - 4 * a * c;
			if (D < 0.0f) {
				continue;
			}

			const float closestHit = (-b - sqrt(D)) / (2.0f * a);
			char shade = GetShadeCharacter(closestHit, sphere.r);

			renderer_.PutChar(x, y, shade);
		}
	}
}

void Application::ComposeFrame_()
{
	renderer_.BeginFrame();

	DrawSphere(sphere);

	renderer_.EndFrame();
}

