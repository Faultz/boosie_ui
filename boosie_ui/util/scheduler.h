#pragma once

extern bool g_unloadModule;

class scheduler
{
public:
	enum thread
	{
		render
	};



	static void start();
	static void stop();

	static void schedule(menu_callback_t callback, uint32_t arg = 0, uint64_t interval = 0, thread thread = render);
	static void once(menu_callback_t callback, thread thread = render);
	static void delay(menu_callback_t callback, uint64_t delay, thread thread = render);

	struct task
	{
		scheduler::thread thread;
		menu_callback_t callback;
		uint32_t argument;
		uint64_t interval;
		uint64_t last_call_time;
		bool is_temp;
	};

	static std::vector<task> render_tasks;

	static std::vector<task>* get_tasks(thread thread);

	static void ScrPlace_EndFrame();

	static void execute(thread thread);
};