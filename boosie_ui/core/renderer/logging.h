#pragma once

enum log_level
{
	LOG_NORMAL = 0,
	LOG_ERROR = 1,
	LOG_CRIT = 2,
};

struct log_data
{
	std::string message;
	log_level level;
	float y;
	float alpha;
	unsigned int start_time;
	unsigned int fade_out_interval;
	bool expired;
};

extern std::deque<log_data> log_queue;

class logging
{
public:
	std::string get_log_level(int level);

	void print(const char* text, ...);
	void draw_log_text(GRect& log_rect);
	void run();
	int size();

	bool enabled;
	int grace_period;
};

extern logging g_log;