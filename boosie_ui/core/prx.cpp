#include "stdafx.h"

SYS_MODULE_INFO(BoosieUI, 0, 1, 1);
SYS_MODULE_START(boosie_ui_entry);
SYS_MODULE_STOP(boosie_ui_exit);

#define GET_RESOLUTION(x)  { \
	CellVideoOutState videoState; \
	cellVideoOutGetState(CELL_VIDEO_OUT_PRIMARY, 0, &videoState); \
	cellVideoOutGetResolution(videoState.displayMode.resolutionId, &x); } \

float ASPECT_RATIO = 1.5f;

uint64_t globals::tick;
float globals::delta_time;
float globals::globalAlpha;
bool globals::exitModule;
Material* globals::gameWhite;
Font* globals::fontNormal;
CellVideoOutResolution globals::resolution;
GColor globals::clr_white;
symbol<int(char*, size_t, const char*, ...)> globals::game_snprintf;
symbol<int(char*, size_t, const char*, std::_Va_list)> globals::game_vsnprintf;

extern "C"
{
	int boosie_ui_entry(void)
	{
		globals::gameWhite = Material_RegisterHandle("white", 7);
		globals::fontNormal = R_RegisterFont("fonts/normalFont", 7);
		globals::clr_white = GColor(255, 255, 255, 255);
		globals::globalAlpha = 0.f;
		globals::game_snprintf = { 0x027E2F8 };
		globals::game_vsnprintf = { 0x04AA890, TOC_2 };

		if (globals::resolution.width == 1280 && globals::resolution.height == 720)
			ASPECT_RATIO = 0.0f;

		sys_memory_info info;
		sys_memory_get_user_memory_size(&info);

		GET_RESOLUTION(globals::resolution);

		menu::start();
		scheduler::start();
		renderables::start();

		return SYS_PRX_RESIDENT;
	}

	int boosie_ui_exit(void)
	{
		globals::exitModule = true;

		scheduler::stop();

		sleep(300);

		renderables::stop();
		menu::stop();

		return SYS_PRX_RESIDENT;
	}
}
