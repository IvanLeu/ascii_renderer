#pragma once

#include <Windows.h>
#include <bitset>
#include <optional>

class EventHandler {
public:
	static EventHandler& Get();
	EventHandler(const EventHandler&) = delete;
	EventHandler& operator=(const EventHandler&) = delete;
	~EventHandler();
	void ProcessEvents();
	static bool KeyIsPressed(unsigned char code) noexcept;
	static bool LMBIsPressed() noexcept;
	static bool RMBIsPressed() noexcept;
	static std::pair<int, int> GetMousePosition() noexcept;
	static std::optional<std::pair<int, int>> GetConsoleSize() noexcept;
private:
	EventHandler();
	void OnKeyPressed(unsigned char code) noexcept;
	void OnKeyReleased(unsigned char code) noexcept;
	void OnMouseMove(int x, int y) noexcept;
	void OnLeftPressed() noexcept;
	void OnLeftReleased() noexcept;
	void OnRightPressed() noexcept;
	void OnRightReleased() noexcept;
	void OnResize(int width, int height) noexcept;
private:
	static constexpr unsigned int nKeys = 256u;
	HANDLE hInput_;
	DWORD oldInputMode_;
	std::bitset<nKeys> keyStates;
	bool leftIsPressed = false;
	bool rightIsPressed = false;
	std::pair<int, int> mousePosition{ 0, 0 };
	std::pair<int, int> consoleSize{ 0, 0 };
};