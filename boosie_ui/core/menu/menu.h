#pragma once

typedef unsigned int menu_id;

enum {
	DIGITAL1 = CELL_PAD_BTN_OFFSET_DIGITAL1,
	DIGITAL2 = CELL_PAD_BTN_OFFSET_DIGITAL2
};

enum menu_data_type
{
	DATA_TYPE_FLOAT,
	DATA_TYPE_INT
};

enum menu_colors
{
	COL_BACKGROUND,
	COL_ITEM_BACKGROUND,
	COL_ITEM_ACTIVE,
	COL_ITEM_SLIDE,
	COL_ACTIVE,
	COL_TEXT,
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

enum menu_tree_node_flags
{
	TREE_NONE,
	TREE_COLLAPSING_HEADER = (1 << 0)
};

enum menu_combo_flags
{
	COMBO_NONE,
	COMBO_FIT_TEXT = (1 << 0)
};

struct menu_last_item_data
{
	menu_id ID;
	GRect Rect;
	GBounds rectRel;
};

struct menu_header
{
	menu_id ID;
	int flags;
	GRect Rect;
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

	char name[32];
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
struct menu_list_clipper_range
{
	int     min;
	int     max;
	bool    posToIndexConvert;      // Begin/End are absolute position (will be converted to indices later)
	char    posToIndexOffsetMin;    // Add to Min after converting to indices
	char    posToIndexOffsetMax;    // Add to Min after converting to indices

	static menu_list_clipper_range    fromIndices(int min, int max) { menu_list_clipper_range r = { min, max, false, 0, 0 }; return r; }
	static menu_list_clipper_range    fromPositions(float y1, float y2, int off_min, int off_max) { menu_list_clipper_range r = { (int)y1, (int)y2, true, (char)off_min, (char)off_max }; return r; }
};

struct menu_list_clipper
{
	int displayStart;
	int displayEnd;
	int itemsCount;
	float itemsHeight;
	float startPosY;
	void* tempData;

	menu_list_clipper()
	{
		memset(this, 0, sizeof(*this));
		itemsCount = -1;
	}
	~menu_list_clipper()
	{
		end();
	}
	void begin(int item_count, int item_height = -1);
	void end();
	bool step();
};

struct menu_list_clipper_data
{
	menu_list_clipper* listClipper;
	float lossynessOffset;
	int stepNo;
	int itemsFrozen;
	vector<menu_list_clipper_range> Ranges;

	menu_list_clipper_data() { memset(this, 0, sizeof(*this)); }
	void reset(menu_list_clipper* clipper) { listClipper = clipper; stepNo = itemsFrozen = 0; Ranges.resize(0); }
};

struct menu_popup
{
	menu_id popupId;
	int popupIdx;
};

class menu_window
{
public:
	menu_window() {
		zero_memory(this, sizeof(menu_window));

		isSameLine = false;
		Pos = vec2_t();
		cursorPos = vec2_t();
		cursorPosLossyness = vec2_t();
		cursorPosPrevLine = vec2_t();
		cursorStartPos = vec2_t();
		cursorMaxPos = vec2_t();
		cursorPrevMove = vec2_t();
		currLineSize = vec2_t();
		prevLineSize = vec2_t();
		windowPadding = vec2_t();
		contentSize = contentSizeIdeal = vec2_t();
		scrollPos = vec2_t();
		scrollTarget = vec2_t(FLT_MAX, FLT_MAX);
		Size = vec2_t();
		sizeFull = vec2_t();
		Indent = vec2_t();
		maxWidth = 0.0f;
		currLineTextBaseOffset = 0.0f;
		prevLineTextBaseOffset = 0.0f;
		innerClipRect = GRect();
		rectRel = GRect();
		itemFlags = 0;
		globalFontScale = 0.0f;
		globalFontScaleY = 0.0f;
		itemFlagStack = vector<menu_item_flags>();
		fontScaleStack = vector<menu_font_mod>();
		currentTab = nullptr;
		tabMap = map<menu_id, menu_tab_bar*>();
		idStack = vector<menu_id>();
		headerStack = vector<menu_header>();
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
	vec2_t cursorPosLossyness;
	vec2_t cursorPosPrevLine;
	vec2_t cursorStartPos;
	vec2_t cursorMaxPos;
	vec2_t cursorPrevMove;
	vec2_t currLineSize;
	vec2_t prevLineSize;
	vec2_t windowPadding;
	vec2_t contentSize;
	vec2_t contentSizeIdeal;
	vec2_t scrollPos;
	vec2_t scrollTarget;
	vec2_t Size;
	vec2_t sizeFull;
	vec2_t Indent;
	float currLineTextBaseOffset;
	float prevLineTextBaseOffset;
	float maxWidth;
	GRect innerClipRect;
	GRect rectRel;
	int itemFlags;
	float globalFontScale;
	float globalFontScaleY;
	vector<menu_item_flags> itemFlagStack;
	vector<menu_font_mod> fontScaleStack;
	menu_tab_bar* currentTab;
	map<menu_id, menu_tab_bar*> tabMap;
	vector<menu_id> idStack;
	vector<menu_header> headerStack;
};

class menu_style
{
public:
	menu_style() 
	{
		itemSpacing = vec2_t(5.0f, 5.0f);
		itemInnerSpacing = vec2_t(1.0f, 1.0f);
		framePadding = vec2_t(0.0f, 0.0f);
		frameRounding = 0.0f;
		indentSpacing = 15.0f;
		colors[COL_BACKGROUND] = color(35, 35, 35, 255);
		colors[COL_ITEM_BACKGROUND] = color(103, 99, 220, 255);
		colors[COL_ITEM_ACTIVE] = color(129, 126, 226, 255);
		colors[COL_ITEM_SLIDE] = color(139, 137, 224, 255);
		colors[COL_ACTIVE] = color(255, 255, 255, 255);
		colors[COL_TEXT] = color(255, 255, 255, 255);
	}

	vec2_t itemSpacing;
	vec2_t itemInnerSpacing;
	vec2_t framePadding;
	float frameRounding;
	float indentSpacing;
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
		activeIdJustActivated = false;
		lastId = 0;
		currentWindow = nullptr;
		currentTabBar = nullptr;
		deltaTime = 0.0f;
		time = 0.0f;
		frameCount = 0;
		sliderCurrentAccum = 0.0f;
		sliderCurrentAccumDirty = false;
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
		openPopupStack = vector<menu_popup>();
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
	bool activeIdJustActivated;
	menu_id lastId;
	menu_window* currentWindow;
	menu_tab_bar* currentTabBar;
	float deltaTime;
	float time;
	int frameCount;
	float sliderCurrentAccum;
	bool sliderCurrentAccumDirty;
	bool isActive;
	bool hasNavRequest;
	bool navRequest;
	bool navInitRequest;
	menu_nav_dir moveNavDir;
	menu_nav_dir moveNavDirLast;
	menu_last_item_data lastItemData;
	menu_nav_results navResults;
	GRect navScoringRect;

	int clipperTempDataStacked;
	vector<menu_list_clipper_data>  clipperTempData;

	menu_style style;
	vector<menu_col_mod> colModifiers;
	vector<menu_popup> openPopupStack;
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
static inline float  saturate(float f) { return (f < 0.0f) ? 0.0f : (f > 1.0f) ? 1.0f : f; }
static inline bool   floatAboveGuaranteedIntegerPrecision(float f) { return f <= -16777216 || f >= 16777216; }

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
	void set_window_pos(menu_window* window, float x, float y);
	void set_window_size(menu_window* window, float w, float h);

	vec2_t calc_next_scroll_and_clamp(menu_window* window);

	void calc_content_size(menu_window* window, vec2_t* content_size, vec2_t* content_size_ideal);
	void set_scroll_from_y(menu_window* window, float local_y, float center_y_ratio);
	vec2_t scroll_to_rect(menu_window* window, const GBounds& item_rect);

	menu_window* create_window(const char* label, int id);

	void set_window_scroll_y(menu_window* window, float scroll);

	void new_frame();

	menu_id get_id(const char* label, const char* str_end = nullptr);
	menu_id get_id(int n);

	void set_active_id(menu_id id);

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
	void indent(float indent_w = 0.0f);
	void unindent(float indent_w = 0.0f);
	float calc_item_dim(std::vector<std::string> data, float scale);

	void push_style_color(int idx, GColor color);
	void pop_style_color(int count = 1);

	void push_item_flags(menu_item_flags flags, bool enabled);
	void pop_item_flags();

	void push_font_scale(float scale, float scale_y = 0.0f);
	void pop_font_scale();

	void push_id(const char* name);
	void push_id(int int_id);
	void pop_id();

	bool is_popup_open(menu_id id);
	void open_popup(menu_id id, int index = 0);
	void close_popup(menu_id id);

	vec2_t calc_item_size(vec2_t size, float default_w = 0.0f, float default_h = 0.0f);

	bool button(const char* label, vec2_t b_size = vec2_t());
	bool checkbox(const char* label, bool* value, vec2_t b_size = vec2_t());
	void text(const char* label, ...);
	void seperator();

	bool slider_behaviour(void* value, int data_type, const char* format, const void* min, const void* max);

	bool begin_tab_bar(const char* name);
	void end_tab_bar();

	bool begin_tab_item(const char* name);
	void end_tab_item();

	bool collapsing_header(const char* name, vec2_t size = vec2_t(0, 0));
	void end_header();

	bool tree_node(const char* name, int flags = TREE_NONE, vec2_t size = vec2_t(0, 0));
	void pop_tree();

	bool tree_node_behaviour(const char* name, int flags = TREE_NONE, vec2_t size = vec2_t(0, 0));

	bool is_header_open(menu_id id);

	void push_header(const char* name, int flags = 0);
	void pop_header_id(menu_id id);
	void pop_header();

	template<typename T = float> bool slider(const char* label, const char* fmt, T* values, int count, int data_type, T min, T max);

	bool sliderf(const char* label, float* value, float min, float max, vec2_t b_size = vec2_t());
	bool sliderf2(const char* label, float* values, float min, float max, vec2_t b_size = vec2_t());
	bool sliderf3(const char* label, float* values, float min, float max, vec2_t b_size = vec2_t());
	bool sliderf4(const char* label, float* values, float min, float max, vec2_t b_size = vec2_t());
	bool slideru(const char* label, int* value, int min, int max, vec2_t b_size = vec2_t());
	bool slideru2(const char* label, int* values, int min, int max, vec2_t b_size = vec2_t());
	bool slideru3(const char* label, int* values, int min, int max, vec2_t b_size = vec2_t());
	bool slideru4(const char* label, int* values, int min, int max, vec2_t b_size = vec2_t());

	bool combo(const char* label, int* index, std::vector<std::string> data, int flags = 0);


	void end_frame();

	void update(void* arg);

	void start();
	void stop();

	extern draw_list* overlay_drawlist;
};
