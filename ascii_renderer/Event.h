#pragma once

#include <Windows.h>
#include <bitset>
#include <optional>
#include <queue>

class EventHandler {
	friend class ConsoleWindow;
public:
	struct RawDelta {
		int x, y;
	};
public:
	~EventHandler();
	static void ProcessConsoleEvents() noexcept;
	static EventHandler& Get();
	static bool KeyIsPressed(unsigned char code) noexcept;
	static bool LMBIsPressed() noexcept;
	static bool RMBIsPressed() noexcept;
	static std::pair<int, int> GetMousePosition() noexcept;
	static std::pair<int, int> GetConsoleSize() noexcept;
	static std::optional<RawDelta> ReadRawDelta() noexcept;
	static void ClearKeyStates() noexcept;
private:
	EventHandler();
	EventHandler(const EventHandler&) = delete;
	EventHandler& operator=(const EventHandler&) = delete;
private:
	static bool WantRegainFocus() noexcept;
	static void OnKeyPressed(unsigned char code) noexcept;
	static void OnKeyReleased(unsigned char code) noexcept;
	static void OnMouseMove(int x, int y) noexcept;
	static void OnLeftPressed() noexcept;
	static void OnLeftReleased() noexcept;
	static void OnRightPressed() noexcept;
	static void OnRightReleased() noexcept;
	static void OnResize(int width, int height) noexcept;
	static void OnRawDelta(int deltaX, int deltaY) noexcept;
	static void TrimRawBuffer() noexcept;
private:
	bool wantFocus = false;
	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int bufferSize = 16u;
	HANDLE hInput_;
	DWORD oldInputMode_;
	std::bitset<nKeys> keyStates;
	bool leftIsPressed = false;
	bool rightIsPressed = false;
	std::pair<int, int> mousePosition{ 0, 0 };
	std::pair<int, int> consoleSize{ 0, 0 };
	std::queue<RawDelta> rawBuffer;
};