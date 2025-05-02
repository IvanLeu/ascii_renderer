#include "HiddenWindow.h"
#include "Event.h"
#include "Exception.h"

HiddenWindow::HiddenWindow()
	:
	hInst_(GetModuleHandle(NULL))
{
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
		throw HR_EXCEPTION_LAST;
	}

	hWnd_ = CreateWindowEx(
		0,
		className_,
		"HiddenWindow",
		WS_POPUP,
		0,
		0,
		10,
		10,
		0,
		NULL,
		hInst_,
		this
	);

	if (!hWnd_) {
		throw HR_EXCEPTION_LAST;
	}

	ShowWindow(hWnd_, SW_SHOW);

	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01;          
	rid.usUsage = 0x02;              
	rid.dwFlags = RIDEV_INPUTSINK;
	rid.hwndTarget = hWnd_;

	if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE) {
		throw HR_EXCEPTION_LAST;
	}
}

std::optional<int> HiddenWindow::ProcessMessages() noexcept
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

bool HiddenWindow::IsFocused() const noexcept
{
	return focused_;
}

void HiddenWindow::SetFocusState(bool state) noexcept
{
	focused_ = state;

	if (focused_) {
		SetForegroundWindow(hWnd_);
	}

}

void HiddenWindow::EnableCursor() noexcept
{
	enabledCursor = true;
	FreeCursor();
	ShowCursor();
}

void HiddenWindow::DisableCursor() noexcept
{
	enabledCursor = false;
	ConfineCursor();
	HideCursor();
}

bool HiddenWindow::CursorEnabled() const noexcept
{
	return enabledCursor;
}

void HiddenWindow::ShowCursor() const
{
	while (::ShowCursor(TRUE) < 0);
}

void HiddenWindow::HideCursor() const
{
	while (::ShowCursor(FALSE) >= 0);
}

void HiddenWindow::ConfineCursor() const
{
	RECT rect{ 0, 0, 0, 0};
	ClipCursor(&rect);
}

void HiddenWindow::FreeCursor() const
{
	ClipCursor(nullptr);
}

LRESULT HiddenWindow::HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
{
	if (msg == WM_NCCREATE) {
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lparam);
		HiddenWindow* const pWnd = static_cast<HiddenWindow*>(pCreate->lpCreateParams);

		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&HiddenWindow::HandleMsgThunk));

		return pWnd->HandleMsg(hwnd, msg, wparam, lparam);
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

LRESULT HiddenWindow::HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
{
	auto pWnd = reinterpret_cast<HiddenWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	return pWnd->HandleMsg(hwnd, msg, wparam, lparam);
}

LRESULT HiddenWindow::HandleMsg(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
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
		if (!EventHandler::RawEnabled()) {
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
	case WM_ACTIVATE: {
		if (!CursorEnabled()) {
			if (wparam & WA_ACTIVE) {
				ConfineCursor();
				HideCursor();
				EventHandler::EnableRaw();
			}
			else {
				FreeCursor();
				ShowCursor();
				EventHandler::DisableRaw();
			}
		}
	} break; 
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}
