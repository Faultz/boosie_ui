#pragma once

#define IM_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR) / sizeof(*(_ARR))))
// ImDrawList: Lookup table size for adaptive arc drawing, cover full circle.
#define IM_DRAWLIST_ARCFAST_TABLE_SIZE                          48 // Number of samples in lookup table.

#define IM_DRAWLIST_ARCFAST_SAMPLE_MAX                          IM_DRAWLIST_ARCFAST_TABLE_SIZE // Sample index _PathArcToFastEx() for 360 angle.

extern vec2_t arc_fast_vtx[IM_DRAWLIST_ARCFAST_TABLE_SIZE];

#define ASSERT(x, ...) do { } while(false)

uint32_t r_convert_clr_to_uint(float* flColor);
void R_AddCmdDrawQuad(vec2_t* vert, color clr, Material* material);
void R_AddCmdDrawQuadMulti(vec2_t* vert, color* clr, float s0, float t0, float s1, float t1, float rotation, Material* material);
GfxQuadVertex* R_AddCmdDrawQuadList2D(Material* material, int quad_count);