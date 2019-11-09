#include "osenv.hpp"

#include "vkapp.hpp"
#include "vkcontext.hpp"
#include "vkrenderer2d.hpp"
#include "vscenegraph.hpp"
#include "vnode-quad2d.hpp"
#include "vmatrix.hpp"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef void(*update_frame_fnp_t)(void);

VKRenderer2d	g_render;
VSceneGraph		g_scene;

void dummy_update()
{}

void render_update()
{
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

	VNode* root = new VNode();
	root->set_name("root");
	VNodeQuad2d* quad = new VNodeQuad2d();
	{
		std::array<VVertex2DRGB, 4> vertices;
		vertices[0] = VVertex2DRGB(VFVec2({ -100.f, -100.f }), VFVec3({ 0.9f, 0.5f, 0.3f }));
		vertices[1] = VVertex2DRGB(VFVec2({ 100.f, -100.f }), VFVec3({ 0.3f, 0.9f, 0.5f }));
		vertices[2] = VVertex2DRGB(VFVec2({ 100.f, 100.f }), VFVec3({ 0.5f, 0.3f, 0.9f }));
		vertices[3] = VVertex2DRGB(VFVec2({ -100.f, 100.f }), VFVec3({ 0.3f, 0.9f, 1.5f }));
		
		quad->set_vertices(vertices);
		quad->set_indices({ 0, 1, 2, 2, 3, 0 });
	}

	root->manage_child(quad);
	g_scene.switch_root_node(root);
	g_render.set_scene_graph(&g_scene);
	g_update_function = render_update;
}

void uninit_gamert_app()
{
	VNode* root = g_scene.switch_root_node(nullptr);
	delete root;	// root will release all its children.

	g_render.unint();
	VKContext::get_instance().destroy();
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	VMatrix<float, 4,4> mat;

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
			g_update_function();
			Sleep(100);
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

