#include "stdafx.h"

#define clamp(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

animations g_anim;

void animation_data::process_animation()
{
	auto time_now = get_time();
	if (time_now < start_time) return;

	auto end_time = static_cast<uint64_t>(start_time + duration);

	if (time_now < end_time)
	{
		auto time_passed = static_cast<uint64_t>(time_now - start_time);
		auto delta_time = static_cast<float>(time_passed) / static_cast<float>(duration); // should be between 0.0 and 1.0
		auto progress = (end_value - start_value) * interpolate(delta_time);
		
		*data = start_value + progress;
	}

	if (get_time() >= end_time)
	{
		*data = end_value;

		for (int i = 0; i < g_anim.animation_stack.size(); i++)
		{
			if (this->id == g_anim.animation_stack[i].id)
			{
				auto callback = g_anim.animation_stack[i].callback;
				if (callback)
					callback();

				g_anim.animation_stack.erase(g_anim.animation_stack.begin() + i);
			}
		}
	}
}

void animation_data::add_animation(const char* name, void(*callback)(), float(*interpolate)(float), float* data, float move, uint64_t duration)
{
	if (animations::already_added(name))
		return;

	animation_data newAnim = animation_data();
	newAnim.id = hash(name);
	newAnim.interpolate = interpolate;
	newAnim.start_time = get_time();
	newAnim.duration = duration;

	newAnim.data = data;
	newAnim.start_value = *data;
	newAnim.end_value = move;

	newAnim.callback = callback;

	g_anim.animation_stack.push_back(newAnim);
}

void animations::add_animation(const char* name, void(*callback)(), float(*interpolate)(float), float* data, float move, uint64_t duration)
{
	if (*data == move || interpolate == nullptr) return;

	animation_data::add_animation(name, callback, interpolate, (float*)data, move, duration);
}

void animations::add_animation(const char* name, void(*callback)(), float(*interpolate)(float), int* data, int move, uint64_t duration)
{
	if (*data == move || interpolate == nullptr) return;

	animation_data::add_animation(name, callback, interpolate, (float*)data, move, duration);
}

void animations::add_animation(const char* name, void(*callback)(), float(*interpolate)(float), vec2_t* data, vec2_t move, uint64_t duration)
{
	if (*data == move || interpolate == nullptr) return;

	animation_data::add_animation(fmt_str("%s_1", name), callback, interpolate, (float*)&data->x, move.x, duration);
	animation_data::add_animation(fmt_str("%s_2", name), callback, interpolate, (float*)&data->y, move.y, duration);
}

void animations::add_animation(const char* name, void(*callback)(), float(*interpolate)(float), vec3_t* data, vec3_t move, uint64_t duration)
{
	if (*data == move || interpolate == nullptr) return;

	animation_data::add_animation(fmt_str("%s_1", name), callback, interpolate, (float*)&data->x, move.x, duration);
	animation_data::add_animation(fmt_str("%s_2", name), callback, interpolate, (float*)&data->y, move.y, duration);
	animation_data::add_animation(fmt_str("%s_3", name), callback, interpolate, (float*)&data->z, move.z, duration);
}

animation_data* animations::find_anim(anim_id id)
{
	for (auto& anim : g_anim.animation_stack)
		if (anim.id == id)
			return &anim;

	return nullptr;
}

animation_data* animations::find_anim(const char* id_name)
{
	const auto& hashed_id = hash(id_name);
	for (auto& anim : g_anim.animation_stack)
		if (anim.id == hashed_id)
			return &anim;

	return nullptr;
}

bool animations::already_added(anim_id id)
{
	for (int i = 0; i < g_anim.animation_stack.size(); i++)
	{
		if (g_anim.animation_stack[i].id == id)
			return true;
	}

	return false;
}

bool animations::already_added(const char* id_name)
{
	for (int i = 0; i < g_anim.animation_stack.size(); i++)
	{
		if (g_anim.animation_stack[i].id == hash(id_name))
			return true;
	}

	return false;
}

void animations::run_frame()
{
	for (animation_data anim : animation_stack)
		anim.process_animation();
}