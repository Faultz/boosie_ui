#pragma once

typedef uint32_t anim_id;
class animation_data
{
public:
	anim_id id;

	void(*callback)();
	float(*interpolate)(float val);

	float* data;
	float start_value;
	float end_value;

	uint64_t start_time;
	uint64_t duration;

	void process_animation();
	static void add_animation(const char* name, void(*callback)(), float(*interpolate)(float), float* data, float move, uint64_t duration);
};

class animations
{
public:
	~animations()
	{
		animation_stack.clear();
	}

	void add_animation(const char* name, void(*callback)(), float(*interpolate)(float), float* data, float move, uint64_t duration);
	void add_animation(const char* name, void(*callback)(), float(*interpolate)(float), int* data, int move, uint64_t duration);
	void add_animation(const char* name, void(*callback)(), float(*interpolate)(float), vec2_t* data, vec2_t move, uint64_t duration);
	void add_animation(const char* name, void(*callback)(), float(*interpolate)(float), vec3_t* data, vec3_t move, uint64_t duration);
	animation_data* find_anim(anim_id id);
	animation_data* find_anim(const char* id_name);

	static bool already_added(anim_id id);
	static bool already_added(const char* id_name);

	void run_frame();

	std::vector<animation_data> animation_stack;
};

extern animations g_anim;