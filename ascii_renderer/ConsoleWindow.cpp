#include "ConsoleWindow.h"
#include "Event.h"
#include <stdexcept>

ConsoleWindow::ConsoleWindow()
	:
	hInst_(GetModuleHandle(NULL)),
	consoleHWND_(GetConsoleWindow())
{
	GetWindowRect(consoleHWND_, &consoleWindowRect_);

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst_;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = className_;
	wc.hIconSm = nullptr;

	if (!RegisterClassEx(&wc)) {
		throw std::runtime_error("Failed to register window class");
	}

	hWnd_ = CreateWindowEx(
		0,
		className_,
		"HiddenWindow",
		0,
		-10000,
		-10000,
		0,
		0,
		0,
		NULL,
		hInst_,
		this
	);

	if (!hWnd_) {
		throw std::runtime_error("Failed to create HWND");
	}

	ShowWindow(hWnd_, SW_SHOW);

	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01;          
	rid.usUsage = 0x02;              
	rid.dwFlags = RIDEV_INPUTSINK;
	rid.hwndTarget = hWnd_;

	if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE) {
		throw std::runtime_error("Failed to register raw input devices");
	}
}

std::optional<int> ConsoleWindow::ProcessMessages() noexcept
{
	if (!IsFocused()) {
		SetFocusState(EventHandler::WantRegainFocus());
	}

	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			return (int)msg.wParam;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}

bool ConsoleWindow::IsFocused() const noexcept
{
	return focused_;
}

void ConsoleWindow::SetFocusState(bool state) noexcept
{
	focused_ = state;

	if (focused_) {
		SetForegroundWindow(hWnd_);
	}

}

void ConsoleWindow::EnableCursor() noexcept
{
	enabledCursor = true;
	FreeCursor();
	ShowCursor();
}

void ConsoleWindow::DisableCursor() noexcept
{
	enabledCursor = false;
	ConfineCursor();
	HideCursor();
}

bool ConsoleWindow::CursorEnabled() const noexcept
{
	return enabledCursor;
}

void ConsoleWindow::ShowCursor() const
{
	while (::ShowCursor(TRUE) < 0);
}

void ConsoleWindow::HideCursor() const
{
	while (::ShowCursor(FALSE) >= 0);
}

void ConsoleWindow::ConfineCursor() const
{
	RECT rect{ 0, 0, 0, 0};
	ClipCursor(&rect);
}

void ConsoleWindow::FreeCursor() const
{
	ClipCursor(nullptr);
}

LRESULT ConsoleWindow::HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
{
	if (msg == WM_NCCREATE) {
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lparam);
		ConsoleWindow* const pWnd = static_cast<ConsoleWindow*>(pCreate->lpCreateParams);

		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&ConsoleWindow::HandleMsgThunk));

		return pWnd->HandleMsg(hwnd, msg, wparam, lparam);
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

LRESULT ConsoleWindow::HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
{
	auto pWnd = reinterpret_cast<ConsoleWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	return pWnd->HandleMsg(hwnd, msg, wparam, lparam);
}

LRESULT ConsoleWindow::HandleMsg(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
{
	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN: {
		EventHandler::OnKeyPressed(static_cast<char>(wparam));
	} break;
	case WM_SYSKEYUP:
	case WM_KEYUP: {
		EventHandler::OnKeyReleased(static_cast<char>(wparam));
	} break;
	case WM_LBUTTONDOWN: {
		EventHandler::OnLeftPressed();
	} break;
	case WM_LBUTTONUP: {
		EventHandler::OnLeftReleased();
	} break;
	case WM_RBUTTONDOWN: {
		EventHandler::OnRightPressed();
	} break;
	case WM_RBUTTONUP: {
		EventHandler::OnRightReleased();
	} break;
	case WM_KILLFOCUS: {
		SetFocusState(false);
		EventHandler::ClearKeyStates();
	} break;
	case WM_INPUT: {
		if (CursorEnabled()) {
			break;
		}

		UINT size;
		if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) == -1) {
			break;
		}

		rawBuffer.resize(size);

		if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, rawBuffer.data(), &size, sizeof(RAWINPUTHEADER)) == -1) {
			break;
		}

		auto& raw = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());

		if (raw.header.dwType == RIM_TYPEMOUSE && (raw.data.mouse.lLastX != 0 || raw.data.mouse.lLastY != 0)) {
			EventHandler::OnRawDelta(raw.data.mouse.lLastX, raw.data.mouse.lLastY);
		}

	} break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}
