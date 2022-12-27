#include "stdafx.h"

bool g_createCmdCalls;
std::vector<GfxPointVertex> g_pathData;
vec2_t circle_vtx_fast[12];

uint32_t r_convert_clr_to_uint(float* flColor)
{
	uint8_t buf[4];

	buf[0] = static_cast<uint8_t>(flColor[0] * 255.f);
	buf[1] = static_cast<uint8_t>(flColor[1] * 255.f);
	buf[2] = static_cast<uint8_t>(flColor[2] * 255.f);
	buf[3] = static_cast<uint8_t>(flColor[3] * 255.f);

	return ((uint8_t)buf[0] << 24 | (uint8_t)buf[1] << 16 | (uint8_t)buf[2] << 8 | (uint8_t)buf[3]);
}

void R_SetVertex4D(GfxVertex* vert, float x, float y, float z, float w, float s, float t, unsigned int color)
{
	vert->xyzw.x = x;
	vert->xyzw.y = y;
	vert->xyzw.z = z;
	vert->xyzw.w = w;
	vert->normal.packed = 2143289344;
	vert->color.packed = color;
	vert->texCoord.x = s;
	vert->texCoord.y = t;
}


void R_AddCmdDrawQuad(vec2_t* vert, color clr, Material* material)
{
	GfxCmdDrawQuadPicOriginal* quad_pic = reinterpret_cast<GfxCmdDrawQuadPicOriginal*>(R_GetCommandBuffer(RB_DrawQuadPic_id_t, sizeof(GfxCmdDrawQuadPicOriginal)));

	uint32_t col[1];
	col[0] = r_convert_clr_to_uint((float*)&clr[0]);

	quad_pic->material = material;

	if (!material)
		quad_pic->material = globals::gameWhite;

	quad_pic->verts[0] = vert[0];
	quad_pic->color.packed = col[0];

	quad_pic->verts[1] = vert[1];
	quad_pic->color.packed = col[0];

	quad_pic->verts[2] = vert[2];
	quad_pic->color.packed = col[0];

	quad_pic->verts[3] = vert[3];
	quad_pic->color.packed = col[0];
	//quad_pic->multicolor = true;
}

void R_AddCmdDrawQuadMulti(vec2_t* vert, color* clr, float s0, float t0, float s1, float t1, float rotation, Material* material)
{
	GfxCmdDrawQuadPic* quad_pic = reinterpret_cast<GfxCmdDrawQuadPic*>(R_GetCommandBuffer(RB_DrawQuadPic_id_t, sizeof(GfxCmdDrawQuadPic)));

	uint32_t col[4];
	col[0] = r_convert_clr_to_uint((float*)&clr[0]);
	col[1] = r_convert_clr_to_uint((float*)&clr[1]);
	col[2] = r_convert_clr_to_uint((float*)&clr[2]);
	col[3] = r_convert_clr_to_uint((float*)&clr[3]);

	quad_pic->material = material;

	if (!material)
		quad_pic->material = globals::gameWhite;

	quad_pic->s0 = s0;
	quad_pic->t0 = t0;
	quad_pic->s1 = s1;
	quad_pic->t1 = t1;

	quad_pic->rotation = rotation;

	quad_pic->verts[0] = vert[0];
	quad_pic->color[0].packed = col[0];

	quad_pic->verts[1] = vert[1];
	quad_pic->color[1].packed = col[1];

	quad_pic->verts[2] = vert[2];
	quad_pic->color[2].packed = col[2];

	quad_pic->verts[3] = vert[3];
	quad_pic->color[3].packed = col[3];
	//quad_pic->multicolor = true;
}

GfxQuadVertex* R_AddCmdDrawQuadList2D(Material* material, int quad_count)
{
	Material* defaultMaterial = material;

	if (!material)
		defaultMaterial = globals::gameWhite;

	GfxCmdDrawQuadList2D* cmd = reinterpret_cast<GfxCmdDrawQuadList2D*>(R_GetCommandBuffer(RB_DrawQuadPic_id_t, 80 * quad_count + 0xC));
	if (!cmd)
		return nullptr;

	cmd->material = defaultMaterial;
	cmd->quadCount = quad_count;

	return reinterpret_cast<GfxQuadVertex*>(cmd + 1);
}

inline bool RB_CheckOverflow(int vertCount, int indexCount)
{
	int vCount = g_materialCommands->vertexCount;
	int iCount = g_materialCommands->indexCount;

	if ((vCount + vertCount) > 0x800 || (iCount + indexCount) > 0xC00)
	{
		RB_TessOverflow();

		return true;
	}

	return false;
}


inline void RB_AddPath(const GfxPointVertex& point)
{
	g_pathData.push_back(point);
}
inline void RB_PathClear()
{
	g_pathData.clear();
}
//{
//	if (((col_in | col_out) & IM_COL32_A_MASK) == 0 || radius < 0.5f)
//		return;
//
//	// Use arc with automatic segment count
//	draw_list->_PathArcToFastEx(center, radius, 0, IM_DRAWLIST_ARCFAST_SAMPLE_MAX, 0);
//	const int count = draw_list->_Path.Size - 1;
//
//	unsigned int vtx_base = draw_list->_VtxCurrentIdx;
//	draw_list->PrimReserve(count * 3, count + 1);
//
//	// Submit vertices
//	const ImVec2 uv = draw_list->_Data->TexUvWhitePixel;
//	draw_list->PrimWriteVtx(center, uv, col_in);
//	for (int n = 0; n < count; n++)
//		draw_list->PrimWriteVtx(draw_list->_Path[n], uv, col_out);
//
//	// Submit a fan of triangles
//	for (int n = 0; n < count; n++)
//	{
//		draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base));
//		draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base + 1 + n));
//		draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base + 1 + ((n + 1) % count)));
//	}
//	draw_list->_Path.Size = 0;

inline void RB_PathConvexPolyFill(int point_count, GfxPointVertex* verts, Material* material)
{
	if (g_materialCommands == nullptr)
		return;

	R_SetTessMaterial(material, 2, 8);

	if (RB_CheckOverflow(point_count, (point_count - 2) * 3))
		return;

	int i = 0;

	for (i = 0; i < point_count - 2; i++)
	{
		g_materialCommands->indices[g_materialCommands->indexCount] = g_materialCommands->vertexCount;
		g_materialCommands->indices[g_materialCommands->indexCount + 1] = g_materialCommands->vertexCount + i + 1;
		g_materialCommands->indices[g_materialCommands->indexCount + 2] = g_materialCommands->vertexCount + i + 2;
		g_materialCommands->indexCount += 3;
	}

	for (i = 0; i < point_count; i++)
	{
		R_SetVertex4D(&g_materialCommands->verts[g_materialCommands->vertexCount], verts[i].xyz.x, verts[i].xyz.y, 0.f, 1.f, 0.f, 1.0f, verts[i].color);
		g_materialCommands->vertexCount++;
	}

	RB_EndTessSurface();
}

inline void RB_AddPolyLine(int point_count, GfxPointVertex* verts, uint32_t color, float thickness, Material* material)
{
	if (g_materialCommands == nullptr)
		return;

	R_SetTessMaterial(material, 2, 8);

	for (int i1 = 0; i1 < point_count; i1++)
	{
		if (RB_CheckOverflow(4, 6))
			break;

		const int i2 = (i1 + 1) == point_count ? 0 : i1 + 1;
		vec3_t& p1 = verts[i1].xyz;
		vec3_t& p2 = verts[i2].xyz;
		vec3_t diff = p2 - p1;
		diff *= inverse_length(diff, 1.0f);

		const float dx = diff.x * (thickness * 0.5f);
		const float dy = diff.y * (thickness * 0.5f);

		R_SetVertex4D(&g_materialCommands->verts[g_materialCommands->vertexCount], p1.x + dy, p1.y - dx, 0.f, 1.f, 0.f, 0.0f, color);
		R_SetVertex4D(&g_materialCommands->verts[g_materialCommands->vertexCount + 1], p2.x + dy, p2.y - dx, 0.f, 1.f, 0.f, 1.0f, color);
		R_SetVertex4D(&g_materialCommands->verts[g_materialCommands->vertexCount + 2], p2.x - dy, p2.y + dx, 0.f, 1.f, 1.f, 1.0f, color);
		R_SetVertex4D(&g_materialCommands->verts[g_materialCommands->vertexCount + 3], p1.x - dy, p1.y + dx, 0.f, 1.f, 1.f, 0.0f, color);

		g_materialCommands->indices[g_materialCommands->indexCount] = g_materialCommands->vertexCount;
		g_materialCommands->indices[g_materialCommands->indexCount + 1] = g_materialCommands->vertexCount + 1;
		g_materialCommands->indices[g_materialCommands->indexCount + 2] = g_materialCommands->vertexCount + 2;
		g_materialCommands->indices[g_materialCommands->indexCount + 3] = g_materialCommands->vertexCount;
		g_materialCommands->indices[g_materialCommands->indexCount + 4] = g_materialCommands->vertexCount + 2;
		g_materialCommands->indices[g_materialCommands->indexCount + 5] = g_materialCommands->vertexCount + 3;

		g_materialCommands->indexCount += 6;
		g_materialCommands->vertexCount += 4;
	}
	if (*(int*)0x02F82E28 != 0)
		RB_EndTessSurface();
}
inline void RB_AddQuad(vec2_t* positions, color* colors, bool is_multicolored, Material* material)
{
	if (g_materialCommands == nullptr)
		return;

	uint32_t col[4];

	if (is_multicolored)
	{
		R_ConvertColor(colors[0], &col[0]);
		R_ConvertColor(colors[1], &col[1]);
		R_ConvertColor(colors[2], &col[2]);
		R_ConvertColor(colors[3], &col[3]);
	}
	else
		R_ConvertColor(colors[0], &col[0]);

	auto& draw_cmd = *g_materialCommands;

	R_SetTessMaterial(material, 2, 8);

	if (RB_CheckOverflow(4, 6))
		return;

	draw_cmd.indices[draw_cmd.indexCount] = draw_cmd.vertexCount + 3;
	draw_cmd.indices[draw_cmd.indexCount + 1] = draw_cmd.vertexCount;
	draw_cmd.indices[draw_cmd.indexCount + 2] = draw_cmd.vertexCount + 2;
	draw_cmd.indices[draw_cmd.indexCount + 3] = draw_cmd.vertexCount + 2;
	draw_cmd.indices[draw_cmd.indexCount + 4] = draw_cmd.vertexCount;
	draw_cmd.indices[draw_cmd.indexCount + 5] = draw_cmd.vertexCount + 1;

	if (is_multicolored)
	{
		R_SetVertex4D(&draw_cmd.verts[draw_cmd.vertexCount], positions[0].x, positions[0].y, 0.0, 1.0, 0.0, 0.0, col[0]);
		R_SetVertex4D(&draw_cmd.verts[draw_cmd.vertexCount + 1], positions[1].x, positions[1].y, 0.0, 1.0, 1.0, 0.0, col[1]);
		R_SetVertex4D(&draw_cmd.verts[draw_cmd.vertexCount + 2], positions[2].x, positions[2].y, 0.0, 1.0, 1.0, 1.0, col[2]);
		R_SetVertex4D(&draw_cmd.verts[draw_cmd.vertexCount + 3], positions[3].x, positions[3].y, 0.0, 1.0, 0.0, 1.0, col[3]);
	}
	else
	{
		R_SetVertex4D(&draw_cmd.verts[draw_cmd.vertexCount], positions[0].x, positions[0].y, 0.0, 0.0, 0.0, 0.0, col[0]);
		R_SetVertex4D(&draw_cmd.verts[draw_cmd.vertexCount + 1], positions[1].x, positions[1].y, 0.0, 0.0, 1.0, 0.0, col[1]);
		R_SetVertex4D(&draw_cmd.verts[draw_cmd.vertexCount + 2], positions[2].x, positions[2].y, 0.0, 0.0, 1.0, 1.0, col[2]);
		R_SetVertex4D(&draw_cmd.verts[draw_cmd.vertexCount + 3], positions[3].x, positions[3].y, 0.0, 0.0, 0.0, 1.0, col[3]);
	}
	if (*(int*)0x02F82E28 != 0)
		RB_EndTessSurface();
}

inline void RB_PathArcTo(const vec3_t& centre, float radius, uint32_t clr, float a_min, float a_max, int num_segments)
{
	if (radius == 0.0f)
	{
		RB_AddPath({ { centre.x, centre.y, 0.f }, clr });
		return;
	}
	for (int i = 0; i <= num_segments; i++)
	{
		const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
		RB_AddPath({ vec3_t(centre.x + cosf(a) * radius, centre.y + sinf(a) * radius, 0.f), clr });
	}
}

inline void RB_PathArcToFast(const vec3_t& center, float radius, uint32_t clr, int a_min_of_12, int a_max_of_12)
{
	if (radius == 0.0f || a_min_of_12 > a_max_of_12)
	{
		RB_AddPath({ { center.x, center.y, 0.f }, clr });
		return;
	}
	for (int a = a_min_of_12; a <= a_max_of_12; a++)
	{
		const vec2_t& c = circle_vtx_fast[a % 12];
		RB_AddPath({ vec3_t(center.x + c.x * radius, center.y + c.y * radius, 0.f), clr });
	}
}

//uint32_t col;
//R_ConvertColor(col_in, col);
//
//// Add the outer vertices with the ending color
//for (int i = 0; i < num_vertices; i++)
//{
//	const float angle = (i / (float)num_vertices) * 2 * M_PI;
//	const vec2_t offset = vec2_t(cosf(angle), sinf(angle)) * radius;
//	R_SetVertex4D(&g_materialCommands->verts[g_materialCommands->vertexCount], center.x + offset.x, center.y + offset.y, 0.f, 1.f, 1.f, 0.f, col);
//
//	g_materialCommands->vertexCount++;
//}
//
//// Add the indices to create a fan of triangles
//for (int i = 1; i < num_indices; i++)
//{
//	g_materialCommands->indices[g_materialCommands->indexCount] = 0;
//	g_materialCommands->indices[g_materialCommands->indexCount] = i;
//	g_materialCommands->indices[g_materialCommands->indexCount] = i + 1;
//	g_materialCommands->indexCount += 3;
//}
//
////RB_PathArcTo(vec3_

inline void RB_PathLineTo(const vec2_t& pos, uint32_t clr)
{
	RB_AddPath({ { pos.x, pos.y, 0.f, }, clr });
}
inline void RB_PathFillConvex()
{
	RB_PathConvexPolyFill(g_pathData.size(), &g_pathData.front(), globals::gameWhite);
	RB_PathClear();
}

// void __fastcall DrawText2D_38B348(int r3, int r4, int r5, int r6, int r7, int r8, int r9, int r10, int r11, int r12, int r13, int r14, int r15, int r16, int r17, int r18, int r19, int r20, int r21, int r22, int r23, int r24, int r25, int r26);
inline void RB_PathStroke(uint32_t col, float thickness)
{
	RB_AddPolyLine(g_pathData.size(), &g_pathData.front(), col, thickness, globals::gameWhite); RB_PathClear();
}
inline void RB_PathRect(float x, float y, float w, float h, float rounding, int rounding_corners, float* clr1, float* clr2, float* clr3, float* clr4)
{
	vec2_t min{ x, y };
	vec2_t max{ vec2_t(x, y) + vec2_t(w, h) };

	rounding = fminf(rounding, fabsf(max.x - min.x) * (((rounding_corners & cornerFlags_Top) == cornerFlags_Top) || ((rounding_corners & cornerFlags_Bot) == cornerFlags_Bot) ? 0.5f : 1.0f) - 1.0f);
	rounding = fminf(rounding, fabsf(max.y - min.y) * (((rounding_corners & cornerFlags_Left) == cornerFlags_Left) || ((rounding_corners & cornerFlags_Right) == cornerFlags_Right) ? 0.5f : 1.0f) - 1.0f);

	uint32_t col[4];
	R_ConvertColor(clr1, &col[0]);
	R_ConvertColor(clr2, &col[1]);
	R_ConvertColor(clr3, &col[2]);
	R_ConvertColor(clr4, &col[3]);

	if (rounding <= 0.0f || rounding_corners == 0)
	{
		RB_PathLineTo(min, col[0]);
		RB_PathLineTo(vec2_t(max.x, min.y), col[1]);
		RB_PathLineTo(max, col[2]);
		RB_PathLineTo(vec2_t(min.x, max.y), col[3]);
	}
	else
	{
		const float rounding_tl = (rounding_corners & cornerFlags_topLeft) ? rounding : 0.0f;
		const float rounding_tr = (rounding_corners & cornerFlags_topRight) ? rounding : 0.0f;
		const float rounding_br = (rounding_corners & cornerFlags_botRight) ? rounding : 0.0f;
		const float rounding_bl = (rounding_corners & cornerFlags_botLeft) ? rounding : 0.0f;
		RB_PathArcToFast(vec3_t(min.x + rounding_tl, min.y + rounding_tl, 0), rounding_tl, col[0], 6, 9);
		RB_PathArcToFast(vec3_t(max.x - rounding_tr, min.y + rounding_tr, 0), rounding_tr, col[1], 9, 12);
		RB_PathArcToFast(vec3_t(max.x - rounding_br, max.y - rounding_br, 0), rounding_br, col[2], 0, 3);
		RB_PathArcToFast(vec3_t(min.x + rounding_bl, max.y - rounding_bl, 0), rounding_bl, col[3], 3, 6);
	}
}

void RB_PathRectGradient(float x, float y, float w, float h, bool vertical, std::vector<gradient_steps> gradient_steps)
{
	int gradientStepCount = gradient_steps.size() - 1;
	int offset = 0;
	int size = vertical ? h : w;

	for (int i = 0; i < gradientStepCount; i++)
	{
		auto current_gradient = gradient_steps[i];
		auto next_gradient = i != gradientStepCount ? gradient_steps[i + 1] : gradient_steps[i];

		float computed_size = (size * next_gradient.step) - (size * current_gradient.step);

		RB_PathRect(x + vertical ? 0 : offset, y + vertical ? offset : 0, vertical ? w : computed_size, vertical ? computed_size : h, 0.f, 0,
			current_gradient.color, vertical ? current_gradient.color : next_gradient.color, next_gradient.color, vertical ? next_gradient.color : current_gradient.color);

		offset += computed_size;

		RB_PathFillConvex();
	}
}

inline void RB_PathRect(float x, float y, float w, float h, float rounding, int rounding_corners, uint32_t col)
{
	vec2_t min{ x, y };
	min += vec2_t(0.50f, 0.50f);
	vec2_t max{ vec2_t(x, y) + vec2_t(w, h) };
	max -= vec2_t(0.50f, 0.50f);

	rounding = fminf(rounding, fabsf(max.x - min.x) * (((rounding_corners & cornerFlags_Top) == cornerFlags_Top) || ((rounding_corners & cornerFlags_Bot) == cornerFlags_Bot) ? 0.5f : 1.0f) - 1.0f);
	rounding = fminf(rounding, fabsf(max.y - min.y) * (((rounding_corners & cornerFlags_Left) == cornerFlags_Left) || ((rounding_corners & cornerFlags_Right) == cornerFlags_Right) ? 0.5f : 1.0f) - 1.0f);

	if (rounding <= 0.0f || rounding_corners == 0)
	{
		RB_PathLineTo(min, col);
		RB_PathLineTo(vec2_t(max.x, min.y), col);
		RB_PathLineTo(max, col);
		RB_PathLineTo(vec2_t(min.x, max.y), col);
	}
	else
	{
		const float rounding_tl = (rounding_corners & cornerFlags_topLeft) ? rounding : 0.0f;
		const float rounding_tr = (rounding_corners & cornerFlags_topRight) ? rounding : 0.0f;
		const float rounding_br = (rounding_corners & cornerFlags_botRight) ? rounding : 0.0f;
		const float rounding_bl = (rounding_corners & cornerFlags_botLeft) ? rounding : 0.0f;
		RB_PathArcToFast(vec3_t(min.x + rounding_tl, min.y + rounding_tl, 0), rounding_tl, col, 6, 9);
		RB_PathArcToFast(vec3_t(max.x - rounding_tr, min.y + rounding_tr, 0), rounding_tr, col, 9, 12);
		RB_PathArcToFast(vec3_t(max.x - rounding_br, max.y - rounding_br, 0), rounding_br, col, 0, 3);
		RB_PathArcToFast(vec3_t(min.x + rounding_bl, max.y - rounding_bl, 0), rounding_bl, col, 3, 6);
	}
}

inline void RB_PathRect(vec2_t min, vec2_t max, float rounding, int rounding_corners, uint32_t col)
{
	min += vec2_t(0.50f, 0.50f);
	max -= vec2_t(0.50f, 0.50f);

	rounding = fminf(rounding, fabsf(max.x - min.x) * (((rounding_corners & cornerFlags_Top) == cornerFlags_Top) || ((rounding_corners & cornerFlags_Bot) == cornerFlags_Bot) ? 0.5f : 1.0f) - 1.0f);
	rounding = fminf(rounding, fabsf(max.y - min.y) * (((rounding_corners & cornerFlags_Left) == cornerFlags_Left) || ((rounding_corners & cornerFlags_Right) == cornerFlags_Right) ? 0.5f : 1.0f) - 1.0f);

	if (rounding <= 0.0f || rounding_corners == 0)
	{
		RB_PathLineTo(min, col);
		RB_PathLineTo(vec2_t(max.x, min.y), col);
		RB_PathLineTo(max, col);
		RB_PathLineTo(vec2_t(min.x, max.y), col);
	}
	else
	{
		const float rounding_tl = (rounding_corners & cornerFlags_topLeft) ? rounding : 0.0f;
		const float rounding_tr = (rounding_corners & cornerFlags_topRight) ? rounding : 0.0f;
		const float rounding_br = (rounding_corners & cornerFlags_botRight) ? rounding : 0.0f;
		const float rounding_bl = (rounding_corners & cornerFlags_botLeft) ? rounding : 0.0f;
		RB_PathArcToFast(vec3_t(min.x + rounding_tl, min.y + rounding_tl, 0), rounding_tl, col, 6, 9);
		RB_PathArcToFast(vec3_t(max.x - rounding_tr, min.y + rounding_tr, 0), rounding_tr, col, 9, 12);
		RB_PathArcToFast(vec3_t(max.x - rounding_br, max.y - rounding_br, 0), rounding_br, col, 0, 3);
		RB_PathArcToFast(vec3_t(min.x + rounding_bl, max.y - rounding_bl, 0), rounding_bl, col, 3, 6);
	}
}
void RB_AddRect(float x, float y, float width, float height, float* color, float rounding, int rounding_flags, float thickness)
{
	uint32_t col;
	R_ConvertColor(color, &col);

	RB_PathRect(x, y, width, height, rounding, rounding_flags, col);
	RB_PathStroke(col, thickness);
}
void RB_AddRectFilled(float x, float y, float width, float height, float* color, float rounding, int rounding_flags)
{
	uint32_t col;
	R_ConvertColor(color, &col);

	RB_PathRect(x, y, width, height, rounding, rounding_flags, col);
	RB_PathFillConvex();
}
void RB_AddRectFilled(vec2_t min, vec2_t max, float* color, float rounding, int rounding_flags)
{
	uint32_t col;
	R_ConvertColor(color, &col);

	RB_PathRect(min, max, rounding, rounding_flags, col);
	RB_PathFillConvex();
}

void RB_AddRectFilled(vec2_t min, vec2_t max, uint32_t color, float rounding, int rounding_flags)
{
	RB_PathRect(min, max, rounding, rounding_flags, color);
	RB_PathFillConvex();
}
void RB_AddRectFilled(float x, float y, float width, float height, float* clr1, float* clr2, float* clr3, float* clr4, float rounding, int rounding_flags)
{
	RB_PathRect(x, y, width, height, rounding, rounding_flags, clr1, clr2, clr3, clr4);
	RB_PathFillConvex();
}
void RB_AddText(const char* text, int maxChars, Font* font, float x, float y, float xScale, float yScale, float rotation, float* clr, int style)
{
	float _r = (rotation * 0.017453292);

	auto sine = sinf(_r);
	auto cos = cosf(_r);

	uint32_t col;
	R_ConvertColor(clr, &col);
	DrawText2D(text,
	x, y, 1.0f,
	font, xScale, yScale,
	sine, cos, col,
	maxChars, 0, -1,
	0, 0.0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0);

	RB_EndTessSurface();
	//RB_DrawText(text, font, x, y, col);
	//DrawText2D(text, x, y, 1.0, font, 1.0, 1.0, 0.0, 1.0, col, 0x7FFFFFFF, 0, 0, 0, 0.0, col, 0, 0, 0, 0, 0, 0, 0, 0);

	//RB_EndTessSurface();
}
void RB_AddCircle(const vec2_t& centre, float radius, float* color, int num_segments, float thickness)
{
	uint32_t col;
	R_ConvertColor(color, &col);

	const float a_max = M_PI * 2.0f * ((float)num_segments - 1.0f) / (float)num_segments;
	RB_PathArcTo(vec3_t(centre.x, centre.y, 0.f), radius - 0.5f, col, 0.f, a_max, num_segments);
	RB_PathStroke(col, thickness);
}

void RB_AddCircleFilled(const vec2_t& centre, float radius, float* color, int num_segments)
{
	uint32_t col;
	R_ConvertColor(color, &col);

	const float a_max = M_PI * 2.0f * ((float)num_segments - 1.0f) / (float)num_segments;
	RB_PathArcTo(vec3_t(centre.x, centre.y, 0.f), radius - 0.5f, col, 0.f, a_max, num_segments);
	RB_PathFillConvex();
}
void RB_AddRadialGradient(vec2_t center, float radius, color col_in, color col_out)
{
	uint32_t clrs[2];
	R_ConvertColor(col_in, &clrs[0]);
	R_ConvertColor(col_out, &clrs[1]);

	RB_PathLineTo(center, clrs[0]);

	constexpr int num_verts = 35;

	for (int i = 0; i < num_verts; i++)
	{
		const float angle = (i / (float)num_verts) * 2 * M_PI;
		const vec2_t offset = vec2_t(cosf(angle), sinf(angle)) * radius;

		RB_PathLineTo(vec2_t(center.x + offset.x, center.y + (offset.y * 1.3)), clrs[1]);
	}
	RB_PathLineTo(vec2_t(center.x + radius, center.y), clrs[1]);

	RB_PathFillConvex();
}