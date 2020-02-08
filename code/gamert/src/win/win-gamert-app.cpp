#include "osenv.hpp"

#include "win-gamert-app.hpp"
#include "vkcontext.hpp"
#include "vkrenderer2d.hpp"
#include "filter-lscene.hpp"
#include "filter-vscene.hpp"
#include "resmgr-runtime.hpp"
#include "logicmgr.hpp"

VKRenderer2d	g_render;

WinGamertApp::WinGamertApp(HWND hwnd)
	: _hwnd(hwnd)
{}

WinGamertApp::~WinGamertApp()
{}

void WinGamertApp::on_init_vklayer() 
{
	VKContext::get_instance().init("gamert",
		"gamert engine",
		VK_MAKE_VERSION(1, 0, 0),
		VK_MAKE_VERSION(1, 0, 0),
		_hwnd);
	VKContext::get_instance().register_renderer(&g_render);
	VKContext::get_instance().resize();
}

void WinGamertApp::on_uninit_vklayer()
{
	VKContext::get_instance().destroy();
}

void WinGamertApp::on_init_renderers()
{	
	// init visual scene
	{
		FilterVScene fvs;
		VSceneGraph* vscene = fvs.load("vscene/default.xml");
		ResMgrRuntime::get_instance()
			.manage_visual_scene("default", vscene);
	}

	// init logic scene
	{
		FilterLScene fls;
		LSceneGraph* lscene = fls.load("lscene/default.xml");
		ResMgrRuntime::get_instance()
			.manage_logic_scene("default", lscene);
	}

	// init renderer
	g_render.bind_scene_graph(
		(VSceneGraph2d*)
		ResMgrRuntime::get_instance()
		.get_visual_scene("default"));

	// init logic layers
	LogicMgr::get_instance()
		.add_channel(
			"default",
			this->min_gameframe_interval(),
			ResMgrRuntime::get_instance()
				.get_logic_scene("default"));
}

void WinGamertApp::on_uninit_renderers()
{
	g_render.unint();
}

void WinGamertApp::on_min_gameframe()
{
	LogicMgr::get_instance().tick();
	g_render.update(30.f);
}

void WinGamertApp::on_sized()
{
	VKContext::get_instance().resize();
}

