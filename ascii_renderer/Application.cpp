#include "Application.h"
#include "Event.h"

Application::Application()
	:
	camera_(renderer_.GetWidth(), renderer_.GetHeight(), renderer_.GetFontAspectRatio(), 45.0f, 0.1f, 100.0f)
{
}

Application::~Application()
{
}

int Application::Run()
{
	while (running_) {
		EventHandler::ProcessConsoleEvents();
		if (const auto opt = consoleWnd_.ProcessMessages()) {
			running_ = false;
			return *opt;
		}

		float dt = timer_.Mark();
		Update_(dt);
		ComposeFrame_();
	}

	return 0;
}

void Application::Update_(float dt)
{ 
	auto [width, height] = EventHandler::GetConsoleSize();
	if (width != 0 && height != 0) {
		renderer_.Resize(width, height);
		camera_.Resize(width, height, renderer_.GetAspectRatio());
	}
	

	glm::vec3 dir{ 0.0f };
	if (EventHandler::KeyIsPressed('W')) {
		dir.z += 1.0f;
	}
	if (EventHandler::KeyIsPressed('A')) {
		dir.x -= 1.0f;
	}
	if (EventHandler::KeyIsPressed('S')) {
		dir.z -= 1.0f;
	}
	if (EventHandler::KeyIsPressed('D')) {
		dir.x += 1.0f;
	}
	if (EventHandler::KeyIsPressed('E')) {
		dir.y -= 1.0f;
	}
	if (EventHandler::KeyIsPressed('Q')) {
		dir.y += 1.0f;
	}

	camera_.Move(dt, dir);
	//camera_.Rotate(dt, deltaX, deltaY);
	camera_.Update();
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

	DrawSphere(sphere_);

	renderer_.EndFrame();
}

