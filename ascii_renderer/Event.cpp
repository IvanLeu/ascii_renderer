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

EventHandler& EventHandler::Get()
{
	static EventHandler instance;
	return instance;
}

EventHandler::~EventHandler()
{
	SetConsoleMode(hInput_, oldInputMode_);
}

void EventHandler::ProcessEvents()
{
	INPUT_RECORD ir;
	DWORD read;
	if (PeekConsoleInput(hInput_, &ir, 1, &read) && read > 0) {
		ReadConsoleInput(hInput_, &ir, 1, &read);

		switch (ir.EventType) {
		case KEY_EVENT: 
		{
			KEY_EVENT_RECORD keyEvent = ir.Event.KeyEvent;
			keyStates[keyEvent.wVirtualKeyCode] = keyEvent.bKeyDown;
			break;
		}
		case MOUSE_EVENT:
		{
			MOUSE_EVENT_RECORD mouseEvent = ir.Event.MouseEvent;
			mousePosition.first = mouseEvent.dwMousePosition.X;
			mousePosition.second = mouseEvent.dwMousePosition.Y;

			leftIsPressed = (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED);
			rightIsPressed = (mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED);
	
			break;
		}
		case WINDOW_BUFFER_SIZE_EVENT:
		{
			WINDOW_BUFFER_SIZE_RECORD sizeEvent = ir.Event.WindowBufferSizeEvent;
			consoleSize.first = sizeEvent.dwSize.X;
			consoleSize.second = sizeEvent.dwSize.Y;
			break;
		}
		default:
			break;
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

std::optional<std::pair<int, int>> EventHandler::GetConsoleSize() noexcept
{
	if (Get().consoleSize.first <= 0 || Get().consoleSize.second <= 0) return {};
	return Get().consoleSize;
}
