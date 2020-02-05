#pragma once

#include "pre-req.hpp"

class GamertApplication
{
public:
	GamertApplication();
	~GamertApplication();

public:
	void init();
	void uninit();
	void do_min_gameframe();
	void sized();

	virtual float min_gameframe_interval() const = 0;
	virtual void on_min_gameframe() = 0;
	virtual void on_init_vklayer() = 0;
	virtual void on_uninit_vklayer() = 0;
	virtual void on_sized() = 0;
	// TODO: let dynopt to do the following works
	virtual void on_init_renderers() = 0;
	virtual void on_uninit_renderers() = 0;
};

