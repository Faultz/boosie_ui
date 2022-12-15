#include "stdafx.h"

logging g_log;
std::deque<log_data> log_queue;

std::string logging::get_log_level(int level)
{
	switch (level)
	{
	case log_level::LOG_NORMAL:
		return "[^2Normal^7] ";
	case log_level::LOG_ERROR:
		return "[^1Error^7] ";
	case log_level::LOG_CRIT:
		return "[^6Critical^7] ";
	}

	return "[None] ";
}

void logging::print(const char* text, ...)
{
	if (size() > 10)
		log_queue.pop_front();

	static char buffer[0x2000];
	va_list va;
	va_start(va, text);
	vsprintf(buffer, text, va);

	log_data _log = { get_log_level(LOG_ERROR) + buffer, LOG_ERROR, 15.f, 0.f, static_cast<unsigned int>(get_time()), 1000, false };

	_log.y = static_cast<float>(globals::resolution.height) - 301 - 15;

	if (strlen(buffer) >= 256)
		return;

	log_queue.push_back(_log);
}

void logging::draw_log_text(GRect& log_rect)
{
	for (int i = 0; i < log_queue.size(); i++)
	{
		log_data& _log = log_queue[i];

		auto text_height = render::get_text_height(_log.message.data(), .6f);

		_log.y = easing::lerp(_log.y, ((log_rect.y + log_rect.h) - text_height) - (i * text_height) - 5.f, easing::in_bounce(0.4f));
		_log.alpha = easing::lerp(_log.alpha, _log.expired ? 0.f : 1.f, .03f);

		GRect log_data_rect{ log_rect.x + 5.f, _log.y, log_rect.w, text_height };


		if (get_time() - _log.start_time > 4400)
		{
			_log.expired = true;
		}

		if (_log.expired && _log.alpha <= 0.4)
		{
			log_queue.erase(log_queue.begin() + i);
			continue;
		}

		render::add_text(_log.message.data(), log_data_rect, 0, horz_left | vert_center, .6f, .6f, color(255, 255, 255, _log.alpha));
	}
}

void logging::run()
{
	//if (log_queue.empty() || g_log.enabled == false)
	//	return;

	const int LOG_BOX_HEIGHT = 301;

	GRect log_rect{ 15.f, static_cast<float>(globals::resolution.height) - LOG_BOX_HEIGHT - 15, 451, LOG_BOX_HEIGHT };
	GRect log_highlight_rect{ 15.f - .5f, (static_cast<float>(globals::resolution.height) - LOG_BOX_HEIGHT - 15) - .5f, 453, LOG_BOX_HEIGHT + 2 };

	GColor log_background_color{ 15, 15, 15, 255 };

	render::add_filled_rect(log_rect, log_background_color);

	draw_log_text(log_rect);
}

int logging::size()
{
	return log_queue.size();
}
