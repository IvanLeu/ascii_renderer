#include "ConsoleWindow.h"

ConsoleWindow::ConsoleWindow()
	:
	hWnd_(GetConsoleWindow())
{
	oldWndProc_ = reinterpret_cast<LRESULT(*)(HWND, UINT, WPARAM, LPARAM)>(GetWindowLongPtr(hWnd_, GWLP_WNDPROC));

	SetWindowLongPtr(hWnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

std::optional<int> ConsoleWindow::ProcessMessages() noexcept
{
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

LRESULT ConsoleWindow::HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
{
	auto pWnd = reinterpret_cast<ConsoleWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	return pWnd->HandleMsg(hwnd, msg, wparam, lparam);
}

LRESULT ConsoleWindow::HandleMsg(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
{
	switch (msg) {

	}

	return oldWndProc_(hwnd, msg, wparam, lparam);
}
