#include "osenv.hpp"

#include "vkcontext.hpp"
#include "vkrenderer.hpp"
#include "vscenegraph2d.hpp"
#include "vnode-quad2d.hpp"
#include "vmatrix.hpp"
#include "lnode2d-move.hpp"
#include "win-gamert-app.hpp"
#include "ltimer.hpp"

#define	FRAMES_FLIPPING_INTERVAL		16.f

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


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
	
	std::unique_ptr<GamertApplication> app(new WinGamertApp(hwnd));

	app->init();
	
	// set fps timer
	LTimer fps_timer;
	fps_timer.snapshot();

	float elapsed = 0.f;

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
			elapsed = fps_timer.elapsed();

			if (elapsed < FRAMES_FLIPPING_INTERVAL)
			{
				Sleep((DWORD)(FRAMES_FLIPPING_INTERVAL - elapsed));
			}

			app->do_min_gameframe();
			fps_timer.snapshot();
		}
	}

	app->uninit();

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SIZE:
		VKContext::get_instance().resize();
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

