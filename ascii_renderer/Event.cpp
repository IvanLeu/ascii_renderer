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
	Get().keyboardBuffer.push({ EventHandler::KeyboardEvent::Type::IsPressed, code });
	Get().TrimKeyboardBuffer();
}

void EventHandler::OnKeyReleased(unsigned char code) noexcept
{
	Get().keyStates[code] = false;
	Get().keyboardBuffer.push({ EventHandler::KeyboardEvent::Type::IsReleased, code });
	Get().TrimKeyboardBuffer();
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

void EventHandler::OnRawDelta(int deltaX, int deltaY) noexcept
{
	Get().rawBuffer.push({ deltaX, deltaY });
	Get().TrimRawBuffer();
}

void EventHandler::TrimRawBuffer() noexcept
{
	while (Get().rawBuffer.size() > Get().bufferSize) {
		Get().rawBuffer.pop();
	}
}

void EventHandler::TrimKeyboardBuffer() noexcept
{
	while (Get().rawBuffer.size() > Get().bufferSize) {
		Get().rawBuffer.pop();
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

			if (keyEvent.bKeyDown == TRUE) {
				Get().OnKeyPressed(static_cast<unsigned char>(keyEvent.wVirtualKeyCode));
			}

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

std::optional<EventHandler::KeyboardEvent> EventHandler::ReadKeyboard() noexcept
{
	if (!Get().keyboardBuffer.empty()) {
		auto e = Get().keyboardBuffer.front();
		Get().keyboardBuffer.pop();

		return e;
	}

	return {};
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

std::optional<EventHandler::RawDelta> EventHandler::ReadRawDelta() noexcept
{
	if (!Get().rawBuffer.empty()) {
		auto front = Get().rawBuffer.front();
		Get().rawBuffer.pop();
		return front;
	}

	return {};
}

void EventHandler::ClearKeyStates() noexcept
{
	Get().keyStates.reset();
}

void EventHandler::EnableRaw() noexcept
{
	Get().rawEnabled = true;
}

void EventHandler::DisableRaw() noexcept
{
	Get().rawEnabled = false;
}

bool EventHandler::RawEnabled() noexcept
{
	return Get().rawEnabled;
}
