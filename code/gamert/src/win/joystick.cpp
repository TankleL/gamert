#include "joystick.hpp"
#include <Xinput.h>

float JoyStick::_idz_left_thumb = (float)(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE / 2);

JoyStick::JoyStick()
	: _states(XUSER_MAX_COUNT, nullptr)
	, _gamepads(XUSER_MAX_COUNT, game_pad_t())
{
	for (auto& state : _states)
	{
		state = new XINPUT_STATE();
	}
}

JoyStick::~JoyStick()
{
	for (auto& state : _states)
	{
		GRT_SAFE_DELETE(state);
	}
}

void JoyStick::check_controllers()
{
	XINPUT_STATE state;
	DWORD res = 0;
	for (auto i = 0; i < XUSER_MAX_COUNT; ++i)
	{
		res = XInputGetState(i, &state);
		if (res == ERROR_SUCCESS)
		{ // controller connected
			auto c = _controllers.begin();
			for (; c != _controllers.end(); ++c)
			{
				if (*c == i)
					break;
			}
			
			if(c == _controllers.end())
				_controllers.push_back(i);
		}
		else
		{ // lost controller
			auto c = _controllers.begin();
			for (; c != _controllers.end(); ++c)
			{
				if (*c == i)
					_controllers.erase(c);
			}
		}
	}
}

const std::vector<int>& JoyStick::get_controllers() const
{
	return _controllers;
}

int JoyStick::get_default_controller() const
{
	int res = -1;
	if (_controllers.size() > 0)
	{
		res = _controllers.front();
	}
	return res;
}

void JoyStick::update_state()
{
	for (auto i = 0; i < _controllers.size(); ++i)
	{
		const auto& ctrlr = _controllers[i];
		XINPUT_STATE* s = (XINPUT_STATE*)_states[ctrlr];
		DWORD res = XInputGetState(ctrlr, s);

		if (res == ERROR_SUCCESS)
		{
			//OutputDebugStringA(dump_state(s).c_str());
			_update_gamepad(ctrlr, s);
		}
		else
		{
			check_controllers();
		}
	}
}

const JoyStick::game_pad_t& JoyStick::get_gamepad(int ctrlr) const
{
	return _gamepads[ctrlr];
}

void JoyStick::_update_gamepad(int ctrlr, void* state)
{
	XINPUT_STATE* s = (XINPUT_STATE*)state;
	game_pad_t& g = _gamepads[ctrlr];

	s->dwPacketNumber;

	float lx = (float)s->Gamepad.sThumbLX;
	float ly = (float)s->Gamepad.sThumbLY;
	float lmag = std::sqrtf(lx*lx + ly*ly);

	if (lmag > _idz_left_thumb)
	{
		g.thumb_lx = lx / lmag;
		g.thumb_ly = -ly / lmag;
	}
	else
	{
		g.thumb_ly = 0.f;
		g.thumb_lx = 0.f;
	}
}

std::string JoyStick::dump_state(void* state)
{
	XINPUT_STATE* s = (XINPUT_STATE*)state;
	std::ostringstream oss;
	oss << "lt:\t" << (int)s->Gamepad.bLeftTrigger << " | "
		<< "rt:\t" << (int)s->Gamepad.bRightTrigger << "\n"
		<< "lx:\t" << (short)s->Gamepad.sThumbLX << " | ly:\t" << (short)s->Gamepad.sThumbLY
		<< "\n"
		<< "rx:\t" << (short)s->Gamepad.sThumbRX << " | ry:\t" << (short)s->Gamepad.sThumbRY
		<< std::endl;
	

	return oss.str();
}



