#pragma once

enum nav_inputs
{
	NAV_ACTIVATE,
	NAV_BACK,
	NAV_DPAD_LEFT,
	NAV_DPAD_RIGHT,
	NAV_DPAD_UP,
	NAV_DPAD_DOWN,
	NAV_R1,
	NAV_R2,
	NAV_L1,
	NAV_L2,
	NAV_L3,
	NAV_R3,
	NAV_START,
	NAV_SELECT,
	NAV_LSTICK,
	NAV_RSTICK,
	NAV_RSTICK_LEFT,
	NAV_RSTICK_RIGHT,
	NAV_RSTICK_UP,
	NAV_RSTICK_DOWN,
	NAV_MAX
};

enum analog_t
{
	ANALOG_RIGHT_X,
	ANALOG_RIGHT_Y,
	ANALOG_LEFT_X,
	ANALOG_LEFT_Y
};

struct analog_input_t
{
	vec2_t analogLeft;
	vec2_t analogRight;
};

class navigation
{
public:
	float is_down(int key, float repeat_rate = 0.f);
	float held_time(int key);
	bool is_pressed(int key);
	bool is_released(int key);

	void new_frame();
	void end_frame();

	CellPadData pad;
private:
	float get_analog_input(analog_t analog);

	void poll_analog(int* x, int* y);

	float nav_inputs[NAV_MAX];
	float nav_inputs_duration[NAV_MAX];
	float nav_inputs_duration_prev[NAV_MAX];
};

extern navigation g_nav;