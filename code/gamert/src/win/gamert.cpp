#include "osenv.hpp"

#include "vkapp.hpp"
#include "vkcontext.hpp"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void init_gamert_app(HWND hwnd)
{
	VKContext::get_instance().init("gamert",
		"gamert engine",
		VK_MAKE_VERSION(1, 0, 0),
		VK_MAKE_VERSION(1, 0, 0),
		hwnd);
}

void uninit_gamert_app()
{
	VKContext::get_instance().uninit();
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Gamert Window Class";
	const wchar_t MAINWIN_NAME[] = L"Gamert View";
	const int MAINWIN_WIDTH = 1000;
	const int MAINWIN_HEIGHT = 800;

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	// Create the window.

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		MAINWIN_NAME,    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, MAINWIN_WIDTH, MAINWIN_HEIGHT,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == NULL)
	{
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	
	init_gamert_app(hwnd);

	// Run the message loop.
	MSG msg = { };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, NULL, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Sleep(30);
		}
	}

	uninit_gamert_app();

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SIZE:
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

