#pragma once

class window_t;
struct GRect
{
	float x, y, w, h;

	GRect() { x = y = w = h = 0; }
	GRect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h){}

	bool overlaps(const GRect& other) const { return x < other.x + other.w && x + w > other.x && y < other.y + other.h && y + h > other.y; }
	int area() const {	return w * h; }
	int perimeter() const {	return 2 * (w + h); }	
	float diagonalLength() const { return sqrtf(w * w + h * h); }
	vec2_t center() const { return { x + w / 2, y + h / 2 }; }
	bool contains(int x, int y) const { return x >= x && x <= x + w && y >= y && y <= y + h; }
	bool contains(vec2_t pos) const { return pos.x >= x && pos.x <= x + w && y >= y && y <= y + h; }

	GRect intersect(const GRect& other) const {
		float new_x = std::max(x, other.x);
		float new_y = std::max(y, other.y);
		float new_w = std::min(x + w, other.x + other.w) - new_x;
		float new_h = std::min(y + h, other.y + other.h) - new_y;
		return GRect(new_x, new_y, new_w, new_h);
	}

	bool operator==(const GRect& other) { return x == other.x && y == other.y && w == w && h == other.h; }
	bool operator!=(const GRect& other) { return x != other.x && y != other.y && w != w && h != other.h; }
	bool operator<(const GRect& other) { return x < other.x && y < other.y && w < w && h < other.h; }
	bool operator>(const GRect& other) { return x > other.x && y > other.y && w > w && h > other.h; }
	GRect operator+(const GRect& other) const { return GRect(x + other.x, y + other.y, w + other.w, h + other.h); }
	GRect operator-(const GRect& other) const { return GRect(x - other.x, y - other.y, w - other.w, h - other.h); }
	GRect operator*(const GRect& other) const { return GRect(x * other.x, y * other.y, w * other.w, h * other.h); }
	GRect operator/(const GRect& other) const { return GRect(x / other.x, y / other.y, w / other.w, h / other.h); }
};

class GBounds
{
public:
	vec2_t Min;
	vec2_t Max;

	GBounds() : Min(FLT_MAX, FLT_MAX), Max(-FLT_MAX, -FLT_MAX) {}
	GBounds(const vec2_t& min, const vec2_t& max) : Min(min), Max(max) {}
	GBounds(const vec4_t& v) : Min(v.x, v.y), Max(v.z, v.w) {}
	GBounds(float x1, float y1, float x2, float y2) : Min(x1, y1), Max(x2, y2) {}
	GBounds(GRect rect) : Min(rect.x, rect.y), Max(rect.x + rect.w, rect.y + rect.h) {}
	vec2_t      GetCenter() const { return vec2_t((Min.x + Max.x) * 0.5f, (Min.y + Max.y) * 0.5f); }
	vec2_t      GetSize() const { return vec2_t(Max.x - Min.x, Max.y - Min.y); }
	float       GetWidth() const { return Max.x - Min.x; }
	float       GetHeight() const { return Max.y - Min.y; }
	vec2_t      GetTL() const { return Min; }                   // Top-left
	vec2_t      GetTR() const { return vec2_t(Max.x, Min.y); }  // Top-right
	vec2_t      GetBL() const { return vec2_t(Min.x, Max.y); }  // Bottom-left
	vec2_t      GetBR() const { return Max; }                   // Bottom-right
	bool        Contains(const vec2_t& p) const { return p.x >= Min.x && p.y >= Min.y && p.x < Max.x&& p.y < Max.y; }
	bool        Contains(const GBounds& r) const { return r.Min.x >= Min.x && r.Min.y >= Min.y && r.Max.x <= Max.x && r.Max.y <= Max.y; }
	bool        Overlaps(const GBounds& r) const { return r.Min.y <  Max.y&& r.Max.y >  Min.y && r.Min.x <  Max.x&& r.Max.x >  Min.x; }
	void        Add(const vec2_t& p) { if (Min.x > p.x)     Min.x = p.x;     if (Min.y > p.y)     Min.y = p.y;     if (Max.x < p.x)     Max.x = p.x;     if (Max.y < p.y)     Max.y = p.y; }
	void        Add(const GBounds& r) { if (Min.x > r.Min.x) Min.x = r.Min.x; if (Min.y > r.Min.y) Min.y = r.Min.y; if (Max.x < r.Max.x) Max.x = r.Max.x; if (Max.y < r.Max.y) Max.y = r.Max.y; }
	void        Expand(const float amount) { Min.x -= amount;   Min.y -= amount;   Max.x += amount;   Max.y += amount; }
	void        Expand(const vec2_t& amount) { Min.x -= amount.x; Min.y -= amount.y; Max.x += amount.x; Max.y += amount.y; }
	void        Translate(const vec2_t& v) { Min.x += v.x; Min.y += v.y; Max.x += v.x; Max.y += v.y; }
	void        ClipWith(const GBounds& r) { Min = maxV(Min, r.Min); Max = maxV(Max, r.Max); }                   // Simple version, may lead to an inverted rectangle, which is fine for Contains/Overlaps test but not for display.
	void        ClipWithFull(const GBounds& r) { Min = clampV(Min, r.Min, r.Max); Max = clampV(Max, r.Min, r.Max); } // Full version, ensure both points are fully clipped.
	void        Floor() { Min.x = (float)(int)Min.x; Min.y = (float)(int)Min.y; Max.x = (float)(int)Max.x; Max.y = (float)(int)Max.y; }
	void        FixInverted() { if (Min.x > Max.x) std::swap(Min.x, Max.x); if (Min.y > Max.y) std::swap(Min.y, Max.y); }
	bool        IsInverted() const { return Min.x > Max.x || Min.y > Max.y; }
};

enum text_flags
{
	// x axis
	horz_left = (1 << 1),
	horz_center = (1 << 2),
	horz_right = (1 << 3),

	// y axis
	vert_top = (1 << 4),
	vert_center = (1 << 5),
	vert_bottom = (1 << 6),

	align_scale = (1 << 7),

	// text accounted
	underline = (1 << 8),

	// max
	align_max = 69420
};

namespace render
{
	void align_text(int alignment, GRect rect, float w, float h, float* x, float* y);
	void scale_text(int& align, float& xScale, GRect& rect, float textWidth, float& yScale, float textHeight);

	// text drawing
	void add_text(const char* text, GRect rect, int style, int align, float xScale, float yScale, float* clr, float* underline_clr = color());
	void add_text_with_effect(const char* text, GRect rect, int style, int align, float xScale, float yScale, float* clr, float* glow_clr = color());

	void add_outline_rect(float x, float y, float width, float height, float* color, float thickness);

	// rect drawing
	void add_rect(float x, float y, float w, float h, float thickness = 1.f, float* clr = c_white);
	void add_filled_rect(float x, float y, float w, float h, float* clr = color());

	void add_rect(vec2_t pos, vec2_t size, float thickness = 1.f, float* clr = c_white);
	void add_filled_rect(vec2_t pos, vec2_t size, float* clr = color());

	void add_rect(GRect rect, float thickness = 1.f, float* clr = c_white);
	void add_filled_rect(GRect rect, float* clr);

	void add_filled_rect_multi(GRect rect, color clr1, color clr2, color clr3, color clr4);
	void add_filled_rect_multi(GRect rect, float s0, float t0, float s1, float t1, color clr1, color clr2, color clr3, color clr4);

	bool push_clip(GRect rect);
	void pop_clip();

	void read_text_size(const char* text, float& width, float& height, float xScale, float yScale);

	float get_text_width(const char* text, float xScale);
	float get_text_height(const char* text, float yScale);

	void end();
};