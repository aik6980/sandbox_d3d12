

#include "app.h"
#include "engine_graphic/engine_graphic.h"

const int WINDOW_WIDTH	= 1920 / 2;
const int WINDOW_HEIGHT = 1080 / 2;

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main message handler.
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) {
		return true;
	}

	// Handle destroy/shutdown messages.
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	// Handle any messages the switch statement didn't.
	return DefWindowProc(hWnd, message, wParam, lParam);
}

wstring RegisterWinClass(HINSTANCE hInstance)
{
	auto winClassNameStr = L"WindowClass1";

	// Initialize the window class.
	WNDCLASSEX windowClass	  = {0};
	windowClass.cbSize		  = sizeof(WNDCLASSEX);
	windowClass.style		  = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc	  = WindowProc;
	windowClass.hInstance	  = hInstance;
	windowClass.hCursor		  = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = winClassNameStr;
	RegisterClassEx(&windowClass);

	return winClassNameStr;
}

HWND CreateMainWindow(HINSTANCE hInstance, int nCmdShow, const wstring& className, const CRect& rect)
{
	RECT windowRect = {0, 0, static_cast<LONG>(rect.Width()), static_cast<LONG>(rect.Height())};
	RECT emptyRect; // for border calculation
	SetRectEmpty(&emptyRect);
	AdjustWindowRect(&emptyRect, WS_OVERLAPPEDWINDOW, FALSE);
	// expand the rect to compensate the window border
	windowRect.right += emptyRect.right - emptyRect.left;
	windowRect.bottom += emptyRect.bottom - emptyRect.top;

	// Create the window and store a handle to it.
	HWND hwnd = CreateWindowEx(NULL, className.c_str(), L"Win32Project", WS_OVERLAPPEDWINDOW, 0, 100, windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL, // We have no parent window, NULL.
		NULL, // We aren't using menus, NULL.
		hInstance,
		NULL); // We aren't using multiple windows, NULL.

	ShowWindow(hwnd, nCmdShow);

	return hwnd;
}

int RunMainWindow(HINSTANCE hInstance, int nCmdShow)
{
	// register window class
	wstring winClassStr = RegisterWinClass(hInstance);
	// create window
	CRect dim(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	HWND  hwnd = CreateMainWindow(hInstance, nCmdShow, winClassStr, dim);
	// create app
	App app;
	app.on_init(hInstance, hwnd);

	// message loop
	MSG msg = {0};
	while (true) {
		// Process any messages in the queue.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;

			// Pass events into our sample.
			app.on_event_msg(msg);
		}

		app.on_update();
	}

	app.on_destroy();

	// Return this part of the WM_QUIT message to Windows.
	return static_cast<char>(msg.wParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	return RunMainWindow(hInstance, nCmdShow);
}
