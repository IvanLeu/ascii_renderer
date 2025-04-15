#include "Renderer.h"
#include <assert.h>
#include <stdexcept>

Renderer::Renderer()
{
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hOutput, &csbi);
	width_ = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	height_ = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	buffer_.resize(width_ * height_);

	CONSOLE_FONT_INFO fontInfo;
	if (!GetCurrentConsoleFont(hOutput, FALSE, &fontInfo)) {
		throw std::runtime_error("Failed to get current console font");
	}
	COORD fontSize = GetConsoleFontSize(hOutput, fontInfo.nFont);

	fontSize.X = fontSize.X <= 0 ? 8 : fontSize.X;

	charAspecRatio = (float)fontSize.X / (float)fontSize.Y;

	consoleScreenBuffer_ = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(consoleScreenBuffer_);
}

Renderer::~Renderer()
{
	CloseHandle(consoleScreenBuffer_);
}

void Renderer::BeginFrame()
{
	std::fill_n(buffer_.begin(), buffer_.size(), ' ');
}

void Renderer::EndFrame()
{
	DWORD dwBytesWritten = 0;
	WriteConsoleOutputCharacter(consoleScreenBuffer_, buffer_.data(), static_cast<DWORD>(buffer_.size()), { 0, 0 }, &dwBytesWritten);
}

void Renderer::Resize(int width, int height)
{
	if (width_ == width && height_ == height) {
		return;
	}

	width_ = width;
	height_ = height;

	buffer_.resize(width_ * height_);
	SetConsoleScreenBufferSize(consoleScreenBuffer_, COORD(static_cast<SHORT>(width_), static_cast<SHORT>(height_) ));
}

void Renderer::PutChar(int x, int y, char c)
{
	assert(x >= 0 && x < width_);
	assert(y >= 0 && y < height_);
	buffer_[y * width_ + x] = c;
}

const char& Renderer::GetChar(int x, int y)
{
	assert(x >= 0 && x < width_);
	assert(y >= 0 && y < height_);
	return buffer_[y * width_ + x];
}

int Renderer::GetWidth() const noexcept
{
	return width_;
}

int Renderer::GetHeight() const noexcept
{
	return height_;
}

float Renderer::GetFontAspectRatio() const noexcept
{
	return charAspecRatio;
}

float Renderer::GetAspectRatio() const noexcept
{
	return ((float)GetWidth() / (float)GetHeight()) * GetFontAspectRatio();
}

