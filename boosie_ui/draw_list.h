#pragma once

enum draw_id
{
	DRAW_LIST_BACKGROUND,
	DRAW_LIST_FOREGROUND
};

enum draw_type : char
{
	DRAW_TYPE_NONE,
	DRAW_TYPE_FILLED_RECT,
	DRAW_TYPE_RECT,
	DRAW_TYPE_TEXT
};

struct draw_element
{
	draw_element() 
	{
		type = DRAW_TYPE_NONE;
		rect = GRect();
		text = "";
		align = 0;
		scaleX = 0;
		scaleY = 0;
		thickness = 0;
		color = GColor();
	}
	~draw_element()
	{

	}

	draw_type type;
	GRect rect;
	std::string text;
	int align;
	float scaleX;
	float scaleY;
	float thickness;
	GColor color;
};

class draw_list
{
public:
	draw_list()
	{
		elements.clear();
	}
	~draw_list()
	{
		elements.clear();
	}
	void add_filled_rect(GRect rect, GColor color);
	void add_rect(GRect rect, float thickness, GColor color);

	void add_text(GRect rect, const char* text, int align, float scaleX, float scaleY, GColor color);

	void render();
private:
	vector<draw_element> elements;
};