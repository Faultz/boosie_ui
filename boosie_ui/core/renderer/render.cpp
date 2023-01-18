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

void render::scale_text(int& align, float& xScale, GRect& rect, float textWidth, float& yScale, float textHeight)
{
	if (align & align_scale)
	{
		xScale *= rect.w / textWidth;
		yScale *= rect.h / textHeight;

		align |= (horz_left | vert_top);
	}
}

void render::add_text(const char* text, GRect rect, int style, int align, float xScale, float yScale, float* clr)
{
	float m_iAdjustedX = 0.f;
	float m_iAdjustedY = 0.f;

	xScale /= 1.1f;
	yScale /= 1.1f;

	float textWidth, textHeight;
	read_text_size(text, textWidth, textHeight, xScale, yScale);

	scale_text(align, xScale, rect, textWidth, yScale, textHeight);
	align_text(align, rect, textWidth, textHeight, &m_iAdjustedX, &m_iAdjustedY);

	if (align & background)
	{
		GRect back_rect(m_iAdjustedX - 2, m_iAdjustedY - textHeight - 2, textWidth + 4, textHeight + 4);

		add_rect(back_rect, 1, GColor(255, 255, 255, 255), 0.0f, 0);
		add_filled_rect(back_rect, GColor(35, 35, 35));
	}

	//RB_AddText(text, 0x7F, globals::fontNormal, m_iAdjustedX, m_iAdjustedY, xScale, yScale, 0.0f, clr, style);
	R_AddCmdDrawText(text, 0x7F, globals::fontNormal, m_iAdjustedX, m_iAdjustedY, xScale, yScale, 0.0f, clr, 6);
}

void render::add_text_with_effect(const char* text, GRect rect, int style, int align, float xScale, float yScale, float* clr, float* glow_clr)
{
	float m_iAdjustedX = 0.f;
	float m_iAdjustedY = 0.f;

	xScale /= 1.1f;
	yScale /= 1.1f;

	float textWidth, textHeight;
	read_text_size(text, textWidth, textHeight, xScale, yScale);

	scale_text(align, xScale, rect, textWidth, yScale, textHeight);
	align_text(align, rect, textWidth, textHeight, &m_iAdjustedX, &m_iAdjustedY);

	//R_AddCmdDrawText(text, 0x7F, globals::fontNormal, m_iAdjustedX, m_iAdjustedY, xScale, yScale, 0.0f, clr, style);

	R_AddCmdDrawTextWithEffects(text, 0x7F, globals::fontNormal, m_iAdjustedX, m_iAdjustedY, 1.0f, xScale, yScale, 0.0f, clr, style, glow_clr, nullptr, nullptr, 0, 0, 0, 0);
}

void render::add_outline_rect(float x, float y, float width, float height, float* color, float thickness)
{
	R_AddCmdDrawStretchPic(x - thickness, y - thickness, width + (thickness * 2), thickness, 0, 0, 1, 1, color, globals::gameWhite); // Top>
	R_AddCmdDrawStretchPic(x - thickness, y + height, width + (thickness * 2), thickness, 0, 0, 1, 1, color, globals::gameWhite); // Bottom>
	R_AddCmdDrawStretchPic(x - thickness, y - thickness, thickness, height + thickness, 0, 0, 1, 1, color, globals::gameWhite); // Left>
	R_AddCmdDrawStretchPic(x + width, y - thickness, thickness, height + (thickness * 2), 0, 0, 1, 1, color, globals::gameWhite); // Right>
}

void render::add_rect(vec2_t pos, vec2_t size, float thickness, float* clr, float rounding, int flags)
{
	if (thickness == 0.f)
		return;

	add_outline_rect(pos.x, pos.y, size.x, size.y, clr, thickness);
	//RB_AddRect(pos.x, pos.y, size.x, size.y, clr, rounding, flags, thickness);
}
void render::add_rect(float x, float y, float w, float h, float thickness, float* clr, float rounding, int flags)
{
	if (thickness == 0.f)
		return;

	add_outline_rect(x, y, w, h, clr, thickness);
	//RB_AddRect(x, y, w, h, clr, rounding, flags, thickness);
}
void render::add_rect(GRect rect, float thickness, float* clr, float rounding, int flags)
{
	if (thickness == 0.f)
		return;

	add_outline_rect(rect.x, rect.y, rect.w, rect.h, clr, thickness);
	//RB_AddRect(rect.x, rect.y, rect.w, rect.h, clr, rounding, flags, thickness);
}

void render::add_filled_rect(vec2_t pos, vec2_t size, float* clr, float rounding, int flags)
{
	//RB_AddRectFilled(pos.x, pos.y, size.x, size.y, clr, rounding, flags);
	R_AddCmdDrawStretchPic(pos.x, pos.y, size.x, size.y, 0, 0, 1, 1, clr, globals::gameWhite);
}
void render::add_filled_rect(float x, float y, float w, float h, float* clr, float rounding, int flags)
{
	//RB_AddRectFilled(x, y, w, h, clr, rounding, flags);
	R_AddCmdDrawStretchPic(x, y, w, h, 0, 0, 1, 1, clr, globals::gameWhite);
}
void render::add_filled_rect(GRect rect, float* clr, Material* override_material, float rotation)
{
	//RB_AddRectFilled(rect.x, rect.y, rect.w, rect.h, clr, rounding, flags);
	CL_DrawStretchPicPhysicalRotateXYInternal(rect.x, rect.y, rect.w, rect.h, 0, 0, 1, 1, rotation, clr, override_material ? override_material : globals::gameWhite);
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

	R_AddCmdDrawQuadMulti(m_QuadPos, m_clrTable, 0, 0, 1, 1, 90, Material_RegisterHandle("white", 7, false, -1));
}

bool render::push_clip(GRect rect)
{
	auto clip_cmd_ptr = R_AddCmdSetScissorRect(true, static_cast<int>(rect.x), static_cast<int>(rect.y), static_cast<int>(rect.w), static_cast<int>(rect.h));
	return clip_cmd_ptr != nullptr;
}

void render::pop_clip()
{
	R_AddCmdSetScissorRect(false, 0, 0, 0, 0);
}

void render::read_text_size(const char* text, float& width, float& height, float xScale, float yScale)
{
	width = R_TextWidth(0, text, strlen(text), globals::fontNormal) * xScale;
	height = globals::fontNormal->pixelHeight * yScale;
}

float render::get_text_width(const char* text, float xScale)
{
	xScale /= 1.1f;
	return R_TextWidth(0, text, strlen(text), globals::fontNormal) * xScale;
}

float render::get_text_height(const char* text, float yScale)
{
	yScale /= 1.1f;
	return globals::fontNormal->pixelHeight * yScale;
}