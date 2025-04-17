#pragma once

#include <Windows.h>
#include <functional>
#include <optional>
#include <vector>

class ConsoleWindow {
public:
	ConsoleWindow();
	ConsoleWindow(const ConsoleWindow&) = delete;
	ConsoleWindow& operator=(const ConsoleWindow&) = delete;
	~ConsoleWindow() = default;
	std::optional<int> ProcessMessages() noexcept;
	bool IsFocused() const noexcept;
	void SetFocusState(bool state) noexcept;
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept;
	LRESULT HandleMsg(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept;
private:
	bool focused_ = false;
	HWND hWnd_;
	HWND consoleHWND_;
	RECT consoleWindowRect_;
	HINSTANCE hInst_;
	static constexpr const char* className_ = "HiddenWindowClass";
	std::vector<char> rawBuffer;
};