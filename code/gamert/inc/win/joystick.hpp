#pragma once

#include "pre-req.hpp"
#include "singleton.hpp"

/**
 * @class JoyStick
 * @brief Win32 implementation
 */
class JoyStick : public Singleton<JoyStick>
{
	DECL_SINGLETON_CTOR(JoyStick);

public:
	typedef struct _gamepad
	{
		_gamepad()
			: thumb_lx(0.0f)
			, thumb_ly(0.0f)
		{}

		float	thumb_lx;
		float	thumb_ly;
	} game_pad_t;

public:
	~JoyStick();

public:
	void					check_controllers();
	const std::vector<int>&	get_controllers() const;
	int						get_default_controller() const;
	void					update_state();
	const game_pad_t&		get_gamepad(int ctrlr) const;

	std::string				dump_state(void* state);

private:
	void					_update_gamepad(int ctrlr, void* state);

private:
	std::vector<int>		_controllers;
	std::vector<void*>		_states;
	std::vector<game_pad_t>	_gamepads;

private:
	static float _idz_left_thumb;
};


