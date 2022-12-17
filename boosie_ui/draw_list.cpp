#include "stdafx.h"

void draw_list::add_filled_rect(GRect rect, GColor color)
{
	draw_element elem;
	elem.type = DRAW_TYPE_FILLED_RECT;
	elem.rect = rect;
	elem.color = color;

	elements.push_back(elem);
}

void draw_list::add_rect(GRect rect, float thickness, GColor color)
{
	draw_element elem;
	elem.type = DRAW_TYPE_RECT;
	elem.rect = rect;
	elem.thickness = thickness;
	elem.color = color;

	elements.push_back(elem);
}

void draw_list::add_text(GRect rect, const char* text, int align, float scaleX, float scaleY, GColor color)
{
	draw_element elem;
	elem.type = DRAW_TYPE_TEXT;
	elem.rect = rect;
	elem.text = text;
	elem.align = align;
	elem.scaleX = scaleX;
	elem.scaleY = scaleY;
	elem.color = color;

	elements.push_back(elem);
}

void draw_list::render()
{
	for (draw_element& element : elements)
	{
		switch (element.type)
		{
			case DRAW_TYPE_FILLED_RECT:
				render::add_filled_rect(element.rect, element.color);
				break;
			case DRAW_TYPE_RECT:
				render::add_rect(element.rect, element.thickness, element.color);
				break;
			case DRAW_TYPE_TEXT:
				render::add_text(element.text.c_str(), element.rect, 1, element.align, element.scaleX, element.scaleY, element.color);
				break;
		}
	}

	elements.clear();
}
