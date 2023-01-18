#pragma once

#define IM_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR) / sizeof(*(_ARR))))

class gradient_steps
{
public:
	color color;
	float step;
};
struct GfxPointVertex
{
	vec3_t xyz;
	unsigned int color;
}; 
static inline float inverse_length(const vec3_t& lhs, float fail_value) { float d = lhs.x * lhs.x + lhs.y * lhs.y; if (d > 0.0f) return 1.0f / sqrtf(d); return fail_value; }

extern bool g_createCmdCalls;
extern std::vector<GfxPointVertex> g_pathData;

void R_SetVertex4D(GfxVertex* vert, float x, float y, float z, float w, float s, float t, unsigned int color);

uint32_t r_convert_clr_to_uint(float* flColor);
void R_AddCmdDrawQuad(vec2_t* vert, color clr, Material* material);
void R_AddCmdDrawQuadMulti(vec2_t* vert, color* clr, float s0, float t0, float s1, float t1, float rotation, Material* material);
GfxQuadVertex* R_AddCmdDrawQuadList2D(Material* material, int quad_count);
//void RB_AddRectFilled(float x, float y, float width, float height, float* clr1, float* clr2, float* clr3, float* clr4, float rounding, int rounding_flags);
//void RB_AddRectFilled(vec2_t min, vec2_t max, float* color, float rounding, int rounding_flags);
//void RB_AddRectFilled(vec2_t min, vec2_t max, uint32_t color, float rounding, int rounding_flags);
//void RB_AddRectFilled(float x, float y, float width, float height, float* color, float rounding, int rounding_flags);
//void RB_AddRect(float x, float y, float width, float height, float* color, float rounding, int rounding_flags, float thickness);
//void RB_AddText(const char* text, int maxChars, Font* font, float x, float y, float xScale, float yScale, float rotation, float* clr, int style);
//void RB_AddCircle(const vec2_t& centre, float radius, float* color, int num_segments, float thickness);
//void RB_AddCircleFilled(const vec2_t& centre, float radius, float* color, int num_segments);
//void RB_AddRadialGradient(vec2_t center, float radius, color col_in, color col_out);
//void RB_PathClear();
//
//void RB_PathRect(float x, float y, float w, float h, float rounding, int rounding_corners, uint32_t col);
//void RB_PathConvexPolyFill(int point_count, GfxPointVertex* verts, Material* material);
//void RB_PathRectGradient(float x, float y, float w, float h, bool vertical, std::vector<gradient_steps> gradient_steps);
