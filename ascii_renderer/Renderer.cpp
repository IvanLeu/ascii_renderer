#include "Renderer.h"
#include "Exception.h"
#include <assert.h>

Renderer::Renderer()
{
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOutput == INVALID_HANDLE_VALUE) {
		throw HR_EXCEPTION_LAST;
	}

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(hOutput, &csbi)) {
		throw HR_EXCEPTION_LAST;
	}
	width_ = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	height_ = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	buffer_.resize(width_ * height_);

	CONSOLE_FONT_INFO fontInfo;
	if (!GetCurrentConsoleFont(hOutput, FALSE, &fontInfo)) {
		throw HR_EXCEPTION_LAST;
	}
	COORD fontSize = GetConsoleFontSize(hOutput, fontInfo.nFont);
	if (fontSize.X == 0 && fontSize.Y == 0) {
		throw HR_EXCEPTION_LAST;
	}

	fontSize.X = fontSize.X <= 0 ? 8 : fontSize.X;

	charAspecRatio = (float)fontSize.X / (float)fontSize.Y;

	consoleScreenBuffer_ = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, NULL);

	if (consoleScreenBuffer_ == INVALID_HANDLE_VALUE) {
		throw HR_EXCEPTION_LAST;
	}

	if (!SetConsoleActiveScreenBuffer(consoleScreenBuffer_)) {
		throw HR_EXCEPTION_LAST;
	}
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
	if (!WriteConsoleOutputCharacter(consoleScreenBuffer_, buffer_.data(), static_cast<DWORD>(buffer_.size()), { 0, 0 }, &dwBytesWritten)) {
		throw HR_EXCEPTION_LAST;
	}
}

void Renderer::Resize(int width, int height)
{
	if (width_ == width && height_ == height) {
		return;
	}

	width_ = width;
	height_ = height;

	buffer_.resize(width_ * height_);
	if (!SetConsoleScreenBufferSize(consoleScreenBuffer_, COORD(static_cast<SHORT>(width_), static_cast<SHORT>(height_)))) {
		throw HR_EXCEPTION_LAST;
	}
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
	return (static_cast<float>(GetWidth()) / static_cast<float>(GetHeight())) * GetFontAspectRatio();
}

