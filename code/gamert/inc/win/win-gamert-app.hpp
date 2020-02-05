#pragma once

#include "gamert-application.hpp"


class WinGamertApp : public GamertApplication
{
public:
	WinGamertApp(HWND hwnd);
	~WinGamertApp();

public:
	virtual void on_init_vklayer() override;
	virtual void on_uninit_vklayer() override;
	virtual void on_init_renderers() override;
	virtual void on_uninit_renderers() override;
	virtual void on_min_gameframe() override;
	virtual float min_gameframe_interval() const override;
	virtual void on_sized() override;

private:
	HWND	_hwnd;
};



