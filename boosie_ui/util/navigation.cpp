#include "stdafx.h"

navigation g_nav;

void IN_RescaleAnalog(int* x, int* y, int dead) {

	float analogX = (float)*x;
	float analogY = (float)*y;
	float deadZone = (float)dead;
	float maximum = 32768.0f;
	float magnitude = sqrt(analogX * analogX + analogY * analogY);
	if (magnitude >= deadZone)
	{
		float scalingFactor = maximum / magnitude * (magnitude - deadZone) / (maximum - deadZone);
		*x = (int)(analogX * scalingFactor);
		*y = (int)(analogY * scalingFactor);
	}
	else {
		*x = 0;
		*y = 0;
	}
}

int calc_repeat_amount(float t, float t_prev, float repeat_delay, float repeat_rate)
{
	if (t == 0.0f)
		return 1;
	if (t <= repeat_delay || repeat_rate <= 0.0f)
		return 0;
	const int count = (int)((t - repeat_delay) / repeat_rate) - (int)((t_prev - repeat_delay) / repeat_rate);
	return (count > 0) ? count : 0;
}

float navigation::is_down(int key, float repeat_rate)
{
	const float t = nav_inputs_duration[key];

	if (repeat_rate == 0.f)
		return nav_inputs[key] == 1.f ? true : false;

	return (float)calc_repeat_amount(t, t - (1.f / 60.f), 0.f, repeat_rate) ? true : false;
}

bool navigation::is_pressed(int key)
{
	const float t = nav_inputs_duration[key];
	return (t == 0.f) ? true : false;
}

bool navigation::is_released(int key)
{
	const float f = nav_inputs_duration[key];

	if (f <= 0.f)
		return nav_inputs_duration_prev[key] >= 0.0f ? true : false;

	return false;
}

void navigation::new_frame()
{
	nav_inputs[NAV_ACTIVATE] = (pad.button[DIGITAL2] & CELL_PAD_CTRL_CROSS) ? 1.f : 0.f;
	nav_inputs[NAV_BACK] = (pad.button[DIGITAL2] & CELL_PAD_CTRL_CIRCLE) ? 1.f : 0.f;
	nav_inputs[NAV_DPAD_LEFT] = (pad.button[DIGITAL1] & CELL_PAD_CTRL_LEFT) ? 1.f : 0.f;
	nav_inputs[NAV_DPAD_RIGHT] = (pad.button[DIGITAL1] & CELL_PAD_CTRL_RIGHT) ? 1.f : 0.f;
	nav_inputs[NAV_DPAD_UP] = (pad.button[DIGITAL1] & CELL_PAD_CTRL_UP) ? 1.f : 0.f;
	nav_inputs[NAV_DPAD_DOWN] = (pad.button[DIGITAL1] & CELL_PAD_CTRL_DOWN) ? 1.f : 0.f;
	nav_inputs[NAV_R1] = (pad.button[DIGITAL2] & CELL_PAD_CTRL_R1) ? 1.f : 0.f;
	nav_inputs[NAV_R2] = (pad.button[DIGITAL2] & CELL_PAD_CTRL_R2) ? 1.f : 0.f;
	nav_inputs[NAV_L1] = (pad.button[DIGITAL2] & CELL_PAD_CTRL_L1) ? 1.f : 0.f;
	nav_inputs[NAV_L2] = (pad.button[DIGITAL2] & CELL_PAD_CTRL_L2) ? 1.f : 0.f;
	nav_inputs[NAV_L3] = (pad.button[DIGITAL1] & CELL_PAD_CTRL_L3) ? 1.f : 0.f;
	nav_inputs[NAV_R3] = (pad.button[DIGITAL1] & CELL_PAD_CTRL_R3) ? 1.f : 0.f;
	nav_inputs[NAV_START] = (pad.button[DIGITAL1] & CELL_PAD_CTRL_START) ? 1.f : 0.f;
	nav_inputs[NAV_SELECT] = (pad.button[DIGITAL1] & CELL_PAD_CTRL_SELECT) ? 1.f : 0.f;

	int lstick_x, lstick_y = 0;
	poll_analog(&lstick_x, &lstick_y);

	if (lstick_x < 0)
		nav_inputs[NAV_RSTICK_LEFT] = (float)(-lstick_x / 16);
	if (lstick_x > 0)
		nav_inputs[NAV_RSTICK_RIGHT] = (float)(lstick_x / 16);
	if (lstick_y < 0)
		nav_inputs[NAV_RSTICK_UP] = (float)(-lstick_y / 16);
	if (lstick_y > 0)
		nav_inputs[NAV_RSTICK_DOWN] = (float)(lstick_y / 16);

	memcpy(nav_inputs_duration_prev, nav_inputs_duration, sizeof(float) * NAV_MAX);
	for (int i = 0; i < NAV_MAX; i++)
		nav_inputs_duration[i] = (nav_inputs[i] > 0.0f) ? (nav_inputs_duration[i] < 0.0f ? 0.0f : nav_inputs_duration[i] + (1.f / 60.f)) : -1.0f;

}

void navigation::end_frame()
{
	memset(nav_inputs, 0, sizeof(float) * NAV_MAX);
}


float navigation::get_analog_input(analog_t analog)
{
	static short analog_deadzone = 20;
	int16_t input = pad.button[4 + analog] - 128;
	if ((input > -analog_deadzone) && (input < analog_deadzone))
		return 0.0;

	return static_cast<float>(input) / 128.0;
}

void navigation::poll_analog(int* x, int* y)
{
	static int hires_x, hires_y;

	int analog_rx = (CELL_PAD_BTN_OFFSET_ANALOG_RIGHT_X) & 0xFF;
	int analog_ry = (CELL_PAD_BTN_OFFSET_ANALOG_RIGHT_Y) & 0xFF;

	int lx = (pad.button[analog_rx] - 127) * 256;
	int ly = (pad.button[analog_ry] - 127) * 256;
	IN_RescaleAnalog(&lx, &ly, 7680);
	hires_x += lx;
	hires_y += ly;
	if (hires_x != 0 || hires_y != 0) {
		// slow down pointer, could be made user-adjustable
		int slowdown = 2048;
		*x += hires_x / slowdown;
		*y += hires_y / slowdown;
		hires_x %= slowdown;
		hires_y %= slowdown;
	}
}