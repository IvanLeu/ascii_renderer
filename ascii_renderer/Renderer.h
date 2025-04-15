#pragma once

#include <Windows.h>
#include <vector>

class Renderer {
public:
	Renderer();
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	~Renderer();
	void BeginFrame();
	void EndFrame();
	void Resize(int width, int height);
	void PutChar(int x, int y, char c);
	const char& GetChar(int x, int y);
	int GetWidth() const noexcept;
	int GetHeight() const noexcept;
	float GetFontAspectRatio() const noexcept;
	float GetAspectRatio() const noexcept;
private:
	float charAspecRatio;
	int width_ = 0;
	int height_ = 0;
	HANDLE consoleScreenBuffer_ = NULL;
	std::vector<char> buffer_;
};