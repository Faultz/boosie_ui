#include "stdafx.h"

vec2_t arc_fast_vtx[IM_DRAWLIST_ARCFAST_TABLE_SIZE];

uint32_t r_convert_clr_to_uint(float* flColor)
{
	uint8_t buf[4];

	buf[0] = static_cast<uint8_t>(flColor[0] * 255.f);
	buf[1] = static_cast<uint8_t>(flColor[1] * 255.f);
	buf[2] = static_cast<uint8_t>(flColor[2] * 255.f);
	buf[3] = 255;

	return ((uint8_t)buf[0] << 24 | (uint8_t)buf[1] << 16 | (uint8_t)buf[2] << 8 | (uint8_t)buf[3]);
}

void fetch_structure()
{

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