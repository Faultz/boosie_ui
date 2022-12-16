#pragma once

typedef unsigned int menu_id;

enum {
	DIGITAL1 = CELL_PAD_BTN_OFFSET_DIGITAL1,
	DIGITAL2 = CELL_PAD_BTN_OFFSET_DIGITAL2
};

enum menu_colors
{
	COL_BACKGROUND,
	COL_ITEM_BACKGROUND,
	COL_ITEM_ACTIVE,
	COL_ITEM_SLIDE,
	COL_ACTIVE,
	COL_TEXT,
	COL_TEXT_ACTIVE,
	COL_MAX
};

enum menu_nav_dir
{
	NAV_DIR_NONE = -1,
	NAV_DIR_LEFT = 0,
	NAV_DIR_RIGHT = 1,
	NAV_DIR_UP = 2,
	NAV_DIR_DOWN = 3,
	NAV_DIR_COUNT
};

enum menu_item_status_flags
{
	ITEM_STATUS_NONE = 0,
	ITEM_STATUS_HOVERING = (1 << 0),
	ITEM_STATUS_VISIBLE = (1 << 1)
};

enum menu_item_flags
{
	ITEM_FLAG_NONE = 0,
	ITEM_FLAG_NO_NAV = (1 << 0),
	ITEM_FLAG_SLIDER_NO_TEXT = (1 << 1)
};

struct menu_last_item_data
{
	menu_id ID;
	GRect Rect;
	GBounds rectRel;
};

struct menu_col_mod
{
	int idx;
	color col;
};

struct menu_font_mod
{
	float backupScale;
	float backupScaleY;
};

struct menu_nav_results
{
	menu_id menuId;
	float distBox;
	float distCenter;
	float distAxial;
	GBounds rectRel;
	GRect navRect;

	menu_nav_results() { clear(); }
	void clear() { menuId = 0; distBox = distCenter = distAxial = FLT_MAX; rectRel = GBounds(); navRect = GRect(); }
};

class menu_tab_item
{
public:
	menu_tab_item()
	{
		zero_memory(this, sizeof(menu_tab_item));

		tabItemRect = GRect();
	}

	std::string name;
	int tabItemId;
	GRect tabItemRect;
};

class menu_tab_bar
{
public:
	menu_tab_bar()
	{
		zero_memory(this, sizeof(menu_tab_bar));

		ID = 0;
		barRect = GRect();
		activeTabId = 0;
		currentTabItem = nullptr;
		tabItems = map<menu_id, menu_tab_item*>();
	}
	~menu_tab_bar()
	{
		for (int i = 0; i < tabItems.size(); i++)
		{
			delete tabItems.Data[i].second;
		}
	}

	menu_id ID;
	GRect barRect;
	int activeTabIndex;
	menu_id activeTabId;
	menu_tab_item* currentTabItem;
	map<menu_id, menu_tab_item*> tabItems;
};

class menu_window
{
public:
	menu_window() {
		zero_memory(this, sizeof(menu_window));

		isSameLine = false;
		Pos = vec2_t();
		cursorPos = vec2_t();
		cursorPosPrevLine = vec2_t();
		cursorStartPos = vec2_t();
		cursorMaxPos = vec2_t();
		cursorPrevMove = vec2_t();
		currLineSize = vec2_t();
		prevLineSize = vec2_t();
		windowPadding = vec2_t();
		scrollPos = vec2_t();
		scrollTarget = vec2_t(FLT_MAX, FLT_MAX);
		indentSize = 0.0f;
		currLineTextBaseOffset = 0.0f;
		prevLineTextBaseOffset = 0.0f;
		clipRect = GRect();
		rectRel = GRect();
		itemFlags = 0;
		globalFontScale = 0.0f;
		globalFontScaleY = 0.0f;
		itemFlagStack = vector<menu_item_flags>();
		fontScaleStack = vector<menu_font_mod>();
		currentTab = nullptr;
		tabMap = map<menu_id, menu_tab_bar*>();
	}
	~menu_window()
	{
		for (int i = 0; i < tabMap.size(); i++)
		{
			delete tabMap.Data[i].second;
		}

		tabMap.Data.clear();
		itemFlagStack.clear();
	}

	bool isSameLine;
	vec2_t Pos;
	vec2_t cursorPos;
	vec2_t cursorPosPrevLine;
	vec2_t cursorStartPos;
	vec2_t cursorMaxPos;
	vec2_t cursorPrevMove;
	vec2_t currLineSize;
	vec2_t prevLineSize;
	vec2_t windowPadding;
	vec2_t contentSize;
	vec2_t scrollPos;
	vec2_t scrollTarget;
	float currLineTextBaseOffset;
	float prevLineTextBaseOffset;
	float indentSize;
	GRect clipRect;
	GRect rectRel;
	int itemFlags;
	float globalFontScale;
	float globalFontScaleY;
	vector<menu_item_flags> itemFlagStack;
	vector<menu_font_mod> fontScaleStack;
	menu_tab_bar* currentTab;
	map<menu_id, menu_tab_bar*> tabMap;
};

class menu_style
{
public:
	menu_style() 
	{
		borderSpacing = vec2_t(8.0f, 8.0f);
		itemSpacing = vec2_t(5.0f, 5.0f);
		itemInnerSpacing = vec2_t(4.0f, 4.0f);
		framePadding = vec2_t(4.0f, 5.0f);
		colors[COL_BACKGROUND] = color(35, 35, 35, 255);
		colors[COL_ITEM_BACKGROUND] = color(103, 99, 220, 255);
		colors[COL_ITEM_ACTIVE] = color(129, 126, 226, 255);
		colors[COL_ITEM_SLIDE] = color(139, 137, 224, 255);
		colors[COL_ACTIVE] = color(255, 255, 255, 255);
		colors[COL_TEXT] = color(255, 255, 255, 255);
		colors[COL_TEXT_ACTIVE] = color(200, 230, 200, 255);
	}

	vec2_t borderSpacing;
	vec2_t itemSpacing;
	vec2_t itemInnerSpacing;
	vec2_t framePadding;
	color colors[COL_MAX];
};

class menu_context
{
public:
	menu_context()
	{
		open = false;
		activeId = 0;
		activeIdLast = 0;
		lastId = 0;
		currentWindow = nullptr;
		currentTabBar = nullptr;
		rect = GRect();
		deltaTime = 0.0f;
		time = 0.0f;
		frameCount = 0;
		isActive = false;
		hasNavRequest = false;
		navRequest = false;
		navInitRequest = true;
		moveNavDir = NAV_DIR_NONE;
		moveNavDirLast = NAV_DIR_NONE;
		lastItemData = menu_last_item_data();
		navResults = menu_nav_results();
		style = menu_style();
		colModifiers = vector<menu_col_mod>();
		idStack = vector<menu_id>();
		windowMap = map<menu_id, menu_window*>();
	}
	~menu_context()
	{
		for (int i = 0; i < windowMap.size(); i++)
		{
			delete windowMap.Data[i].second;
		}

		colModifiers.clear();
		windowMap.Data.clear();
	}

	bool open;
	menu_id activeId;
	menu_id activeIdLast;
	menu_id lastId;
	menu_window* currentWindow;
	menu_tab_bar* currentTabBar;
	GRect rect;
	float deltaTime;
	float time;
	int frameCount;
	bool isActive;
	bool hasNavRequest;
	bool navRequest;
	bool navInitRequest;
	menu_nav_dir moveNavDir;
	menu_nav_dir moveNavDirLast;
	menu_last_item_data lastItemData;
	menu_nav_results navResults;
	GRect navScoringRect;
	menu_style style;
	vector<menu_col_mod> colModifiers;
	vector<menu_id> idStack;
	map<menu_id, menu_window*> windowMap;

	void createNavRequest(menu_nav_dir navDir) 
	{
		moveNavDir = moveNavDirLast = navDir;
		hasNavRequest = true;
	}
	void startNavRequest() 
	{
		if (hasNavRequest)
		{
			navRequest = true;
		}
	}
	void stopNavRequest() 
	{
		if (navRequest)
		{
			hasNavRequest = false;
			navRequest = false;
		}
	}
};

extern menu_context* gMenuCtx;

namespace menu
{
	void create_context();
	void destroy_context();

	bool is_down(int id, float repeat = 0.0f);
	bool is_pressed(int id);
	bool is_released(int id);

	inline GBounds window_rect_rel_to_abs(menu_window* window, const GBounds& r) { vec2_t off = window->cursorStartPos; return GBounds(r.Min.x - off.x, r.Min.y - off.y, r.Max.x - off.x, r.Max.y - off.y); }

	void set_window_pos(vec2_t pos);
	void set_window_size(vec2_t size);
	void set_window_pos(float x, float y);
	void set_window_size(float w, float h);

	vec2_t calc_next_scroll_and_clamp(menu_window* window);

	void calc_content_size(menu_window* window, vec2_t* content_size, vec2_t* content_size_ideal);
	void set_scroll_from_y(menu_window* window, float local_y, float center_y_ratio);
	vec2_t scroll_to_rect(menu_window* window, const GBounds& item_rect);

	menu_window* create_window(int id);

	void set_window_scroll_y(menu_window* window, float scroll);

	void new_frame();

	menu_id get_id(const char* label);

	menu_window* get_current_window();
	static void read_analog_input(analog_input_t* analog)
	{
		static short analog_deadzone = 20;
		short m_RightX = g_nav.pad.button[4 + 0] - 128;
		short m_RightY = g_nav.pad.button[4 + 1] - 128;
		short m_LeftX = g_nav.pad.button[4 + 2] - 128;
		short m_LeftY = g_nav.pad.button[4 + 3] - 128;
		if ((m_RightX > -analog_deadzone) && (m_RightX < analog_deadzone))
			m_RightX = 0;

		if ((m_RightY > -analog_deadzone) && (m_RightY < analog_deadzone))
			m_RightY = 0;

		if ((m_LeftX > -analog_deadzone) && (m_LeftX < analog_deadzone))
			m_LeftX = 0;

		if ((m_LeftY > -analog_deadzone) && (m_LeftY < analog_deadzone))
			m_LeftY = 0;

		analog->analogRight.x = static_cast<double>(m_RightX) / 128.0f;
		analog->analogRight.y = static_cast<double>(m_RightY) / 128.0f;
		analog->analogLeft.x = static_cast<double>(m_LeftX) / 128.0f;
		analog->analogLeft.y = static_cast<double>(m_LeftY) / 128.0f;
	}

	void begin(const char* name);
	void end();

	void set_active_id(int id);
	bool item_add(GRect rect, int id);
	void item_size(vec2_t size, float text_baseline_y = -1.0f);
	void same_line(float offset_from_start_x = 0.0f, float spacing_w = -1.0f);

	void push_style_color(int idx, GColor color);
	void pop_style_color(int count = 1);

	void push_item_flags(menu_item_flags flags, bool enabled);
	void pop_item_flags();

	void push_font_scale(float scale, float scale_y = 0.0f);
	void pop_font_scale();

	void push_id(const char* name);
	void push_id(int int_id);
	void pop_id();

	vec2_t calc_item_size(vec2_t size, float default_w, float default_h);

	bool button(const char* label, vec2_t b_size = vec2_t());
	bool checkbox(const char* label, bool* value, vec2_t b_size = vec2_t());
	void text(const char* label, ...);

	bool begin_tab_bar(const char* name);
	void end_tab_bar();

	bool begin_tab_item(const char* name);
	void end_tab_item();

	template<typename T> bool slider_behaviour(T& value, T inc, T min, T max)
	{
		menu_context& g = *gMenuCtx;

		if (!g.isActive)
			return false;

		if (g_nav.is_pressed(NAV_BACK))
			g.isActive = false;

		if (g_nav.is_down(NAV_DPAD_LEFT, 0.04f))
		{
			if (value <= min)
				value = min;
			else
				value -= inc;

			return true;
		}
		else if (g_nav.is_down(NAV_DPAD_RIGHT, 0.04f))
		{
			if (value >= max)
				value = max;
			else
				value += inc;

			return true;
		}

		return false;
	}
	bool slider(const char* label, const char* fmt, float* values, int count, float inc, float min, float max);
	bool slider(const char* label, const char* fmt, int* values, int count, int inc, int min, int max);

	bool sliderf(const char* label, float* value, float inc, float min, float max, vec2_t b_size = vec2_t());
	bool sliderf2(const char* label, float* values, float inc, float min, float max, vec2_t b_size = vec2_t());
	bool sliderf3(const char* label, float* values, float inc, float min, float max, vec2_t b_size = vec2_t());
	bool sliderf4(const char* label, float* values, float inc, float min, float max, vec2_t b_size = vec2_t());
	bool slideru(const char* label, int* value, int inc, int min, int max, vec2_t b_size = vec2_t());
	bool slideru2(const char* label, int* values, int inc, int min, int max, vec2_t b_size = vec2_t());
	bool slideru3(const char* label, int* values, int inc, int min, int max, vec2_t b_size = vec2_t());
	bool slideru4(const char* label, int* values, int inc, int min, int max, vec2_t b_size = vec2_t());

	void end_frame();

	void update();

	void start();
	void stop();
};
