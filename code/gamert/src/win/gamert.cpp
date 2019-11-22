#include "osenv.hpp"

#include "vkapp.hpp"
#include "vkcontext.hpp"
#include "vkrenderer2d.hpp"
#include "vscenegraph2d.hpp"
#include "vnode-quad2d.hpp"
#include "vmatrix.hpp"
#include "joystick.hpp"
#include "logicmgr.hpp"
#include "lnode2d-move.hpp"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef void(*update_frame_fnp_t)(void);

VKRenderer2d	g_render;
VSceneGraph2d	g_scene;

VNode2d* vn_mid = nullptr;;
VNode2d* vn_left = nullptr;;
VNode2d* vn_right = nullptr;;


void dummy_update()
{}

void render_update()
{
	JoyStick::get_instance().update_state();

	LogicMgr::get_instance().tick();
	g_render.update(30.f);
}

update_frame_fnp_t g_update_function = dummy_update;

void init_gamert_app(HWND hwnd)
{
	VKContext::get_instance().init("gamert",
		"gamert engine",
		VK_MAKE_VERSION(1, 0, 0),
		VK_MAKE_VERSION(1, 0, 0),
		hwnd);

	VKContext::get_instance().register_renderer(&g_render);
	VKContext::get_instance().resize();

	JoyStick::get_instance().check_controllers();

	VNode* root = new VNode();
	root->set_name("root");

	{ // middle
		VNodeQuad2d* quad = new VNodeQuad2d();
		quad->set_scale(VFVec2({ 150.f, 150.0f }));
		root->manage_child(quad);
		vn_mid = quad;
	}

	{ // left
		VNodeQuad2d* quad = new VNodeQuad2d();
		quad->set_poisition_fast(VFVec2({ -300.0f, 0.0f }));
		quad->set_scale_fast(VFVec2({ 200.f, 200.0f }));
		quad->calculate_world();
		root->manage_child(quad);
		vn_left = quad;
	}

	{ // right
		VNodeQuad2d* quad = new VNodeQuad2d();
		quad->set_poisition_fast(VFVec2({ 300.0f, 0.0f }));
		quad->set_scale_fast(VFVec2({ 100.f, 100.0f }));
		quad->calculate_world();
		root->manage_child(quad);
		vn_right = quad;
	}

	{
		LNode2dMove* move = new LNode2dMove();
		move->bind(vn_mid);
		LogicMgr::get_instance().root().manage_child(move);
	}
	

	g_scene.init();
	g_scene.switch_root_node(root);
	g_render.bind_scene_graph(&g_scene);
	g_update_function = render_update;

}

void uninit_gamert_app()
{
	g_scene.uninit();

	VNode* root = g_scene.switch_root_node(nullptr);
	delete root;	// root will release all its children.

	g_render.unint();
	VKContext::get_instance().destroy();
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

	// set fps timer
	LTimer fps_timer;
	fps_timer.snapshot();

	int elapsed = 0;
	const int itvl = 100;	// frames' interval in milliseconds

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
			elapsed = (int)fps_timer.elapsed();

			if (elapsed < itvl)
			{
				Sleep(itvl - elapsed);
			}

			g_update_function();
			fps_timer.snapshot();
		}
	}

	VKContext::get_instance().wait_device_idle();
	uninit_gamert_app();

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

