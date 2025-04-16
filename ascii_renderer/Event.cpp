#include "Event.h"
#include <stdexcept>

EventHandler::EventHandler()
	:
	hInput_(GetStdHandle(STD_INPUT_HANDLE))
{
	if (hInput_ == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("Could get standart input handle");
	}

	if (!GetConsoleMode(hInput_, &oldInputMode_)) {
		throw std::runtime_error("Could get current console mode");
	}

	DWORD mode = oldInputMode_ | (ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);
	mode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_QUICK_EDIT_MODE);
	if (!SetConsoleMode(hInput_, mode)) {
		throw std::runtime_error("Could set console mode");;
	}

}

bool EventHandler::WantRegainFocus() noexcept {
	return std::exchange(Get().wantFocus, false);
}

void EventHandler::OnKeyPressed(unsigned char code) noexcept
{
	Get().keyStates[code] = true;
}

void EventHandler::OnKeyReleased(unsigned char code) noexcept
{
	Get().keyStates[code] = false;
}

void EventHandler::OnMouseMove(int x, int y) noexcept
{
	Get().mousePosition = { x, y };
}

void EventHandler::OnLeftPressed() noexcept
{
	Get().leftIsPressed = true;
}

void EventHandler::OnLeftReleased() noexcept
{
	Get().leftIsPressed = false;
}

void EventHandler::OnRightPressed() noexcept
{
	Get().rightIsPressed = true;
}

void EventHandler::OnRightReleased() noexcept
{
	Get().rightIsPressed = false;
}

void EventHandler::OnResize(int width, int height) noexcept
{
	Get().consoleSize = { width, height };
}

EventHandler& EventHandler::Get()
{
	static EventHandler instance;
	return instance;
}

EventHandler::~EventHandler()
{
	SetConsoleMode(hInput_, oldInputMode_);
}

void EventHandler::ProcessConsoleEvents() noexcept
{
	INPUT_RECORD ir;
	DWORD read;
	if (PeekConsoleInput(Get().hInput_, &ir, 1, &read) && read > 0) {
		ReadConsoleInput(Get().hInput_, &ir, 1, &read);

		switch (ir.EventType) {
		case KEY_EVENT: {
			KEY_EVENT_RECORD keyEvent = ir.Event.KeyEvent;
			Get().wantFocus = (keyEvent.bKeyDown == TRUE);
			break;
		}
		case WINDOW_BUFFER_SIZE_EVENT: {
			Get().consoleSize.first = ir.Event.WindowBufferSizeEvent.dwSize.X;
			Get().consoleSize.second = ir.Event.WindowBufferSizeEvent.dwSize.Y;
			break;
		}
		}
	}
}

bool EventHandler::KeyIsPressed(unsigned char code) noexcept
{
	return Get().keyStates[code];
}

bool EventHandler::LMBIsPressed() noexcept
{
	return Get().leftIsPressed;
}

bool EventHandler::RMBIsPressed() noexcept
{
	return Get().rightIsPressed;
}

std::pair<int, int> EventHandler::GetMousePosition() noexcept
{
	return Get().mousePosition;
}

std::pair<int, int> EventHandler::GetConsoleSize() noexcept
{
	return Get().consoleSize;
}
