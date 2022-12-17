#include "stdafx.h"

menu_context* gMenuCtx;

GRect navScoringRect, navResultRect;

draw_list* menu::overlay_drawlist;

static menu_nav_dir navScoreItemQuadrant(float dx, float dy)
{
	if (fabsf(dx) > fabsf(dy))
		return (dx > 0.0f) ? NAV_DIR_RIGHT : NAV_DIR_LEFT;
	return (dy > 0.0f) ? NAV_DIR_DOWN : NAV_DIR_UP;
}

static float inline navScoreItemDist(float a0, float a1, float b0, float b1)
{
	if (a1 < b0)
		return a1 - b0;
	if (b1 < a0)
		return a0 - b1;
	return 0.0f;
}

bool navScoreItem(menu_nav_results* result) {
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	GRect scoring_rect = navScoringRect;

	if (g.lastItemData.Rect == scoring_rect)
		return false;

	GBounds clip(window.innerClipRect);
	GBounds cand(g.lastItemData.Rect);
	GBounds navScoring(scoring_rect);

	cand.ClipWithFull(clip);

	cand.Min.y = clamp(cand.Min.y, clip.Min.y, clip.Max.y);
	cand.Max.y = clamp(cand.Max.y, clip.Min.y, clip.Max.y);

	// Compute distance between boxes
	  // FIXME-NAV: Introducing biases for vertical navigation, needs to be removed.
	float dbx = navScoreItemDist(cand.Min.x, cand.Max.x, navScoring.Min.x, navScoring.Max.x);
	float dby = navScoreItemDist(easing::lerp(cand.Min.y, cand.Max.y, 0.2f), easing::lerp(cand.Min.y, cand.Max.y, 0.8f),
		easing::lerp(navScoring.Min.y, navScoring.Max.y, 0.2f), easing::lerp(navScoring.Min.y, navScoring.Max.y, 0.8f));
	if (dby != 0.0f && dbx != 0.0f)
		dbx = (dbx / 1000.0f) + ((dbx > 0.0f) ? +1.0f : -1.0f);
	float dist_box = fabsf(dbx) + fabsf(dby);

	// Compute distance between centers (this is off by a factor of 2, but we only compare center distances with each other so it doesn't matter)
	float dcx = (cand.Min.x + cand.Max.x) - (navScoring.Min.x + navScoring.Max.x);
	float dcy = (cand.Min.y + cand.Max.y) - (navScoring.Min.y + navScoring.Max.y);
	float dist_center = fabsf(dcx) + fabsf(dcy); // L1 metric (need this for our connectedness guarantee)

	// Determine which quadrant of 'curr' our candidate item 'cand' lies in based on distance
	menu_nav_dir quadrant = NAV_DIR_NONE;
	float dax = 0.0f, day = 0.0f, dist_axial = 0.0f;
	if (dbx != 0.0f || dby != 0.0f)
	{
		// For non-overlapping boxes, use distance between boxes
		dax = dbx;
		day = dby;
		dist_axial = dist_box;
		quadrant = navScoreItemQuadrant(dbx, dby);
	}
	else if (dcx != 0.0f || dcy != 0.0f)
	{
		// For overlapping boxes with different centers, use distance between centers
		dax = dcx;
		day = dcy;
		dist_axial = dist_center;
		quadrant = navScoreItemQuadrant(dcx, dcy);
	}

	//auto fmt = fmt_str_n<1028>("dbox (%.2f,%.2f->%.4f)\ndcen (%.2f,%.2f->%.4f)\nd (%.2f,%.2f->%.4f)\nnav %c, quadrant %c", dbx, dby, dist_box, dcx, dcy, dist_center, dax, day, dist_axial, "WENS"[g.moveNavDir], "WENS"[quadrant]);
	//render::add_text(fmt, cand, 1, vert_center | horz_center, 0.5f, 0.5f * 1.5, color(255, 255, 255));

	bool new_best = false;
	if (quadrant == g.moveNavDir)
	{
		// Does it beat the current best candidate?
		if (dist_box < result->distBox)
		{
			result->distBox = dist_box;
			result->distCenter = dist_center;
			return true;
		}
		if (dist_box == result->distBox)
		{
			// Try using distance between center points to break ties
			if (dist_center < result->distCenter)
			{
				result->distCenter = dist_center;
				new_best = true;
			}
			else if (dist_center == result->distCenter)
			{
				// Still tied! we need to be extra-careful to make sure everything gets linked properly. We consistently break ties by symbolically moving "later" items 
				// (with higher index) to the right/downwards by an infinitesimal amount since we the current "best" button already (so it must have a lower index), 
				// this is fairly easy. This rule ensures that all buttons with dx==dy==0 will end up being linked in order of appearance along the x axis.
				if (((g.moveNavDir == NAV_DIR_UP || g.moveNavDir == NAV_DIR_DOWN) ? dby : dbx) < 0.0f) // moving bj to the right/down decreases distance
					new_best = true;
			}
		}
	}

	// Axial check: if 'curr' has no link at all in some direction and 'cand' lies roughly in that direction, add a tentative link. This will only be kept if no "real" matches
	// are found, so it only augments the graph produced by the above method using extra links. (important, since it doesn't guarantee strong connectedness)
	// This is just to avoid buttons having no links in a particular direction when there's a suitable neighbor. you get good graphs without this too.
	// 2017/09/29: FIXME: This now currently only enabled inside menu bars, ideally we'd disable it everywhere. Menus in particular need to catch failure. For general navigation it feels awkward.
	// Disabling it may however lead to disconnected graphs when nodes are very spaced out on different axis. Perhaps consider offering this as an option?
	if (result->distBox == FLT_MAX && dist_axial < result->distAxial)  // Check axial match
		if ((g.moveNavDir == NAV_DIR_LEFT && dax < 0.0f) || (g.moveNavDir == NAV_DIR_RIGHT && dax > 0.0f) || (g.moveNavDir == NAV_DIR_UP && day < 0.0f) || (g.moveNavDir == NAV_DIR_DOWN && day > 0.0f))
		{
			result->distAxial = dist_axial;
			new_best = true;
		}


	return new_best;
}

detour* cell_pad_get_data_d = nullptr;
int cellPad_GetData(uint32_t port_no, CellPadData* data)
{
	CellPadData tmpCellPad = { 0 };

	tmpCellPad.len = data->len;
	cellPadGetData(port_no, &tmpCellPad);
	memcpy((void*)data, (void*)&tmpCellPad, sizeof(CellPadData));

	if (data->len > 0 && port_no == 0)
		memcpy(&g_nav.pad, &tmpCellPad, sizeof(CellPadData));	
	
	if (data->len > 0 && gMenuCtx->open)
	{
		data->button[CELL_PAD_BTN_OFFSET_DIGITAL1] &= ~(CELL_PAD_CTRL_UP | CELL_PAD_CTRL_DOWN | CELL_PAD_CTRL_LEFT |
			CELL_PAD_CTRL_RIGHT);
		data->button[CELL_PAD_BTN_OFFSET_DIGITAL2] &= ~(CELL_PAD_CTRL_CROSS | CELL_PAD_CTRL_CIRCLE);
	}

	return 0;
}

void menu::create_context()
{
	gMenuCtx = new menu_context();
	overlay_drawlist = new draw_list();
}

void menu::destroy_context()
{
	delete gMenuCtx;
	delete overlay_drawlist;
}

void menu::set_window_pos(vec2_t pos)
{
	menu_context& g = *gMenuCtx;
	g.rect.x = pos.x;
	g.rect.y = pos.y;
}

void menu::set_window_size(vec2_t size)
{
	menu_context& g = *gMenuCtx;
	g.rect.w = size.x;
	g.rect.h = size.y;
}

void menu::set_window_pos(float x, float y)
{
	menu_context& g = *gMenuCtx;
	g.rect.x = x;
	g.rect.y = y;
}

void menu::set_window_size(float w, float h)
{
	menu_context& g = *gMenuCtx;
	g.rect.w = w;
	g.rect.h = h;
}

vec2_t menu::calc_next_scroll_and_clamp(menu_window* window)
{
	vec2_t scroll = window->scrollPos;
	if (window->scrollTarget.y < FLT_MAX)
	{
		scroll.y = window->scrollTarget.y;
		scroll.y = clamp(scroll.y, 0.0f, window->contentSize.y - window->innerClipRect.h);
	}

	return scroll;
}

void menu::calc_content_size(menu_window* window, vec2_t* content_size, vec2_t* content_size_ideal)
{
	content_size->x = floorf(window->cursorMaxPos.x - window->cursorStartPos.x);
	content_size->y = floorf(window->cursorMaxPos.y - window->cursorStartPos.y);
}

void menu::set_scroll_from_y(menu_window* window, float local_y, float center_y_ratio)
{
	window->scrollTarget.y = floorf(local_y + window->scrollPos.y);
}

vec2_t menu::scroll_to_rect(menu_window* window, const GBounds& item_rect)
{
	menu_context& g = *gMenuCtx;

	GBounds clip_bb(window->innerClipRect);

	const bool can_be_fully_visible_y = item_rect.GetHeight() + (g.style.itemSpacing.y * 2.0f) <= clip_bb.GetHeight();
	float target_y = can_be_fully_visible_y ? floorf((item_rect.Min.y + item_rect.Max.y - clip_bb.GetHeight()) * 0.5f) : item_rect.Min.y;
	set_scroll_from_y(window, item_rect.Max.y - window->Pos.y, 0.0f);

	vec2_t next_scroll = calc_next_scroll_and_clamp(window);
	vec2_t delta_scroll = next_scroll - window->scrollPos;

	return delta_scroll;
}

menu_window* menu::create_window(int id)
{
	menu_context& g = *gMenuCtx;
	menu_style& style = g.style;

	const int headerHeight = 20 * ASPECT_RATIO;

	GRect window_rect(GRect(g.rect.x, g.rect.y + headerHeight, g.rect.w, g.rect.h - headerHeight));

	g.windowMap[id] = new menu_window();
	auto* created_window = g.windowMap[id];
	
	created_window->Pos = vec2_t(window_rect.x, window_rect.y);
	created_window->windowPadding = vec2_t(7.0f, 7.0f);
	created_window->innerClipRect = GRect(g.rect.x, g.rect.y, g.rect.w, g.rect.h);
	created_window->itemFlags = ITEM_FLAG_NONE;
	created_window->globalFontScale = 0.5f;
	created_window->globalFontScaleY = 0.5f;

	return created_window;
}

void menu::set_window_scroll_y(menu_window* window, float scroll)
{
	window->cursorMaxPos.y -= window->scrollPos.y;
	window->scrollTarget.y = scroll;
	window->cursorMaxPos.y += window->scrollPos.y;
}

void menu::new_frame()
{
	static uint64_t prev_time;

	constexpr uint64_t frequency = 1000000;

	menu_context& g = *gMenuCtx;

	navScoringRect = navResultRect;

	if (g.navResults.menuId != 0)
	{
		g.activeIdLast = g.activeId;
		g.activeId = g.navResults.menuId;
	}

	uint64_t currentTime = clock();
	g.deltaTime = prev_time > 0 ? static_cast<float>((double)(currentTime - prev_time) / frequency) : (float)(1.0f / 60.0f);
	prev_time = currentTime;

	g.time += g.deltaTime;
	g.frameCount += 1;

	if (g.currentWindow)
	{
		if (g.moveNavDir == NAV_DIR_DOWN || g.moveNavDir == NAV_DIR_UP && g.currentWindow->contentSize.y > g.currentWindow->innerClipRect.h)
		{
			menu_nav_results* results = &g.navResults;

			GBounds rect_abs = window_rect_rel_to_abs(g.currentWindow, results->rectRel);
			scroll_to_rect(g.currentWindow, rect_abs);
		}
	}

	g.moveNavDir = NAV_DIR_NONE;
	g.navResults.clear();

	if (g.currentWindow)
	{
		menu_window& window = *g.currentWindow;

		calc_content_size(&window, &window.contentSize, nullptr);
		static analog_input_t analog;
		read_analog_input(&analog);

		const float scroll_speed = (16.0f * 100 * g.deltaTime + 0.5f);
		const auto s = g_nav.is_down(NAV_RSTICK_UP) - g_nav.is_down(NAV_RSTICK_DOWN);

		if (analog.analogRight.y != 0.0f && window.contentSize.y > window.innerClipRect.h)
		{
			set_window_scroll_y(&window, (window.scrollPos.y + s * scroll_speed));
		}

		window.scrollPos = calc_next_scroll_and_clamp(&window);
		window.scrollTarget = vec2_t(FLT_MAX, FLT_MAX);

	}

	if (!g.isActive)
	{
		if (is_down(NAV_DPAD_DOWN, 1.0)) g.createNavRequest(NAV_DIR_DOWN);
		if (is_down(NAV_DPAD_UP, 1.0)) g.createNavRequest(NAV_DIR_UP);
		if (is_down(NAV_DPAD_LEFT, 1.0)) g.createNavRequest(NAV_DIR_LEFT);
		if (is_down(NAV_DPAD_RIGHT, 1.0)) g.createNavRequest(NAV_DIR_RIGHT);
	}

	g.startNavRequest();

	begin("boosie ui");
}

menu_id menu::get_id(const char* label)
{
	menu_context& g = *gMenuCtx;
	return g.idStack.empty() ? hash(label) : g.idStack.back();
}

menu_window* menu::get_current_window()
{
	menu_context& g = *gMenuCtx; return g.currentWindow;
}

void menu::begin(const char* label)
{
	menu_context& g = *gMenuCtx;
	menu_style& style = g.style;

	auto id = get_id(label);

	const int headerHeight = 20 * ASPECT_RATIO;

	GRect clip_rect(GRect(g.rect.x, g.rect.y + headerHeight, g.rect.w, g.rect.h - headerHeight));

	menu_window* window = g.windowMap[id]; 
	if (!window)
	{
		window = create_window(id);
	}

	if (!window)
		return;

	window->indentSize = 0.0f + window->windowPadding.x;

	window->cursorStartPos = vec2_t(window->Pos.x + window->windowPadding.x - window->scrollPos.x, window->Pos.y + window->windowPadding.y - window->scrollPos.y);
	window->cursorPos = window->cursorStartPos;
	window->cursorPosPrevLine = window->cursorPos;
	window->cursorMaxPos = window->cursorStartPos;
	window->currLineSize = window->prevLineSize = vec2_t(0.0f, 0.0f);
	window->isSameLine = false;

	if (g.currentWindow)
	{
		menu_window& window = *g.currentWindow;

		navScoringRect.x = window.rectRel.x;
		navScoringRect.y = window.rectRel.y;
	}

	GRect header_rect(g.rect.x, g.rect.y, g.rect.w, headerHeight);
	GRect headertext_rect(g.rect.x + window->windowPadding.x, g.rect.y, g.rect.w - 10.f, headerHeight);

	g.currentWindow = window;

	render::add_filled_rect(window->innerClipRect, style.colors[COL_BACKGROUND]);

	render::add_filled_rect(header_rect, style.colors[COL_ITEM_BACKGROUND]);
	render::add_text(label, headertext_rect, 1, vert_center | horz_left, .5f, .5f * ASPECT_RATIO, style.colors[COL_TEXT]);

	render::push_clip(clip_rect);
}

void menu::end()
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	window.cursorMaxPos.x = window.cursorPos.x;
	window.cursorMaxPos.y = window.cursorPos.y + 50.0f;

	//render::add_filled_rect(GRect(window.cursorMaxPos.x, window.cursorMaxPos.y, 300, 3), g.style.colors[COL_ITEM_BACKGROUND]);

	render::pop_clip();
}

bool menu::begin_tab_bar(const char* name)
{
	menu_context& g = *gMenuCtx;
	menu_style& style = g.style;

	auto id = get_id(name);

	menu_window* window = g.currentWindow;
	if (!window)
		return false;

	auto current_tab = window->tabMap[id];
	if (!current_tab)
	{
		window->tabMap[id] = new menu_tab_bar();
		return false;
	}

	g.currentTabBar = current_tab;

	GRect rect(window->cursorPos.x - window->windowPadding.x, window->cursorPos.y, window->innerClipRect.w, 20 * ASPECT_RATIO);
	GRect divider_line(rect.x, rect.y + rect.h - 2, rect.w, 2);

	render::add_filled_rect(divider_line, style.colors[COL_ITEM_BACKGROUND]);

	for (int i = 0; i < current_tab->tabItems.size(); i++)
	{
		if(i > 0)
			same_line(0, g.style.framePadding.x);

		auto tab_item = current_tab->tabItems.find_by_index(i);

		auto tab_id = get_id(fmt_str("%s#tab_item", tab_item->name, i));

		auto tab_name_width = render::get_text_width(tab_item->name, .5f) + 10;

		GRect tab_name_rect(window->cursorPos.x, window->cursorPos.y, tab_name_width, rect.h);

		vec2_t size = vec2_t(tab_name_width, rect.h);

		item_size(size);
		if (!item_add(GRect(tab_name_rect.x, tab_name_rect.y, tab_name_rect.w, tab_name_rect.h), tab_id))
			return false;

		const bool active = g.activeId == tab_id;

		if (active && g_nav.is_pressed(NAV_ACTIVATE))
		{
			current_tab->activeTabIndex = i;
			current_tab->activeTabId = tab_id;
			current_tab->currentTabItem = tab_item;
		}

		render::add_filled_rect(tab_name_rect, current_tab->activeTabId == tab_id ? style.colors[COL_ITEM_ACTIVE] : style.colors[COL_ITEM_BACKGROUND]);
		if (active) render::add_rect(tab_name_rect, 1, style.colors[COL_ACTIVE]);
		render::add_text(tab_item->name, tab_name_rect, 1, vert_center | horz_center, .5f, .5f * ASPECT_RATIO, style.colors[COL_TEXT]);
	}

	return true;
}

void menu::end_tab_bar()
{
}

bool menu::begin_tab_item(const char* name)
{
	menu_context& g = *gMenuCtx;

	menu_window* window = g.currentWindow;
	if (!window)
		return false;

	auto current_tab = g.currentTabBar;
	if (!current_tab)
		return false;

	auto id = get_id(fmt_str("%s#tab_item", name));

	auto current_tab_item = current_tab->tabItems[id];
	if (!current_tab_item)
	{
		current_tab->tabItems[id] = new menu_tab_item();
		strcpy(current_tab->tabItems[id]->name, name);
		return false;
	}

	if (!current_tab_item)
		return false;

	if (current_tab->activeTabId == id && current_tab->currentTabItem == current_tab_item)
	{
		return true;
	}

	return false;
}

void menu::end_tab_item()
{
}

void menu::set_active_id(int id)
{
}

bool menu::item_add(GRect rect, int id)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	if (id != 0)
	{
		GBounds navBounds(rect);
		GBounds navRectRel(navBounds.Min - window.Pos, navBounds.Max - window.Pos);

		g.lastItemData.ID = id;
		g.lastItemData.Rect = rect;
		g.lastItemData.rectRel = navBounds;

		if (g.navInitRequest && g.activeId != id)
		{
			g.navInitRequest = false;
			g.navResults.menuId = id;
			g.navResults.navRect = rect;
			navResultRect = rect;
			window.rectRel = rect;
		}

		if (g.activeId != id && !(window.itemFlags & ITEM_FLAG_NO_NAV))
		{
			bool newBest = g.navRequest && navScoreItem(&g.navResults);
			if (newBest)
			{
				g.navResults.menuId = id;
				g.navResults.rectRel = window_rect_rel_to_abs(&window, navRectRel);
				g.navResults.navRect = rect;
				navResultRect = rect;
			}
		}

		if (g.activeId == id)
		{
			window.rectRel = rect;
		}
	}

	if (!rect.overlaps(GRect(g.currentWindow->innerClipRect)))
		return false;

	return true;
}

void menu::item_size(vec2_t size, float text_baseline_y)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	const float offset_to_match_baseline_y = (text_baseline_y >= 0) ? std::max(0.0f, window.currLineTextBaseOffset - text_baseline_y) : 0.0f;

	const float line_y1 = window.isSameLine ? window.cursorPosPrevLine.y : window.cursorPos.y;
	const float line_height = std::max(window.currLineSize.y, window.cursorPos.y - line_y1 + size.y + offset_to_match_baseline_y);

	window.cursorPosPrevLine.x = window.cursorPos.x + size.x;
	window.cursorPosPrevLine.y = line_y1;
	window.cursorPos.x = floorf(window.Pos.x + window.indentSize);
	window.cursorPos.y = floorf(line_y1 + line_height + g.style.itemSpacing.y);
	window.cursorMaxPos.x = std::max(window.cursorMaxPos.x, window.cursorPosPrevLine.x);
	window.cursorMaxPos.y = std::max(window.cursorMaxPos.y, window.cursorPos.y - g.style.itemSpacing.y);
	window.cursorPrevMove = size;
	
	window.prevLineSize.y = line_height;
	window.currLineSize.y = 0.0f;
	window.prevLineTextBaseOffset = std::max(window.currLineTextBaseOffset, text_baseline_y);
	window.isSameLine = false;
}

void menu::same_line(float offset_from_start_x, float spacing_w)
{
	menu_context& g = *gMenuCtx;
	menu_window* window = g.currentWindow;

	if (offset_from_start_x != 0.0f)
	{
		if (spacing_w < 0.0f)
			spacing_w = 0.0f;
		window->cursorPos.x = window->Pos.x - window->scrollPos.x + offset_from_start_x + spacing_w;
		window->cursorPos.y = window->cursorPosPrevLine.y;
	}
	else
	{
		if (spacing_w < 0.0f)
			spacing_w = g.style.itemSpacing.x;
		window->cursorPos.x = window->cursorPosPrevLine.x + spacing_w;
		window->cursorPos.y = window->cursorPosPrevLine.y;
	}

	window->currLineSize = window->prevLineSize;
	window->currLineTextBaseOffset = window->prevLineTextBaseOffset;
	window->isSameLine = true;
}

void menu::push_style_color(int idx, GColor color)
{
	menu_context& g = *gMenuCtx;
	menu_col_mod mod;
	mod.idx = idx;
	mod.col = g.style.colors[idx];
	g.colModifiers.push_back(mod);
	g.style.colors[idx] = color;
}

void menu::pop_style_color(int count)
{
	menu_context& g = *gMenuCtx;
	while (count > 0)
	{
		menu_col_mod& mod = g.colModifiers.back();
		g.style.colors[mod.idx] = mod.col;
		g.colModifiers.pop_back();
		--count;
	}
}

void menu::push_item_flags(menu_item_flags flags, bool enabled)
{
	menu_window* window = get_current_window();
	if (enabled)
		window->itemFlags |= (flags);
	else
		window->itemFlags &= ~(flags);
	window->itemFlagStack.push_back(flags);
}

void menu::pop_item_flags()
{
	menu_window* window = get_current_window();
	window->itemFlagStack.pop_back();
	window->itemFlags = window->itemFlagStack.empty() ? ITEM_FLAG_NONE : window->itemFlagStack.back();
}

void menu::push_font_scale(float scale, float scale_y)
{
	menu_window* window = get_current_window();
	menu_font_mod mod;
	mod.backupScale = window->globalFontScale;
	mod.backupScaleY = window->globalFontScaleY;

	window->globalFontScale = scale;
	if (scale_y != 0.0f)
		window->globalFontScaleY = scale_y;

	window->fontScaleStack.push_back(mod);
}

void menu::pop_font_scale()
{
	menu_window* window = get_current_window();
	window->fontScaleStack.pop_back();
	menu_font_mod& mod = window->fontScaleStack.back();
	window->globalFontScale = window->fontScaleStack.empty() ? 0.5f : mod.backupScale;
	if(mod.backupScaleY != 0.0f)
		window->globalFontScaleY = window->fontScaleStack.empty() ? 0.5f : mod.backupScaleY;
}

void menu::push_id(const char* name)
{
	menu_context& g = *gMenuCtx;
	menu_window* window = get_current_window();
	auto id = hash(name);
	g.lastId = id;
	g.idStack.push_back(id);
}

void menu::push_id(int int_id)
{
	menu_context& g = *gMenuCtx;
	menu_window* window = get_current_window();
	auto id = hash((char*)&int_id);
	g.lastId = id;
	g.idStack.push_back(id);
}

void menu::pop_id()
{
	menu_context& g = *gMenuCtx;
	menu_window* window = get_current_window();
	g.idStack.pop_back();
}

vec2_t menu::calc_item_size(vec2_t size, float default_w, float default_h)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	if (size.x == 0.0f)
		size.x = default_w;
	else if (size.x < 0.0f)
		size.x = std::max(4.0f, window.cursorPos.x + size.x);

	if (size.y == 0.0f)
		size.y = default_h;
	else if (size.y < 0.0f)
		size.y = std::max(4.0f, window.cursorPos.y + size.y);

	return size;
}

bool menu::button(const char* label, vec2_t b_size)
{
	menu_context& g = *gMenuCtx;
	menu_style& style = g.style;
	menu_window& window = *g.currentWindow;

	auto id = get_id(label);

	const int height = 10 * ASPECT_RATIO;
	const float fontScale = window.globalFontScale;

	const bool active = g.activeId == id;

	vec2_t size = b_size == vec2_t() ? calc_item_size(vec2_t(), render::get_text_width(label, fontScale) + 10, height + 15) : b_size;

	GRect rect(window.cursorPos.x, window.cursorPos.y, size.x, size.y);

	item_size(size);
	if (!item_add(rect, id))
		return false;

	render::add_filled_rect(rect, style.colors[COL_ITEM_BACKGROUND]);
	if (active) render::add_rect(rect, 1, style.colors[COL_ACTIVE]);
	render::add_text(label, rect, 1, vert_center | horz_center, fontScale, fontScale * ASPECT_RATIO, style.colors[COL_TEXT]);

	if(active)
		if (is_down(NAV_ACTIVATE, 1.f))
		{
			return true;
		}

	return false;
}

bool menu::checkbox(const char* label, bool* value, vec2_t b_size)
{
	menu_context& g = *gMenuCtx;
	menu_style& style = g.style;
	menu_window& window = *g.currentWindow;

	auto id = get_id(label);

	const int height = 10 * ASPECT_RATIO;
	const float fontScale = window.globalFontScale;

	const bool active = g.activeId == id;

	vec2_t size = b_size == vec2_t() ? calc_item_size(vec2_t(), render::get_text_width(label, fontScale) + 22, (height + 10 * (4 /3))) : b_size;

	GRect rect(window.cursorPos.x, window.cursorPos.y, 20, size.y);
	GRect text_rect(window.cursorPos.x + 24, window.cursorPos.y, size.x, size.y);

	item_size(size);
	if (!item_add(rect, id))
		return false;

	render::add_filled_rect(rect, *value ? style.colors[COL_ITEM_SLIDE] : style.colors[COL_ITEM_BACKGROUND]);
	if(active) render::add_rect(rect, 1, style.colors[COL_ACTIVE]);
	render::add_text(label, text_rect, 1, vert_center | horz_left, fontScale, fontScale * ASPECT_RATIO, style.colors[COL_TEXT]);

	if (active)
		if (is_down(NAV_ACTIVATE, 1.f))
		{
			*value ^= true;
			return true;
		}

	return false;
}

void menu::text(const char* label, ...)
{
	menu_context& g = *gMenuCtx;
	menu_style& style = g.style;
	menu_window& window = *g.currentWindow;

	const float fontScale = window.globalFontScale;

	auto id = get_id(label);

	char buffer[512];
	va_list args;
	va_start(args, label);
	vsnprintf(buffer, 512, label, args);
	va_end(args);

	std::string str = fmt_str(label);

	const int height = render::get_text_height(buffer, fontScale) * ASPECT_RATIO;

	vec2_t size = vec2_t(render::get_text_width(buffer, fontScale) + 4.f, height);

	GRect rect(window.cursorPos.x, window.cursorPos.y, size.x, size.y);

	item_size(size);
	item_add(rect, 0);

	render::add_text(buffer, rect, 1, vert_center | horz_left, fontScale, fontScale * ASPECT_RATIO, style.colors[COL_TEXT]);
}

bool menu::slider(const char* label, const char* fmt, float* values, int count, float inc, float min, float max)
{
	menu_context& g = *gMenuCtx;
	menu_style& style = g.style;
	menu_window& window = *g.currentWindow;

	const int height = 10 * ASPECT_RATIO;

	bool modified = false;

	const float fontScale = window.globalFontScale - 0.07;

	vec2_t size = vec2_t(120, 20);

	for (int i = 0; i < count; i++)
	{
		if (i > 0)
			same_line(0, style.framePadding.x);

		size = calc_item_size(vec2_t(), 360 / count, height + 10);

		auto id = get_id(fmt_str("%s_%i", label, i));
		const char* str = fmt_str("%.2f/%.2f", values[i], max);

		const bool active = g.activeId == id;

		GRect rect(window.cursorPos.x, window.cursorPos.y, size.x, size.y);
		GRect rect_calc(window.cursorPos.x, window.cursorPos.y, (((values[i] - min) / (max - min)) * size.x), size.y);

		item_size(vec2_t(size.x, size.y));
		if (!item_add(rect, id))
			return false;

		if (active)
		{
			if (is_down(NAV_ACTIVATE, 1.f))
				g.isActive = true;

			modified = slider_behaviour(values[i], inc, min, max);
		}

		render::add_filled_rect(rect, style.colors[COL_ITEM_BACKGROUND]);
		if (active) render::add_rect(rect, 1, style.colors[COL_TEXT]);
		if (values[i] != min)
			render::add_filled_rect(rect_calc, style.colors[COL_ITEM_SLIDE]);
		render::add_text(str, rect, 1, vert_center | horz_center, fontScale, fontScale * ASPECT_RATIO, style.colors[COL_TEXT]);
	}

	return false;
}

bool menu::slider(const char* label, const char* fmt, int* values, int count, int inc, int min, int max)
{
	return false;
}

bool menu::sliderf(const char* label, float* value, float inc, float min, float max, vec2_t b_size)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	if (!(window.itemFlags & ITEM_FLAG_SLIDER_NO_TEXT))
		text(label);

	return slider(label, "%.2f/%.2f", value, 1, inc, min, max);
}

bool menu::sliderf2(const char* label, float* values, float inc, float min, float max, vec2_t b_size)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	if (!(window.itemFlags & ITEM_FLAG_SLIDER_NO_TEXT))
		text(label);

	return slider(label, "%.2f/%.2f", values, 2, inc, min, max);
}

bool menu::sliderf3(const char* label, float* values, float inc, float min, float max, vec2_t b_size)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	if (!(window.itemFlags & ITEM_FLAG_SLIDER_NO_TEXT))
		text(label);

	return slider(label, "%.2f/%.2f", values, 3, inc, min, max);
}

bool menu::sliderf4(const char* label, float* values, float inc, float min, float max, vec2_t b_size)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	if (!(window.itemFlags & ITEM_FLAG_SLIDER_NO_TEXT))
		text(label);

	return slider(label, "%.2f/%.2f", values, 4, inc, min, max);
}

bool menu::slideru(const char* label, int* value, int inc, int min, int max, vec2_t b_size)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	if (!(window.itemFlags & ITEM_FLAG_SLIDER_NO_TEXT))
		text(label);

	return slider(label, "%i/%i", value, 1, inc, min, max);
}


bool menu::slideru2(const char* label, int* values, int inc, int min, int max, vec2_t b_size)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	if (!(window.itemFlags & ITEM_FLAG_SLIDER_NO_TEXT))
		text(label);

	return slider(label, "%i/%i", values, 2, inc, min, max);
}

bool menu::slideru3(const char* label, int* values, int inc, int min, int max, vec2_t b_size)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	if (!(window.itemFlags & ITEM_FLAG_SLIDER_NO_TEXT))
		text(label);

	return slider(label, "%i/%i", values, 3, inc, min, max);
}

bool menu::slideru4(const char* label, int* values, int inc, int min, int max, vec2_t b_size)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	if (!(window.itemFlags & ITEM_FLAG_SLIDER_NO_TEXT))
		text(label);

	return slider(label, "%i/%i", values, 4, inc, min, max);
}


void menu::end_frame()
{
	menu_context& g = *gMenuCtx;

	overlay_drawlist->render();

	g_nav.end_frame();

	g.stopNavRequest();
}

bool menu::is_down(int id, float repeat)
{
	return g_nav.is_down(id, repeat);
}

bool menu::is_pressed(int id)
{
	return g_nav.is_pressed(id);
}

bool menu::is_released(int id)
{
	return g_nav.is_released(id);
}

void menu::update(void* arg)
{
	g_nav.new_frame();

	//DrawText2D("test",
	//	300, 300, 1.0,
	//	globals::fontNormal, 1.0, 1.0,
	//	0.0, 1.0, GfxColor(0xFFFFFFFF),
	//	0x7FFFFFFF, 0, 0,
	//	0, 0.0, GfxColor(0xFFFFFFFF),
	//	0, 0, 0, 
	//	0, 0, 0, 
	//	0, 0);

	//RB_EndTessSurface();

	menu_context& g = *gMenuCtx;
	menu_style& style = g.style;

	if (!g.open && is_down(NAV_R3) && is_down(NAV_R1))
		g.open = true;

	if (!g.open)
		return;

	if (!g.isActive && is_down(NAV_BACK, 1.f))
		g.open = false;

	new_frame();

	menu_window& window = *get_current_window();

	text("boosie ui by faultz :)");
	text("g.isActive: %i", g.isActive); same_line(); 
	push_style_color(COL_TEXT, GColor(255, 0, 0));
	text("g.activeId: 0x%08X", g.activeId); same_line();
	pop_style_color();
	text("g.navRequest: %i", g.navRequest);

	text("g.rect(%.1f, %.1f, %.1f, %.1f)", g.rect.x, g.rect.y, g.rect.w, g.rect.h);
	text("window.windowPos(%.1f, %.1f)", window.Pos.x, window.Pos.y);
	text("window.itemSpacing(%.1f, %.1f)", style.itemSpacing.x, style.itemSpacing.y);

	sliderf2("itemSpacing", style.itemSpacing, 0.5f, 0.0, 15.0f);

	static float val3[2];
	sliderf2("slider 3", val3, 1.0f, 0.0f, 1000.0f);

	static float val;
	sliderf("slider 1", &val, 1.0f, 0.0f, 100.0f);

	if (begin_tab_bar("settings#bar"))
	{
		if (begin_tab_item("settings"))
		{
			sliderf4("Background color", style.colors[COL_BACKGROUND], .03f, 0.f, 1.f);
			sliderf4("Item color", style.colors[COL_ITEM_BACKGROUND], .03f, 0.f, 1.f);
			sliderf4("Active color", style.colors[COL_ACTIVE], .03f, 0.f, 1.f);
			sliderf4("Active slider color", style.colors[COL_ITEM_SLIDE], .03f, 0.f, 1.f);
			sliderf4("Text color", style.colors[COL_TEXT], .03f, 0.f, 1.f);
			sliderf4("Active text color", style.colors[COL_TEXT_ACTIVE], .03f, 0.f, 1.f);

			static bool v;
			checkbox("new checkbox", &v);
			same_line();
			checkbox("new checkbox3", &v);

			end_tab_item();
		}
		if (begin_tab_item("new item 2"))
		{
			button("tab_item 2");

			if (begin_tab_bar("tabs2"))
			{
				if (begin_tab_item("new item2"))
				{
					button("tab_item 12");
					sliderf4("Color background2", style.colors[COL_BACKGROUND], .03f, 0.f, 1.f);

					end_tab_item();
				}
				if (begin_tab_item("new item 22"))
				{
					button("tab_item 22");

					end_tab_item();
				}

				end_tab_bar();
			}

			end_tab_item();
		}

		end_tab_bar();
	}

	static int count;
	if (button(fmt_str("button outside of tab item #%i", count)))
		count++;

	end();

	end_frame();
}

void menu::start()
{
	create_context();

	set_window_pos(350, 200);
	set_window_size(450, 750);

	scheduler::schedule(update, 0u, scheduler::render_foreground);

	cell_pad_get_data_d = new detour(cellPad_GetData_t, cellPad_GetData);
}

void menu::stop()
{
	destroy_context();
	delete cell_pad_get_data_d;
}
