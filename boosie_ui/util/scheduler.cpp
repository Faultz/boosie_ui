#include "stdafx.h"

bool g_unloadModule;
std::vector<scheduler::task> scheduler::render_tasks;

std::vector<scheduler::task>* scheduler::get_tasks(thread thread)
{
	if (thread == thread::render) return &render_tasks;
	else return {};
}

void scheduler::schedule(menu_callback_t callback, uint32_t arg, uint64_t interval, scheduler::thread thread)
{
	scheduler::task task;
	task.argument = arg;
	task.callback = callback;
	task.interval = interval;
	task.thread = thread;
	task.last_call_time = get_time();
	task.is_temp = false;

	get_tasks(thread)->push_back(task);
}

void scheduler::once(menu_callback_t callback, thread thread)
{
	scheduler::task task;
	task.callback = callback;
	task.interval = 0;
	task.thread = thread;
	task.last_call_time = get_time();
	task.is_temp = true;

	get_tasks(thread)->push_back(task);
}

void scheduler::delay(menu_callback_t callback, uint64_t delay, thread thread)
{
	scheduler::task task;
	task.callback = callback;
	task.interval = delay;
	task.thread = thread;
	task.last_call_time = get_time();
	task.is_temp = true;

	get_tasks(thread)->push_back(task);
}


void scheduler::execute(thread thread)
{
	if (g_unloadModule) return;

	auto tasks = get_tasks(thread);
	for (int i = 0; i < tasks->size(); i++)
	{
		auto task = tasks->at(i);

		const auto diff = get_time() - task.last_call_time;
		if (diff < task.interval)
			continue;

		task.last_call_time = get_time();
		task.callback((void*)task.argument);

		if (task.is_temp)
		{
			tasks->erase(tasks->begin() + i);
			--i;
		}
	}
}

detour* ScrPlace_EndFrame_d = nullptr;
void scheduler::ScrPlace_EndFrame()
{
	scheduler::execute(scheduler::thread::render);
	ScrPlace_EndFrame_d->invoke();
}

void scheduler::start()
{
	ScrPlace_EndFrame_d = new detour(R_EndFrame_t, ScrPlace_EndFrame);
}

void scheduler::stop()
{
	delete ScrPlace_EndFrame_d;
}