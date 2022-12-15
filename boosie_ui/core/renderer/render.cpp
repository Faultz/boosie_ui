#include "stdafx.h"

void render::align_text(int alignment, GRect rect, float w, float h, float* x, float* y)
{

	if (alignment & horz_left)
		*x = rect.x;
	else if (alignment & horz_center)
		*x = (rect.x + (rect.w / 2) - (w / 2));
	else if (alignment & horz_right)
		*x = ((rect.x + rect.w) - w);

	*y = rect.y;
	if (alignment & vert_top)
		*y += rect.h;
	if (alignment & vert_center)
		*y += ((rect.h / 2) + (h / 2));
	else if (alignment & vert_bottom)
		*y += ((rect.h / 2) + h);
}

void render::add_text(std::string text, GRect rect, int style, int align, float xScale, float yScale, float* clr, float* underline_clr)
{
	float m_iAdjustedX = 0.f;
	float m_iAdjustedY = 0.f;

	float textWidth, textHeight;
	read_text_size(text, textWidth, textHeight, xScale, yScale);

	scale_text(align, xScale, rect, textWidth, yScale, textHeight);
	align_text(align, rect, textWidth, textHeight, &m_iAdjustedX, &m_iAdjustedY);

	R_AddCmdDrawText(text.data(), 0x7F, globals::fontNormal, m_iAdjustedX, m_iAdjustedY, xScale, yScale, 0.0f, clr, style);

	if (align & underline && underline_clr != color())
		add_filled_rect(m_iAdjustedX, m_iAdjustedY + yScale, textWidth, 2.f, underline_clr);

}

void render::scale_text(int& align, float& xScale, GRect& rect, float textWidth, float& yScale, float textHeight)
{
	if (align & align_scale)
	{
		xScale *= rect.w / textWidth;
		yScale *= rect.h / textHeight;

		align |= (horz_left | vert_top);
	}
}

void render::add_text_with_effect(std::string text, GRect rect, int style, int align, float xScale, float yScale, float* clr, float* glow_clr)
{
	float m_iAdjustedX = 0.f;
	float m_iAdjustedY = 0.f;

	float textWidth, textHeight;
	read_text_size(text, textWidth, textHeight, xScale, yScale);

	scale_text(align, xScale, rect, textWidth, yScale, textHeight);
	align_text(align, rect, textWidth, textHeight, &m_iAdjustedX, &m_iAdjustedY);

	//R_AddCmdDrawTextWithEffects(text.data(), 0x7F, globals::fontNormal, m_iAdjustedX, m_iAdjustedY, xScale, yScale, 0.0f, clr, style, glow_clr, nullptr, nullptr, 0, 0, 0, 0);
}

void render::add_outline_rect(float x, float y, float width, float height, float* color, float thickness)
{
	R_AddCmdDrawStretchPic(x - thickness, y - thickness, width + (thickness * 2), thickness, 0, 0, 1, 1, color, globals::gameWhite); // Top>
	R_AddCmdDrawStretchPic(x - thickness, y + height, width + (thickness * 2), thickness, 0, 0, 1, 1, color, globals::gameWhite); // Bottom>
	R_AddCmdDrawStretchPic(x - thickness, y - thickness, thickness, height + thickness, 0, 0, 1, 1, color, globals::gameWhite); // Left>
	R_AddCmdDrawStretchPic(x + width, y - thickness, thickness, height + (thickness * 2), 0, 0, 1, 1, color, globals::gameWhite); // Right>
}

void render::add_rect(vec2_t pos, vec2_t size, float thickness, float* clr)
{
	if (thickness == 0.f)
		return;

	add_outline_rect(pos.x, pos.y, size.x, size.y, clr, thickness);
}
void render::add_rect(float x, float y, float w, float h, float thickness, float* clr)
{
	if (thickness == 0.f)
		return;

	add_outline_rect(x, y, w, h, clr, thickness);
}
void render::add_rect(GRect rect, float thickness, float* clr)
{
	if (thickness == 0.f)
		return;

	add_outline_rect(rect.x, rect.y, rect.w, rect.h, clr, thickness);
}

void render::add_filled_rect(vec2_t pos, vec2_t size, float* clr)
{
	R_AddCmdDrawStretchPic(pos.x, pos.y, size.x, size.y, 0, 0, 1, 1, clr, globals::gameWhite);
}
void render::add_filled_rect(float x, float y, float w, float h, float* clr)
{
	R_AddCmdDrawStretchPic(x, y, w, h, 0, 0, 1, 1, clr, globals::gameWhite);
}
void render::add_filled_rect(GRect rect, float* clr)
{
	float aspect_ratio = 4 / 3;

	R_AddCmdDrawStretchPic(rect.x, rect.y, rect.w, rect.h * aspect_ratio, 0, 0, 1, 1, clr, globals::gameWhite);
}

void render::add_filled_rect_multi(GRect rect, color clr1, color clr2, color clr3, color clr4)
{
	vec2_t min(rect.x, rect.y);

	vec2_t max(min + vec2_t(rect.w, rect.h));

	vec2_t m_QuadPos[4] =
	{
		min,
		vec2_t(max.x, min.y),
		max,
		vec2_t(min.x, max.y)
	};

	color m_clrTable[4] =
	{
		clr1,
		clr2,
		clr3,
		clr4
	};

	R_AddCmdDrawQuadMulti(m_QuadPos, m_clrTable, 0.f, 0.f, 1.f, 1.f, 0, globals::gameWhite);
}

void render::add_filled_rect_multi(GRect rect, float s0, float t0, float s1, float t1, color clr1, color clr2, color clr3, color clr4)
{
	vec2_t min(rect.x, rect.y);

	vec2_t max(min + vec2_t(rect.w, rect.h));

	vec2_t m_QuadPos[4] =
	{
		min,
		vec2_t(max.x, min.y),
		max,
		vec2_t(min.x, max.y)
	};

	color m_clrTable[4] =
	{
		clr1,
		clr2,
		clr3,
		clr4
	};
	//R_AddCmdDrawQuad(m_QuadPos, m_clrTable[0], Material_RegisterHandle("master_prestige_01", 7));

	R_AddCmdDrawQuadMulti(m_QuadPos, m_clrTable, 0, 0, 1, 1, 90, Material_RegisterHandle("master_prestige_01", 7));
}

bool render::push_clip(GRect rect)
{
	auto clip_cmd_ptr = R_AddCmdSetScissorRect(static_cast<int>(rect.x), static_cast<int>(rect.y), static_cast<int>(rect.w), static_cast<int>(rect.h));
	return clip_cmd_ptr != nullptr;
}

void render::pop_clip()
{
	R_AddCmdClearScissorRect();
}

void render::read_text_size(std::string text, float& width, float& height, float xScale, float yScale)
{
	width = R_TextWidth(text.data(), text.length(), globals::fontNormal) * xScale;
	height = globals::fontNormal->pixelHeight * yScale;
}

float render::get_text_width(std::string text, float xScale)
{
	return R_TextWidth(text.data(), text.length(), globals::fontNormal) * xScale;
}

float render::get_text_height(std::string text, float yScale)
{
	return globals::fontNormal->pixelHeight * yScale;
}