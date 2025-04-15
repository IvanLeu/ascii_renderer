#pragma once

#include <Windows.h>
#include <functional>
#include <optional>

class ConsoleWindow {
	friend class EventHandler;
public:
	ConsoleWindow();
	static std::optional<int> ProcessMessages() noexcept;
private:
	static LRESULT CALLBACK HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept;
	LRESULT HandleMsg(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept;
private:
	HWND hWnd_;
	std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> oldWndProc_;
};