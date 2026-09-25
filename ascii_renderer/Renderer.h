#pragma once

#include "utils\utility_classes.h"
#include <Windows.h>
#include <vector>

namespace core
{

class Renderer : private utils::NonCopyable
{
public:
    Renderer();
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

} // namespace core