#pragma once

extern bool g_unloadModule;

class scheduler
{
public:
	enum thread
	{
		render_foreground,
		render_background
	};



	static void start();
	static void stop();

	static void schedule(menu_callback_t callback, uint64_t arg = 0, uint64_t interval = 0, thread thread = render_foreground);
	static void once(menu_callback_t callback, thread thread = render_foreground);
	static void delay(menu_callback_t callback, uint64_t delay, thread thread = render_foreground);

	struct task
	{
		scheduler::thread thread;
		menu_callback_t callback;
		uint64_t argument;
		uint64_t interval;
		uint64_t last_call_time;
		bool is_temp;
	};

	static std::vector<task> main_tasks;
	static std::vector<task> render_bkg_tasks;
	static std::vector<task> render_fore_tasks;

	static std::vector<task>* get_tasks(thread thread);

	static void ScrPlace_EndFrame();
	static void RB_ExecuteRenderCommandsLoop(const void* cmd);

	static void execute(thread thread);
};