#include "stdafx.h"

menu_context* gMenuCtx;

GRect navScoringRect, navResultRect;

draw_list* menu::overlay_drawlist;

static const uint32_t GCrc32LookupTable[256] =
{
	0x00000000,0x77073096,0xEE0E612C,0x990951BA,0x076DC419,0x706AF48F,0xE963A535,0x9E6495A3,0x0EDB8832,0x79DCB8A4,0xE0D5E91E,0x97D2D988,0x09B64C2B,0x7EB17CBD,0xE7B82D07,0x90BF1D91,
	0x1DB71064,0x6AB020F2,0xF3B97148,0x84BE41DE,0x1ADAD47D,0x6DDDE4EB,0xF4D4B551,0x83D385C7,0x136C9856,0x646BA8C0,0xFD62F97A,0x8A65C9EC,0x14015C4F,0x63066CD9,0xFA0F3D63,0x8D080DF5,
	0x3B6E20C8,0x4C69105E,0xD56041E4,0xA2677172,0x3C03E4D1,0x4B04D447,0xD20D85FD,0xA50AB56B,0x35B5A8FA,0x42B2986C,0xDBBBC9D6,0xACBCF940,0x32D86CE3,0x45DF5C75,0xDCD60DCF,0xABD13D59,
	0x26D930AC,0x51DE003A,0xC8D75180,0xBFD06116,0x21B4F4B5,0x56B3C423,0xCFBA9599,0xB8BDA50F,0x2802B89E,0x5F058808,0xC60CD9B2,0xB10BE924,0x2F6F7C87,0x58684C11,0xC1611DAB,0xB6662D3D,
	0x76DC4190,0x01DB7106,0x98D220BC,0xEFD5102A,0x71B18589,0x06B6B51F,0x9FBFE4A5,0xE8B8D433,0x7807C9A2,0x0F00F934,0x9609A88E,0xE10E9818,0x7F6A0DBB,0x086D3D2D,0x91646C97,0xE6635C01,
	0x6B6B51F4,0x1C6C6162,0x856530D8,0xF262004E,0x6C0695ED,0x1B01A57B,0x8208F4C1,0xF50FC457,0x65B0D9C6,0x12B7E950,0x8BBEB8EA,0xFCB9887C,0x62DD1DDF,0x15DA2D49,0x8CD37CF3,0xFBD44C65,
	0x4DB26158,0x3AB551CE,0xA3BC0074,0xD4BB30E2,0x4ADFA541,0x3DD895D7,0xA4D1C46D,0xD3D6F4FB,0x4369E96A,0x346ED9FC,0xAD678846,0xDA60B8D0,0x44042D73,0x33031DE5,0xAA0A4C5F,0xDD0D7CC9,
	0x5005713C,0x270241AA,0xBE0B1010,0xC90C2086,0x5768B525,0x206F85B3,0xB966D409,0xCE61E49F,0x5EDEF90E,0x29D9C998,0xB0D09822,0xC7D7A8B4,0x59B33D17,0x2EB40D81,0xB7BD5C3B,0xC0BA6CAD,
	0xEDB88320,0x9ABFB3B6,0x03B6E20C,0x74B1D29A,0xEAD54739,0x9DD277AF,0x04DB2615,0x73DC1683,0xE3630B12,0x94643B84,0x0D6D6A3E,0x7A6A5AA8,0xE40ECF0B,0x9309FF9D,0x0A00AE27,0x7D079EB1,
	0xF00F9344,0x8708A3D2,0x1E01F268,0x6906C2FE,0xF762575D,0x806567CB,0x196C3671,0x6E6B06E7,0xFED41B76,0x89D32BE0,0x10DA7A5A,0x67DD4ACC,0xF9B9DF6F,0x8EBEEFF9,0x17B7BE43,0x60B08ED5,
	0xD6D6A3E8,0xA1D1937E,0x38D8C2C4,0x4FDFF252,0xD1BB67F1,0xA6BC5767,0x3FB506DD,0x48B2364B,0xD80D2BDA,0xAF0A1B4C,0x36034AF6,0x41047A60,0xDF60EFC3,0xA867DF55,0x316E8EEF,0x4669BE79,
	0xCB61B38C,0xBC66831A,0x256FD2A0,0x5268E236,0xCC0C7795,0xBB0B4703,0x220216B9,0x5505262F,0xC5BA3BBE,0xB2BD0B28,0x2BB45A92,0x5CB36A04,0xC2D7FFA7,0xB5D0CF31,0x2CD99E8B,0x5BDEAE1D,
	0x9B64C2B0,0xEC63F226,0x756AA39C,0x026D930A,0x9C0906A9,0xEB0E363F,0x72076785,0x05005713,0x95BF4A82,0xE2B87A14,0x7BB12BAE,0x0CB61B38,0x92D28E9B,0xE5D5BE0D,0x7CDCEFB7,0x0BDBDF21,
	0x86D3D2D4,0xF1D4E242,0x68DDB3F8,0x1FDA836E,0x81BE16CD,0xF6B9265B,0x6FB077E1,0x18B74777,0x88085AE6,0xFF0F6A70,0x66063BCA,0x11010B5C,0x8F659EFF,0xF862AE69,0x616BFFD3,0x166CCF45,
	0xA00AE278,0xD70DD2EE,0x4E048354,0x3903B3C2,0xA7672661,0xD06016F7,0x4969474D,0x3E6E77DB,0xAED16A4A,0xD9D65ADC,0x40DF0B66,0x37D83BF0,0xA9BCAE53,0xDEBB9EC5,0x47B2CF7F,0x30B5FFE9,
	0xBDBDF21C,0xCABAC28A,0x53B39330,0x24B4A3A6,0xBAD03605,0xCDD70693,0x54DE5729,0x23D967BF,0xB3667A2E,0xC4614AB8,0x5D681B02,0x2A6F2B94,0xB40BBE37,0xC30C8EA1,0x5A05DF1B,0x2D02EF8D,
};

// Known size hash
// It is ok to call ImHashData on a string with known length but the ### operator won't be supported.
// FIXME-OPT: Replace with e.g. FNV1a hash? CRC32 pretty much randomly access 1KB. Need to do proper measurements.
menu_id hash_data(const void* data_p, size_t data_size, uint32_t seed)
{
	uint32_t crc = ~seed;
	const unsigned char* data = (const unsigned char*)data_p;
	const uint32_t* crc32_lut = GCrc32LookupTable;
	while (data_size-- != 0)
		crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ *data++];
	return ~crc;
}

// Zero-terminated string hash, with support for ### to reset back to seed value
// We support a syntax of "label###id" where only "###id" is included in the hash, and only "label" gets displayed.
// Because this syntax is rarely used we are optimizing for the common case.
// - If we reach ### in the string we discard the hash so far and reset to the seed.
// - We don't do 'current += 2; continue;' after handling ### to keep the code smaller/faster (measured ~10% diff in Debug build)
// FIXME-OPT: Replace with e.g. FNV1a hash? CRC32 pretty much randomly access 1KB. Need to do proper measurements.
menu_id hash_str(const char* data_p, size_t data_size, uint32_t seed)
{
	seed = ~seed;
	uint32_t crc = seed;
	const unsigned char* data = (const unsigned char*)data_p;
	const uint32_t* crc32_lut = GCrc32LookupTable;
	if (data_size != 0)
	{
		while (data_size-- != 0)
		{
			unsigned char c = *data++;
			if (c == '#' && data_size >= 2 && data[0] == '#' && data[1] == '#')
				crc = seed;
			crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ c];
		}
	}
	else
	{
		while (unsigned char c = *data++)
		{
			if (c == '#' && data[0] == '#' && data[1] == '#')
				crc = seed;
			crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ c];
		}
	}
	return ~crc;
}

const char* parseFormatFindStart(const char* fmt)
{
	while (char c = fmt[0])
	{
		if (c == '%' && fmt[1] != '%')
			return fmt;
		else if (c == '%')
			fmt++;
		fmt++;
	}
	return fmt;
}

const char* parseFormatFindEnd(const char* fmt)
{
	if (fmt[0] != '%')
		return fmt;
	const unsigned int ignored_uppercase_mask = (1 << ('I' - 'A')) | (1 << ('L' - 'A'));
	const unsigned int ignored_lowercase_mask = (1 << ('h' - 'a')) | (1 << ('j' - 'a')) | (1 << ('l' - 'a')) | (1 << ('t' - 'a')) | (1 << ('w' - 'a')) | (1 << ('z' - 'a'));
	for (char c; (c = *fmt) != 0; fmt++)
	{
		if (c >= 'A' && c <= 'Z' && ((1 << (c - 'A')) & ignored_uppercase_mask) == 0)
			return fmt + 1;
		if (c >= 'a' && c <= 'z' && ((1 << (c - 'a')) & ignored_lowercase_mask) == 0)
			return fmt + 1;
	}
	return fmt;
}

void parseFormatSanitizeForPrinting(const char* fmt_in, char* fmt_out, size_t fmt_out_size)
{
	const char* fmt_end = parseFormatFindEnd(fmt_in);
	while (fmt_in < fmt_end)
	{
		char c = *fmt_in++;
		if (c != '\'' && c != '$' && c != '_')
			*(fmt_out++) = c;
	}
	*fmt_out = 0;
}

template<typename TYPE>
TYPE roundScalarWithFormatT(const char* format, int data_type, TYPE v)
{
	const char* fmt_start = parseFormatFindStart(format);
	if (fmt_start[0] != '%' || fmt_start[1] == '%')
		return v;

	char fmt_sanitized[32];
	parseFormatSanitizeForPrinting(fmt_start, fmt_sanitized, IM_ARRAYSIZE(fmt_sanitized));
	fmt_start = fmt_sanitized;

	char v_str[64];
	snprintf(v_str, (size_t)64, fmt_start, v);
	const char* p = v_str;
	while (*p == ' ')
		p++;
	v = (TYPE)std::atof(p);

	return v;
}

template<typename TYPE>
static const char* Atoi(const char* src, TYPE* output)
{
	int negative = 0;
	if (*src == '-') { negative = 1; src++; }
	if (*src == '+') { src++; }
	TYPE v = 0;
	while (*src >= '0' && *src <= '9')
		v = (v * 10) + (*src++ - '0');
	*output = negative ? -v : v;
	return src;
}


int parseFormatPrecision(const char* fmt, int default_precision)
{
	fmt = parseFormatFindStart(fmt);
	if (fmt[0] != '%')
		return default_precision;
	fmt++;
	while (*fmt >= '0' && *fmt <= '9')
		fmt++;
	int precision = INT_MAX;
	if (*fmt == '.')
	{
		fmt = Atoi<int>(fmt + 1, &precision);
		if (precision < 0 || precision > 99)
			precision = default_precision;
	}
	if (*fmt == 'e' || *fmt == 'E') // Maximum precision with scientific notation
		precision = -1;
	if ((*fmt == 'g' || *fmt == 'G') && precision == INT_MAX)
		precision = -1;
	return (precision == INT_MAX) ? default_precision : precision;
}

// Convert a value v in the output space of a slider into a parametric position on the slider itself (the logical opposite of ScaleValueFromRatioT)
template<typename TYPE, typename SIGNEDTYPE, typename FLOATTYPE>
float scaleRatioFromValueT(int data_type, TYPE v, TYPE v_min, TYPE v_max, bool is_logarithmic, float logarithmic_zero_epsilon, float zero_deadzone_halfsize)
{
	if (v_min == v_max)
		return 0.0f;

	const TYPE v_clamped = (v_min < v_max) ? clamp(v, v_min, v_max) : clamp(v, v_max, v_min);
	if (is_logarithmic)
	{
		bool flipped = v_max < v_min;

		if (flipped) // Handle the case where the range is backwards
			std::swap(v_min, v_max);

		// Fudge min/max to avoid getting close to log(0)
		FLOATTYPE v_min_fudged = (fabsf((FLOATTYPE)v_min) < logarithmic_zero_epsilon) ? ((v_min < 0.0f) ? -logarithmic_zero_epsilon : logarithmic_zero_epsilon) : (FLOATTYPE)v_min;
		FLOATTYPE v_max_fudged = (fabsf((FLOATTYPE)v_max) < logarithmic_zero_epsilon) ? ((v_max < 0.0f) ? -logarithmic_zero_epsilon : logarithmic_zero_epsilon) : (FLOATTYPE)v_max;

		// Awkward special cases - we need ranges of the form (-100 .. 0) to convert to (-100 .. -epsilon), not (-100 .. epsilon)
		if ((v_min == 0.0f) && (v_max < 0.0f))
			v_min_fudged = -logarithmic_zero_epsilon;
		else if ((v_max == 0.0f) && (v_min < 0.0f))
			v_max_fudged = -logarithmic_zero_epsilon;

		float result;
		if (v_clamped <= v_min_fudged)
			result = 0.0f; // Workaround for values that are in-range but below our fudge
		else if (v_clamped >= v_max_fudged)
			result = 1.0f; // Workaround for values that are in-range but above our fudge
		else if ((v_min * v_max) < 0.0f) // Range crosses zero, so split into two portions
		{
			float zero_point_center = (-(float)v_min) / ((float)v_max - (float)v_min); // The zero point in parametric space.  There's an argument we should take the logarithmic nature into account when calculating this, but for now this should do (and the most common case of a symmetrical range works fine)
			float zero_point_snap_L = zero_point_center - zero_deadzone_halfsize;
			float zero_point_snap_R = zero_point_center + zero_deadzone_halfsize;
			if (v == 0.0f)
				result = zero_point_center; // Special case for exactly zero
			else if (v < 0.0f)
				result = (1.0f - (float)(logf(-(FLOATTYPE)v_clamped / logarithmic_zero_epsilon) / logf(-v_min_fudged / logarithmic_zero_epsilon))) * zero_point_snap_L;
			else
				result = zero_point_snap_R + ((float)(logf((FLOATTYPE)v_clamped / logarithmic_zero_epsilon) / logf(v_max_fudged / logarithmic_zero_epsilon)) * (1.0f - zero_point_snap_R));
		}
		else if ((v_min < 0.0f) || (v_max < 0.0f)) // Entirely negative slider
			result = 1.0f - (float)(logf(-(FLOATTYPE)v_clamped / -v_max_fudged) / logf(-v_min_fudged / -v_max_fudged));
		else
			result = (float)(logf((FLOATTYPE)v_clamped / v_min_fudged) / logf(v_max_fudged / v_min_fudged));

		return flipped ? (1.0f - result) : result;
	}
	else
	{
		// Linear slider
		return (float)((FLOATTYPE)(SIGNEDTYPE)(v_clamped - v_min) / (FLOATTYPE)(SIGNEDTYPE)(v_max - v_min));
	}
}

// Convert a parametric position on a slider into a value v in the output space (the logical opposite of ScaleRatioFromValueT)
template<typename TYPE, typename SIGNEDTYPE, typename FLOATTYPE>
TYPE scaleValueFromRatioT(int data_type, float t, TYPE v_min, TYPE v_max, bool is_logarithmic, float logarithmic_zero_epsilon, float zero_deadzone_halfsize)
{
	// We special-case the extents because otherwise our logarithmic fudging can lead to "mathematically correct"
	// but non-intuitive behaviors like a fully-left slider not actually reaching the minimum value. Also generally simpler.
	if (t <= 0.0f || v_min == v_max)
		return v_min;
	if (t >= 1.0f)
		return v_max;

	TYPE result = (TYPE)0;
	if (is_logarithmic)
	{
		// Fudge min/max to avoid getting silly results close to zero
		FLOATTYPE v_min_fudged = (fabsf((FLOATTYPE)v_min) < logarithmic_zero_epsilon) ? ((v_min < 0.0f) ? -logarithmic_zero_epsilon : logarithmic_zero_epsilon) : (FLOATTYPE)v_min;
		FLOATTYPE v_max_fudged = (fabsf((FLOATTYPE)v_max) < logarithmic_zero_epsilon) ? ((v_max < 0.0f) ? -logarithmic_zero_epsilon : logarithmic_zero_epsilon) : (FLOATTYPE)v_max;

		const bool flipped = v_max < v_min; // Check if range is "backwards"
		if (flipped)
			std::swap(v_min_fudged, v_max_fudged);

		// Awkward special case - we need ranges of the form (-100 .. 0) to convert to (-100 .. -epsilon), not (-100 .. epsilon)
		if ((v_max == 0.0f) && (v_min < 0.0f))
			v_max_fudged = -logarithmic_zero_epsilon;

		float t_with_flip = flipped ? (1.0f - t) : t; // t, but flipped if necessary to account for us flipping the range

		if ((v_min * v_max) < 0.0f) // Range crosses zero, so we have to do this in two parts
		{
			float zero_point_center = (-(float)std::min(v_min, v_max)) / fabsf((float)v_max - (float)v_min); // The zero point in parametric space
			float zero_point_snap_L = zero_point_center - zero_deadzone_halfsize;
			float zero_point_snap_R = zero_point_center + zero_deadzone_halfsize;
			if (t_with_flip >= zero_point_snap_L && t_with_flip <= zero_point_snap_R)
				result = (TYPE)0.0f; // Special case to make getting exactly zero possible (the epsilon prevents it otherwise)
			else if (t_with_flip < zero_point_center)
				result = (TYPE)-(logarithmic_zero_epsilon * powf(-v_min_fudged / logarithmic_zero_epsilon, (FLOATTYPE)(1.0f - (t_with_flip / zero_point_snap_L))));
			else
				result = (TYPE)(logarithmic_zero_epsilon * powf(v_max_fudged / logarithmic_zero_epsilon, (FLOATTYPE)((t_with_flip - zero_point_snap_R) / (1.0f - zero_point_snap_R))));
		}
		else if ((v_min < 0.0f) || (v_max < 0.0f)) // Entirely negative slider
			result = (TYPE)-(-v_max_fudged * powf(-v_min_fudged / -v_max_fudged, (FLOATTYPE)(1.0f - t_with_flip)));
		else
			result = (TYPE)(v_min_fudged * powf(v_max_fudged / v_min_fudged, (FLOATTYPE)t_with_flip));
	}
	else
	{
		// Linear slider
		const bool is_floating_point = (data_type == DATA_TYPE_FLOAT) || (data_type == DATA_TYPE_FLOAT);
		if (is_floating_point)
		{
			result = (TYPE)easing::lerp<TYPE>(v_min, v_max, t);
		}
		else if (t < 1.0)
		{
			// - For integer values we want the clicking position to match the grab box so we round above
			//   This code is carefully tuned to work with large values (e.g. high ranges of U64) while preserving this property..
			// - Not doing a *1.0 multiply at the end of a range as it tends to be lossy. While absolute aiming at a large s64/u64
			//   range is going to be imprecise anyway, with this check we at least make the edge values matches expected limits.
			FLOATTYPE v_new_off_f = (SIGNEDTYPE)(v_max - v_min) * t;
			result = (TYPE)((SIGNEDTYPE)v_min + (SIGNEDTYPE)(v_new_off_f + (FLOATTYPE)(v_min > v_max ? -0.5 : 0.5)));
		}
	}

	return result;
}

static menu_nav_dir navScoreItemQuadrant(float dx, float dy)
{
	if (fabsf(dx) > fabsf(dy))
		return (dx > 0.0f) ? NAV_DIR_RIGHT : NAV_DIR_LEFT;
	return (dy > 0.0f) ? NAV_DIR_DOWN : NAV_DIR_UP;
}

static float inline navScoreItemDist(float a0, float a1, float b0, float b1)
{
	if (a1 < b0)
		return a1 - b0;
	if (b1 < a0)
		return a0 - b1;
	return 0.0f;
}

bool navScoreItem(menu_nav_results* result) {
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	GRect scoring_rect = navScoringRect;

	if (g.lastItemData.Rect == scoring_rect)
		return false;

	GBounds clip(window.innerClipRect);
	GBounds cand(g.lastItemData.Rect);
	GBounds navScoring(scoring_rect);

	cand.ClipWithFull(clip);

	cand.Min.y = clamp(cand.Min.y, clip.Min.y, clip.Max.y);
	cand.Max.y = clamp(cand.Max.y, clip.Min.y, clip.Max.y);

	// Compute distance between boxes
	  // FIXME-NAV: Introducing biases for vertical navigation, needs to be removed.
	float dbx = navScoreItemDist(cand.Min.x, cand.Max.x, navScoring.Min.x, navScoring.Max.x);
	float dby = navScoreItemDist(easing::lerp(cand.Min.y, cand.Max.y, 0.2f), easing::lerp(cand.Min.y, cand.Max.y, 0.8f),
		easing::lerp(navScoring.Min.y, navScoring.Max.y, 0.2f), easing::lerp(navScoring.Min.y, navScoring.Max.y, 0.8f));
	if (dby != 0.0f && dbx != 0.0f)
		dbx = (dbx / 1000.0f) + ((dbx > 0.0f) ? +1.0f : -1.0f);
	float dist_box = fabsf(dbx) + fabsf(dby);

	// Compute distance between centers (this is off by a factor of 2, but we only compare center distances with each other so it doesn't matter)
	float dcx = (cand.Min.x + cand.Max.x) - (navScoring.Min.x + navScoring.Max.x);
	float dcy = (cand.Min.y + cand.Max.y) - (navScoring.Min.y + navScoring.Max.y);
	float dist_center = fabsf(dcx) + fabsf(dcy); // L1 metric (need this for our connectedness guarantee)

	// Determine which quadrant of 'curr' our candidate item 'cand' lies in based on distance
	menu_nav_dir quadrant = NAV_DIR_NONE;
	float dax = 0.0f, day = 0.0f, dist_axial = 0.0f;
	if (dbx != 0.0f || dby != 0.0f)
	{
		// For non-overlapping boxes, use distance between boxes
		dax = dbx;
		day = dby;
		dist_axial = dist_box;
		quadrant = navScoreItemQuadrant(dbx, dby);
	}
	else if (dcx != 0.0f || dcy != 0.0f)
	{
		// For overlapping boxes with different centers, use distance between centers
		dax = dcx;
		day = dcy;
		dist_axial = dist_center;
		quadrant = navScoreItemQuadrant(dcx, dcy);
	}

	//auto fmt = fmt_str_n<1028>("dbox (%.2f,%.2f->%.4f)\ndcen (%.2f,%.2f->%.4f)\nd (%.2f,%.2f->%.4f)\nnav %c, quadrant %c", dbx, dby, dist_box, dcx, dcy, dist_center, dax, day, dist_axial, "WENS"[g.moveNavDir], "WENS"[quadrant]);
	//render::add_text(fmt, cand, 1, vert_center | horz_center, 0.5f, 0.5f * 1.5, color(255, 255, 255));

	bool new_best = false;
	if (quadrant == g.moveNavDir)
	{
		// Does it beat the current best candidate?
		if (dist_box < result->distBox)
		{
			result->distBox = dist_box;
			result->distCenter = dist_center;
			return true;
		}
		if (dist_box == result->distBox)
		{
			// Try using distance between center points to break ties
			if (dist_center < result->distCenter)
			{
				result->distCenter = dist_center;
				new_best = true;
			}
			else if (dist_center == result->distCenter)
			{
				// Still tied! we need to be extra-careful to make sure everything gets linked properly. We consistently break ties by symbolically moving "later" items 
				// (with higher index) to the right/downwards by an infinitesimal amount since we the current "best" button already (so it must have a lower index), 
				// this is fairly easy. This rule ensures that all buttons with dx==dy==0 will end up being linked in order of appearance along the x axis.
				if (((g.moveNavDir == NAV_DIR_UP || g.moveNavDir == NAV_DIR_DOWN) ? dby : dbx) < 0.0f) // moving bj to the right/down decreases distance
					new_best = true;
			}
		}
	}

	// Axial check: if 'curr' has no link at all in some direction and 'cand' lies roughly in that direction, add a tentative link. This will only be kept if no "real" matches
	// are found, so it only augments the graph produced by the above method using extra links. (important, since it doesn't guarantee strong connectedness)
	// This is just to avoid buttons having no links in a particular direction when there's a suitable neighbor. you get good graphs without this too.
	// 2017/09/29: FIXME: This now currently only enabled inside menu bars, ideally we'd disable it everywhere. Menus in particular need to catch failure. For general navigation it feels awkward.
	// Disabling it may however lead to disconnected graphs when nodes are very spaced out on different axis. Perhaps consider offering this as an option?
	if (result->distBox == FLT_MAX && dist_axial < result->distAxial)  // Check axial match
		if ((g.moveNavDir == NAV_DIR_LEFT && dax < 0.0f) || (g.moveNavDir == NAV_DIR_RIGHT && dax > 0.0f) || (g.moveNavDir == NAV_DIR_UP && day < 0.0f) || (g.moveNavDir == NAV_DIR_DOWN && day > 0.0f))
		{
			result->distAxial = dist_axial;
			new_best = true;
		}


	return new_best;
}

detour* cell_pad_get_data_d = nullptr;
int cellPad_GetData(uint32_t port_no, CellPadData* data)
{
	CellPadData tmpCellPad = { 0 };

	tmpCellPad.len = data->len;
	cellPadGetData(port_no, &tmpCellPad);
	memcpy((void*)data, (void*)&tmpCellPad, sizeof(CellPadData));

	if (data->len > 0 && port_no == 0)
		memcpy(&g_nav.pad, &tmpCellPad, sizeof(CellPadData));	
	
	if (data->len > 0 && gMenuCtx->open)
	{
		data->button[CELL_PAD_BTN_OFFSET_DIGITAL1] &= ~(CELL_PAD_CTRL_UP | CELL_PAD_CTRL_DOWN | CELL_PAD_CTRL_LEFT |
			CELL_PAD_CTRL_RIGHT);
		data->button[CELL_PAD_BTN_OFFSET_DIGITAL2] &= ~(CELL_PAD_CTRL_CROSS | CELL_PAD_CTRL_CIRCLE);
	}

	return 0;
}

void menu_list_clipper::begin(int item_count, int item_height)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	startPosY = window.cursorPos.y;
	itemsHeight = item_height;
	itemsCount = item_count;
	displayStart = -1;
	displayEnd = 0;

	if (++g.clipperTempDataStacked > g.clipperTempData.Size)
		g.clipperTempData.resize(g.clipperTempDataStacked, menu_list_clipper_data());
	menu_list_clipper_data* data = &g.clipperTempData[g.clipperTempDataStacked - 1];
	data->reset(this);
	data->lossynessOffset = window.cursorPosLossyness.y;
	tempData = data;
}

static void sortAndFuseRanges(vector<menu_list_clipper_range>& ranges, int offset = 0)
{
	if (ranges.Size - offset <= 1)
		return;

	// Helper to order ranges and fuse them together if possible (bubble sort is fine as we are only sorting 2-3 entries)
	for (int sort_end = ranges.Size - offset - 1; sort_end > 0; --sort_end)
		for (int i = offset; i < sort_end + offset; ++i)
			if (ranges[i].min > ranges[i + 1].min)
				std::swap(ranges[i], ranges[i + 1]);

	// Now fuse ranges together as much as possible.
	for (int i = 1 + offset; i < ranges.Size; i++)
	{
		IM_ASSERT(!ranges[i].posToIndexConvert && !ranges[i - 1].posToIndexConvert);
		if (ranges[i - 1].max < ranges[i].min)
			continue;
		ranges[i - 1].min =	std::min(ranges[i - 1].min, ranges[i].min);
		ranges[i - 1].max =	std::max(ranges[i - 1].max, ranges[i].max);
		ranges.erase(ranges.Data + i);
		i--;
	}
}
static void seekCursorAndSetupPrevLine(float pos_y, float line_height)
{
	menu_context& g = *gMenuCtx;
	menu_window* window = g.currentWindow;

	window->cursorPos.y = pos_y;
	window->cursorMaxPos.y = std::max(window->cursorMaxPos.y, pos_y - g.style.itemSpacing.y);
	window->cursorPosPrevLine.y = window->cursorPos.y - line_height;  // Setting those fields so that SetScrollHereY() can properly function after the end of our clipper usage.
	window->prevLineSize.y = (line_height - g.style.itemSpacing.y);      // If we end up needing more accurate data (to e.g. use SameLine) we may as well make the clipper have a fourth step to let user process and display the last item in their list.
}

static void seekCursorForItem(menu_list_clipper* clipper, int item_n)
{
	// StartPosY starts from ItemsFrozen hence the subtraction
	// Perform the add and multiply with double to allow seeking through larger ranges
	menu_list_clipper_data* data = (menu_list_clipper_data*)clipper->tempData;
	float pos_y = (float)((double)clipper->startPosY + data->lossynessOffset + (double)(item_n - data->itemsFrozen) * clipper->itemsHeight);
	seekCursorAndSetupPrevLine(pos_y, clipper->itemsHeight);
}

static bool listClipper_StepInternal(menu_list_clipper* clipper)
{
	menu_context& g = *gMenuCtx;
	menu_window* window = g.currentWindow;
	menu_list_clipper_data* data = (menu_list_clipper_data*)clipper->tempData;

	// No items
	if (clipper->itemsCount == 0)
		return false;

	// Step 0: Let you process the first element (regardless of it being visible or not, so we can measure the element height)
	bool calc_clipping = false;
	if (data->stepNo == 0)
	{
		clipper->startPosY = window->cursorPos.y;
		if (clipper->itemsHeight <= 0.0f)
		{
			// Submit the first item (or range) so we can measure its height (generally the first range is 0..1)
			data->Ranges.push_front(menu_list_clipper_range::fromIndices(data->itemsFrozen, data->itemsFrozen + 1));
			clipper->displayStart = std::max(data->Ranges[0].min, data->itemsFrozen);
			clipper->displayEnd = std::min(data->Ranges[0].max, clipper->itemsCount);
			data->stepNo = 1;
			return true;
		}
		calc_clipping = true;   // If on the first step with known item height, calculate clipping.
	}

	// Step 1: Let the clipper infer height from first range
	if (clipper->itemsHeight <= 0.0f)
	{
		clipper->itemsHeight = (window->cursorPos.y - clipper->startPosY) / (float)(clipper->displayEnd - clipper->displayStart);
		bool affected_by_floating_point_precision = floatAboveGuaranteedIntegerPrecision(clipper->startPosY) || floatAboveGuaranteedIntegerPrecision(window->cursorPos.y);
		if (affected_by_floating_point_precision)
			clipper->itemsHeight = window->prevLineSize.y + g.style.itemSpacing.y; // FIXME: Technically wouldn't allow multi-line entries.

		calc_clipping = true;   // If item height had to be calculated, calculate clipping afterwards.
	}

	// Step 0 or 1: Calculate the actual ranges of visible elements.
	const int already_submitted = clipper->displayEnd;
	if (calc_clipping)
	{

		// Add visible range
		data->Ranges.push_back(menu_list_clipper_range::fromPositions(window->innerClipRect.y, window->innerClipRect.y + window->innerClipRect.h, 0, 0));

		// Convert position ranges to item index ranges
		// - Very important: when a starting position is after our maximum item, we set Min to (ItemsCount - 1). This allows us to handle most forms of wrapping.
		// - Due to how Selectable extra padding they tend to be "unaligned" with exact unit in the item list,
		//   which with the flooring/ceiling tend to lead to 2 items instead of one being submitted.
		for (int i = 0; i < data->Ranges.Size; i++)
			if (data->Ranges[i].posToIndexConvert)
			{
				int m1 = (int)(((double)data->Ranges[i].min - window->cursorPos.y - data->lossynessOffset) / clipper->itemsHeight);
				int m2 = (int)((((double)data->Ranges[i].max - window->cursorPos.y - data->lossynessOffset) / clipper->itemsHeight) + 0.999999f);
				data->Ranges[i].min = clamp(already_submitted + m1 + data->Ranges[i].posToIndexOffsetMin, already_submitted, clipper->itemsCount - 1);
				data->Ranges[i].max = clamp(already_submitted + m2 + data->Ranges[i].posToIndexOffsetMax, data->Ranges[i].min + 1, clipper->itemsCount);
				data->Ranges[i].posToIndexConvert = false;
			}
		sortAndFuseRanges(data->Ranges, data->stepNo);
	}

	// Step 0+ (if item height is given in advance) or 1+: Display the next range in line.
	if (data->stepNo < data->Ranges.Size)
	{
		clipper->displayStart = std::max(data->Ranges[data->stepNo].min, already_submitted);
		clipper->displayEnd = std::min(data->Ranges[data->stepNo].max, clipper->itemsCount);
		if (clipper->displayStart > already_submitted) //-V1051
			seekCursorForItem(clipper, clipper->displayStart);
		data->stepNo++;
		return true;
	}

	// After the last step: Let the clipper validate that we have reached the expected Y position (corresponding to element DisplayEnd),
	// Advance the cursor to the end of the list and then returns 'false' to end the loop.
	if (clipper->itemsCount < INT_MAX)
		seekCursorForItem(clipper, clipper->itemsCount);

	return false;
}

void menu_list_clipper::end()
{
	menu_context& g = *gMenuCtx;
	if (menu_list_clipper_data* data = (menu_list_clipper_data*)tempData)
	{
		if (itemsCount >= 0 && itemsCount < INT_MAX && displayStart >= 0)
			seekCursorForItem(this, itemsCount);

		// Restore temporary buffer and fix back pointers which may be invalidated when nesting
		data->stepNo = data->Ranges.Size;
		if (--g.clipperTempDataStacked > 0)
		{
			data = &g.clipperTempData[g.clipperTempDataStacked - 1];
			data->listClipper->tempData = data;
		}
		tempData = NULL;
	}
	itemsCount = -1;
}

bool menu_list_clipper::step()
{
	bool ret = listClipper_StepInternal(this);
	if (ret && (displayStart == displayEnd))
		ret = false;

	if (!ret)
		end();

	return ret;
}

void menu::create_context()
{
	gMenuCtx = new menu_context();
	overlay_drawlist = new draw_list();
}

void menu::destroy_context()
{
	g_pathData.clear();

	delete gMenuCtx;
	delete overlay_drawlist;
}

void menu::set_window_pos(vec2_t pos)
{
	menu_window& window = *get_current_window();
	window.Pos.x = pos.x;
	window.Pos.y = pos.y;
}

void menu::set_window_size(vec2_t size)
{
	menu_window& window = *get_current_window();
	window.Size.x = size.x;
	window.Size.y = size.y;
}

void menu::set_window_pos(menu_window* window, float x, float y)
{
	window->Pos.x = x;
	window->Pos.y = y;
}

void menu::set_window_size(menu_window* window, float w, float h)
{
	window->Size.x = w;
	window->Size.y = h;
	window->sizeFull = vec2_t(w, h);
}

vec2_t menu::calc_next_scroll_and_clamp(menu_window* window)
{
	vec2_t scroll = window->scrollPos;
	if (window->scrollTarget.y < FLT_MAX)
	{
		scroll.y = window->scrollTarget.y;
		scroll.y = clamp(scroll.y, 0.0f, window->contentSize.y - window->innerClipRect.h);
	}

	return scroll;
}

void menu::calc_content_size(menu_window* window, vec2_t* content_size, vec2_t* content_size_ideal)
{
	content_size->x = floorf(window->cursorMaxPos.x - window->cursorStartPos.x);
	content_size->y = floorf(window->cursorMaxPos.y - window->cursorStartPos.y);
}

void menu::set_scroll_from_y(menu_window* window, float local_y, float center_y_ratio)
{
	window->scrollTarget.y = floorf(local_y + window->scrollPos.y);
}

vec2_t menu::scroll_to_rect(menu_window* window, const GBounds& item_rect)
{
	menu_context& g = *gMenuCtx;

	GBounds clip_bb(window->innerClipRect);

	const bool can_be_fully_visible_y = item_rect.GetHeight() + (g.style.itemSpacing.y * 2.0f) <= clip_bb.GetHeight();
	float target_y = can_be_fully_visible_y ? floorf((item_rect.Min.y + item_rect.Max.y - clip_bb.GetHeight()) * 0.5f) : item_rect.Min.y;
	set_scroll_from_y(window, item_rect.Max.y - window->Pos.y, 0.0f);

	vec2_t next_scroll = calc_next_scroll_and_clamp(window);
	vec2_t delta_scroll = next_scroll - window->scrollPos;

	return delta_scroll;
}

menu_window* menu::create_window(const char* label, int id)
{
	menu_context& g = *gMenuCtx;

	g.windowMap[id] = new menu_window();
	auto* created_window = g.windowMap[id];

	set_window_pos(created_window, 250, 100);
	set_window_size(created_window, 450, 750);

	created_window->idStack.push_back(id);
	
	created_window->windowPadding = vec2_t(7.0f, 7.0f);
	created_window->itemFlags = ITEM_FLAG_NONE;
	created_window->globalFontScale = 0.5f;
	created_window->globalFontScaleY = 0.5f;

	return created_window;
}

void menu::set_window_scroll_y(menu_window* window, float scroll)
{
	window->cursorMaxPos.y -= window->scrollPos.y;
	window->scrollTarget.y = scroll;
	window->cursorMaxPos.y += window->scrollPos.y;
}

void menu::new_frame()
{
	static uint64_t prev_time;

	constexpr uint64_t frequency = 1000000;

	menu_context& g = *gMenuCtx;

	navScoringRect = navResultRect;

	if (g.navResults.menuId != 0)
	{
		g.activeIdLast = g.activeId;
		g.activeId = g.navResults.menuId;
	}

	uint64_t currentTime = clock();
	g.deltaTime = prev_time > 0 ? static_cast<float>((double)(currentTime - prev_time) / frequency) : (float)(1.0f / 60.0f);
	prev_time = currentTime;

	g.time += g.deltaTime;
	g.frameCount += 1;

	if (g.currentWindow)
	{
		if (g.moveNavDir == NAV_DIR_DOWN || g.moveNavDir == NAV_DIR_UP && g.currentWindow->contentSize.y > g.currentWindow->innerClipRect.h)
		{
			menu_nav_results* results = &g.navResults;

			GBounds rect_abs = window_rect_rel_to_abs(g.currentWindow, results->rectRel);
			scroll_to_rect(g.currentWindow, rect_abs);
		}
	}

	g.moveNavDir = NAV_DIR_NONE;
	g.navResults.clear();

	if (g.currentWindow)
	{
		menu_window& window = *g.currentWindow;

		calc_content_size(&window, &window.contentSize, nullptr);
		static analog_input_t analog;
		read_analog_input(&analog);

		const float scroll_speed = (16.0f * 100 * g.deltaTime + 0.5f);
		const auto s = g_nav.is_down(NAV_RSTICK_UP) - g_nav.is_down(NAV_RSTICK_DOWN);

		if (analog.analogRight.y != 0.0f && window.contentSize.y > window.innerClipRect.h)
		{
			set_window_scroll_y(&window, (window.scrollPos.y + s * scroll_speed));
		}

		window.scrollPos = calc_next_scroll_and_clamp(&window);
		window.scrollTarget = vec2_t(FLT_MAX, FLT_MAX);

	}

	if (!g.isActive)
	{
		if (is_down(NAV_DPAD_DOWN, 1.0)) g.createNavRequest(NAV_DIR_DOWN);
		if (is_down(NAV_DPAD_UP, 1.0)) g.createNavRequest(NAV_DIR_UP);
		if (is_down(NAV_DPAD_LEFT, 1.0)) g.createNavRequest(NAV_DIR_LEFT);
		if (is_down(NAV_DPAD_RIGHT, 1.0)) g.createNavRequest(NAV_DIR_RIGHT);
	}

	g.startNavRequest();

	begin("boosie ui");
}

menu_id menu::get_id(const char* label, const char* str_end)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	menu_id seed = window.idStack.back();
	menu_id id = hash_str(label, str_end ? (str_end - label) : 0, seed);
	return id;
}

void menu::set_active_id(menu_id id)
{
	menu_context& g = *gMenuCtx;

	g.activeIdJustActivated = (g.activeIdJustActivated != id);
	g.activeId = id;
}

menu_window* menu::get_current_window()
{
	menu_context& g = *gMenuCtx; return g.currentWindow;
}

void menu::begin(const char* label)
{
	menu_context& g = *gMenuCtx;
	menu_style& style = g.style;

	const int headerHeight = 20 * ASPECT_RATIO;

	auto id = hash_str(label, 0, 0);

	menu_window* window = g.windowMap[id]; 
	if (!window)
	{
		window = create_window(label, id);
	}

	if (!window)
		return;

	GRect clip_rect(window->Pos.x, window->Pos.y + headerHeight, window->Size.x, window->Size.y - headerHeight);

	window->indentSize = 0.0f + window->windowPadding.x;

	double start_pos_x = (double)window->Pos.x + window->windowPadding.x - (double)window->scrollPos.x;
	double start_pos_y = (double)window->Pos.y + headerHeight + window->windowPadding.y - (double)window->scrollPos.y;

	window->innerClipRect = GRect(window->Pos.x, window->Pos.y + headerHeight, window->Size.x, window->Size.y);
	window->cursorStartPos = vec2_t((float)start_pos_x, (float)start_pos_y);
	window->cursorPosLossyness = vec2_t((float)(start_pos_x - window->cursorStartPos.x), (float)(start_pos_y - window->cursorStartPos.y));
	window->cursorPos = window->cursorStartPos;
	window->cursorPosPrevLine = window->cursorPos;
	window->cursorMaxPos = window->cursorStartPos;
	window->currLineSize = window->prevLineSize = vec2_t(0.0f, 0.0f);
	window->isSameLine = false;

	if (g.currentWindow)
	{
		menu_window& window = *g.currentWindow;

		navScoringRect.x = window.rectRel.x;
		navScoringRect.y = window.rectRel.y;
	}

	GRect header_rect(window->Pos.x, window->Pos.y, window->Size.x, headerHeight);
	GRect headertext_rect(window->Pos.x + window->windowPadding.x, window->Pos.y, window->Size.x - 10.f, headerHeight);

	g.currentWindow = window;

	render::add_filled_rect(window->innerClipRect, style.colors[COL_BACKGROUND], style.frameRounding, cornerFlags_Bot);

	render::add_filled_rect(header_rect, style.colors[COL_ITEM_BACKGROUND], style.frameRounding, cornerFlags_Top);
	render::add_text(label, headertext_rect, 1, vert_center | horz_left, .5f, .5f * ASPECT_RATIO, style.colors[COL_TEXT]);

	render::push_clip(clip_rect);
}

void menu::end()
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	if (window.contentSize.x > window.sizeFull.x)
		window.Size.x = window.contentSize.x + 10.0f;
	else
		window.Size.x = window.sizeFull.x;
	//window.cursorMaxPos.x = window.cursorPos.x;
	window.cursorMaxPos.y += 50.0f;

	//render::add_filled_rect(GRect(window.cursorMaxPos.x, window.cursorMaxPos.y, 300, 3), g.style.colors[COL_ITEM_BACKGROUND]);

	render::pop_clip();
}

bool menu::begin_tab_bar(const char* name)
{
	menu_context& g = *gMenuCtx;
	menu_style& style = g.style;

	auto id = get_id(name);

	menu_window* window = g.currentWindow;
	if (!window)
		return false;

	auto current_tab = window->tabMap[id];
	if (!current_tab)
	{
		window->tabMap[id] = new menu_tab_bar();
		return false;
	}

	g.currentTabBar = current_tab;

	GRect rect(window->cursorPos.x - window->windowPadding.x, window->cursorPos.y, window->innerClipRect.w, 20 * ASPECT_RATIO);
	GRect divider_line(rect.x, rect.y + rect.h - 2, rect.w, 2);

	render::add_filled_rect(divider_line, style.colors[COL_ITEM_BACKGROUND]);

	for (int i = 0; i < current_tab->tabItems.size(); i++)
	{
		if(i > 0)
			same_line();

		auto tab_item = current_tab->tabItems.find_by_index(i);

		auto tab_id = get_id(fmt_str("%s#tab_item", tab_item->name, i));

		auto tab_name_width = render::get_text_width(tab_item->name, .5f) + 10;

		GRect tab_name_rect(window->cursorPos.x, window->cursorPos.y, tab_name_width, rect.h);

		vec2_t size = vec2_t(tab_name_width, rect.h);

		item_size(size);
		if (!item_add(GRect(tab_name_rect.x, tab_name_rect.y, tab_name_rect.w, tab_name_rect.h), tab_id))
			return false;

		const bool active = g.activeId == tab_id;

		if (active && g_nav.is_pressed(NAV_ACTIVATE))
		{
			current_tab->activeTabIndex = i;
			current_tab->activeTabId = tab_id;
			current_tab->currentTabItem = tab_item;
		}

		render::add_filled_rect(tab_name_rect, current_tab->activeTabId == tab_id ? style.colors[COL_ITEM_ACTIVE] : style.colors[COL_ITEM_BACKGROUND], g.style.frameRounding, cornerFlags_Top);
		if (active) render::add_rect(tab_name_rect, 1, style.colors[COL_ACTIVE]);
		render::add_text(tab_item->name, tab_name_rect, 1, vert_center | horz_center, .5f, .5f * ASPECT_RATIO, style.colors[COL_TEXT]);
	}

	return true;
}

void menu::end_tab_bar()
{
}

bool menu::begin_tab_item(const char* name)
{
	menu_context& g = *gMenuCtx;

	menu_window* window = g.currentWindow;
	if (!window)
		return false;

	auto current_tab = g.currentTabBar;
	if (!current_tab)
		return false;

	auto id = get_id(fmt_str("%s#tab_item", name));

	auto current_tab_item = current_tab->tabItems[id];
	if (!current_tab_item)
	{
		current_tab->tabItems[id] = new menu_tab_item();
		strcpy(current_tab->tabItems[id]->name, name);
		return false;
	}

	if (!current_tab_item)
		return false;

	if (current_tab->activeTabId == id && current_tab->currentTabItem == current_tab_item)
	{
		return true;
	}

	return false;
}

void menu::end_tab_item()
{
}

void menu::set_active_id(int id)
{
}

bool menu::item_add(GRect rect, int id)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	if (!rect.overlaps(GRect(g.currentWindow->innerClipRect)))
		return false;

	if (id != 0)
	{
		GBounds navBounds(rect);
		GBounds navRectRel(navBounds.Min - window.Pos, navBounds.Max - window.Pos);

		g.lastItemData.ID = id;
		g.lastItemData.Rect = rect;
		g.lastItemData.rectRel = navBounds;

		if (g.navInitRequest && g.activeId != id)
		{
			g.navInitRequest = false;
			g.navResults.menuId = id;
			g.navResults.navRect = rect;
			navResultRect = rect;
			window.rectRel = rect;
		}

		if (g.activeId != id && !(window.itemFlags & ITEM_FLAG_NO_NAV))
		{
			bool newBest = g.navRequest && navScoreItem(&g.navResults);
			if (newBest)
			{
				g.navResults.menuId = id;
				g.navResults.rectRel = window_rect_rel_to_abs(&window, navRectRel);
				g.navResults.navRect = rect;
				navResultRect = rect;
			}
		}

		if (g.activeId == id)
		{
			window.rectRel = rect;
		}
	}

	return true;
}

void menu::item_size(vec2_t size, float text_baseline_y)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	const float offset_to_match_baseline_y = (text_baseline_y >= 0) ? std::max(0.0f, window.currLineTextBaseOffset - text_baseline_y) : 0.0f;

	const float line_y1 = window.isSameLine ? window.cursorPosPrevLine.y : window.cursorPos.y;
	const float line_height = std::max(window.currLineSize.y, window.cursorPos.y - line_y1 + size.y + offset_to_match_baseline_y);

	window.cursorPosPrevLine.x = window.cursorPos.x + size.x;
	window.cursorPosPrevLine.y = line_y1;
	window.cursorPos.x = floorf(window.Pos.x + window.indentSize);
	window.cursorPos.y = floorf(line_y1 + line_height + g.style.itemSpacing.y);
	window.cursorMaxPos.x = std::max(window.cursorMaxPos.x, window.cursorPosPrevLine.x);
	window.cursorMaxPos.y = std::max(window.cursorMaxPos.y, window.cursorPos.y - g.style.itemSpacing.y);
	window.cursorPrevMove = size;
	
	window.prevLineSize.y = line_height;
	window.currLineSize.y = 0.0f;
	window.prevLineTextBaseOffset = std::max(window.currLineTextBaseOffset, text_baseline_y);
	window.isSameLine = false;
}

void menu::same_line(float offset_from_start_x, float spacing_w)
{
	menu_context& g = *gMenuCtx;
	menu_window* window = g.currentWindow;

	if (offset_from_start_x != 0.0f)
	{
		if (spacing_w < 0.0f)
			spacing_w = 0.0f;
		window->cursorPos.x = window->Pos.x - window->scrollPos.x + offset_from_start_x + spacing_w;
		window->cursorPos.y = window->cursorPosPrevLine.y;
	}
	else
	{
		if (spacing_w < 0.0f)
			spacing_w = g.style.itemSpacing.x;
		window->cursorPos.x = window->cursorPosPrevLine.x + spacing_w;
		window->cursorPos.y = window->cursorPosPrevLine.y;
	}

	if (window->maxWidth < (window->cursorPos.x - window->cursorStartPos.x))
		window->maxWidth = (window->cursorPos.x - window->cursorStartPos.x);

	window->currLineSize = window->prevLineSize;
	window->currLineTextBaseOffset = window->prevLineTextBaseOffset;
	window->isSameLine = true;
}

void menu::push_style_color(int idx, GColor color)
{
	menu_context& g = *gMenuCtx;
	menu_col_mod mod;
	mod.idx = idx;
	mod.col = g.style.colors[idx];
	g.colModifiers.push_back(mod);
	g.style.colors[idx] = color;
}

void menu::pop_style_color(int count)
{
	menu_context& g = *gMenuCtx;
	while (count > 0)
	{
		menu_col_mod& mod = g.colModifiers.back();
		g.style.colors[mod.idx] = mod.col;
		g.colModifiers.pop_back();
		--count;
	}
}

void menu::push_item_flags(menu_item_flags flags, bool enabled)
{
	menu_window* window = get_current_window();
	if (enabled)
		window->itemFlags |= (flags);
	else
		window->itemFlags &= ~(flags);
	window->itemFlagStack.push_back(flags);
}

void menu::pop_item_flags()
{
	menu_window* window = get_current_window();
	window->itemFlagStack.pop_back();
	window->itemFlags = window->itemFlagStack.empty() ? ITEM_FLAG_NONE : window->itemFlagStack.back();
}

void menu::push_font_scale(float scale, float scale_y)
{
	menu_window* window = get_current_window();
	menu_font_mod mod;
	mod.backupScale = window->globalFontScale;
	mod.backupScaleY = window->globalFontScaleY;

	window->globalFontScale = scale;
	if (scale_y != 0.0f)
		window->globalFontScaleY = scale_y;

	window->fontScaleStack.push_back(mod);
}

void menu::pop_font_scale()
{
	menu_window* window = get_current_window();
	window->fontScaleStack.pop_back();
	menu_font_mod& mod = window->fontScaleStack.back();
	window->globalFontScale = window->fontScaleStack.empty() ? 0.5f : mod.backupScale;
	if(mod.backupScaleY != 0.0f)
		window->globalFontScaleY = window->fontScaleStack.empty() ? 0.5f : mod.backupScaleY;
}

void menu::push_id(const char* name)
{
	menu_context& g = *gMenuCtx;
	menu_window* window = get_current_window();
	auto id = hash(name);
	g.lastId = id;
	window->idStack.push_back(id);
}

void menu::push_id(int int_id)
{
	menu_context& g = *gMenuCtx;
	menu_window* window = get_current_window();
	auto id = hash((char*)&int_id);
	g.lastId = id;
	window->idStack.push_back(id);
}

void menu::pop_id()
{
	menu_window* window = get_current_window();
	window->idStack.pop_back();
}

vec2_t menu::calc_item_size(vec2_t size, float default_w, float default_h)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	if (size.x == 0.0f)
		size.x = default_w;
	else if (size.x < 0.0f)
		size.x = std::max(4.0f, window.cursorPos.x + size.x);

	if (size.y == 0.0f)
		size.y = default_h;
	else if (size.y < 0.0f)
		size.y = std::max(4.0f, window.cursorPos.y + size.y);

	return size;
}

bool menu::button(const char* label, vec2_t b_size)
{
	menu_context& g = *gMenuCtx;
	menu_style& style = g.style;
	menu_window& window = *g.currentWindow;

	auto id = get_id(label);

	const int height = 10 * ASPECT_RATIO;
	const float fontScale = window.globalFontScale;

	const bool active = g.activeId == id;

	vec2_t size = b_size == vec2_t() ? calc_item_size(vec2_t(), render::get_text_width(label, fontScale) + 10 + (g.style.framePadding.x * 2.0f), height + 15 + (g.style.framePadding.y * 2.0f)) : b_size;

	GRect rect(window.cursorPos.x, window.cursorPos.y, size.x, size.y);

	item_size(size);
	if (!item_add(rect, id))
		return false;

	render::add_filled_rect(rect, style.colors[COL_ITEM_BACKGROUND], style.frameRounding, cornerFlags_all);
	if (active) render::add_rect(rect, 1, style.colors[COL_ACTIVE]);
	render::add_text(label, rect, 1, vert_center | horz_center, fontScale, fontScale * ASPECT_RATIO, style.colors[COL_TEXT]);

	if(active)
		if (is_down(NAV_ACTIVATE, 1.f))
		{
			return true;
		}

	return false;
}

bool menu::checkbox(const char* label, bool* value, vec2_t b_size)
{
	menu_context& g = *gMenuCtx;
	menu_style& style = g.style;
	menu_window& window = *g.currentWindow;

	auto id = get_id(label);

	const int height = 10 * ASPECT_RATIO;
	const float fontScale = window.globalFontScale;

	const bool active = g.activeId == id;

	vec2_t size = b_size == vec2_t() ? calc_item_size(vec2_t(), render::get_text_width(label, fontScale) + 22, (height + 10 * (4 /3))) : b_size;

	GRect rect(window.cursorPos.x, window.cursorPos.y, 20, size.y);
	GRect text_rect(window.cursorPos.x + 24, window.cursorPos.y, size.x, size.y);

	item_size(size);
	if (!item_add(rect, id))
		return false;

	render::add_filled_rect(rect, *value ? style.colors[COL_ITEM_SLIDE] : style.colors[COL_ITEM_BACKGROUND], style.frameRounding, cornerFlags_all);
	if(active) render::add_rect(rect, 1, style.colors[COL_ACTIVE]);
	render::add_text(label, text_rect, 1, vert_center | horz_left, fontScale, fontScale * ASPECT_RATIO, style.colors[COL_TEXT]);

	if (active)
		if (is_down(NAV_ACTIVATE, 1.f))
		{
			*value ^= true;
			return true;
		}

	return false;
}

void menu::text(const char* label, ...)
{
	menu_context& g = *gMenuCtx;
	menu_style& style = g.style;
	menu_window& window = *g.currentWindow;

	const float fontScale = window.globalFontScale;

	auto id = get_id(label);

	char buffer[512];
	va_list args;
	va_start(args, label);
	vsnprintf(buffer, 512, label, args);
	va_end(args);

	const int height = render::get_text_height(buffer, fontScale) * ASPECT_RATIO;

	vec2_t size = vec2_t(render::get_text_width(buffer, fontScale) + 4.f, height + style.framePadding.y * 2.0f);

	GRect rect(window.cursorPos.x, window.cursorPos.y, size.x, size.y);

	item_size(size);
	if(!item_add(rect, 0))
		return;


	render::add_text(buffer, rect, 1, vert_center | horz_left, fontScale, fontScale * ASPECT_RATIO, style.colors[COL_TEXT]);
}


template<typename TYPE, typename SIGNEDTYPE, typename FLOATTYPE>
bool sliderBehaviorT(int data_type, TYPE* v, const TYPE v_min, const TYPE v_max, const char* format)
{
	menu_context& g = *gMenuCtx;

	bool value_changed = false;
	const bool is_floating_point = data_type == DATA_TYPE_FLOAT;
	const SIGNEDTYPE v_range = (v_min < v_max ? v_max - v_min : v_min - v_max);


	bool set_new_value = false;
	float clicked_t = 0.0f;

	if (g.activeIdJustActivated)
	{
		g.sliderCurrentAccum = 0.0f; // Reset any stored nav delta upon activation
		g.sliderCurrentAccumDirty = false;
	}

	float input_delta = g_nav.is_down(NAV_DPAD_RIGHT) - g_nav.is_down(NAV_DPAD_LEFT);
	if (input_delta != 0.0f)
	{
		const bool tweak_slow = true;
		const int decimal_precision = is_floating_point ? parseFormatPrecision(format, 3) : 0;
		if (decimal_precision > 0)
		{
			input_delta /= 100.0f;    // Gamepad/keyboard tweak speeds in % of slider bounds
			if (tweak_slow)
				input_delta /= 10.0f;
		}
		else
		{
			if ((v_range >= -100.0f && v_range <= 100.0f) || tweak_slow)
				input_delta = ((input_delta < 0.0f) ? -1.0f : +1.0f) / (float)v_range; // Gamepad/keyboard tweak speeds in integer steps
			else
				input_delta /= 100.0f;
		}

		if(data_type == DATA_TYPE_FLOAT)
			input_delta *= 20.0f;

		g.sliderCurrentAccum += input_delta;
		g.sliderCurrentAccumDirty = true;
	}

	float delta = g.sliderCurrentAccum;
	if (g.sliderCurrentAccumDirty)
	{
		clicked_t = scaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, *v, v_min, v_max, false, 0.0f, 0.0f);
		if ((clicked_t >= 1.0f && delta > 0.0f) || (clicked_t <= 0.0f && delta < 0.0f)) // This is to avoid applying the saturation when already past the limits
		{
			set_new_value = false;
			g.sliderCurrentAccum = 0.0f; // If pushing up against the limits, don't continue to accumulate
		}
		else
		{
			set_new_value = true;
			float old_clicked_t = clicked_t;
			clicked_t = saturate(clicked_t + delta);

			// Calculate what our "new" clicked_t will be, and thus how far we actually moved the slider, and subtract this from the accumulator
			TYPE v_new = scaleValueFromRatioT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, clicked_t, v_min, v_max, false, 0.0f, 0.0f);
			if (is_floating_point)
				v_new = roundScalarWithFormatT<TYPE>(format, data_type, v_new);
			float new_clicked_t = scaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, v_new, v_min, v_max, false, 0.0f, 0.0f);

			if (delta > 0)
				g.sliderCurrentAccum -= std::min(new_clicked_t - old_clicked_t, delta);
			else
				g.sliderCurrentAccum -= std::max(new_clicked_t - old_clicked_t, delta);
		}

		g.sliderCurrentAccum = false;
	}

	if (set_new_value)
	{
		TYPE v_new = scaleValueFromRatioT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, clicked_t, v_min, v_max, false, 0.0f, 0.0f);

		// Round to user desired precision based on format string
		if (is_floating_point)
			v_new = roundScalarWithFormatT<TYPE>(format, data_type, v_new);

		// Apply result
		if (*v != v_new)
		{
			*v = v_new;
			value_changed = true;
		}
	}

	return value_changed;
}

bool menu::slider_behaviour(void* value, int data_type, const char* format, const void* min, const void* max)
{
	menu_context& g = *gMenuCtx;

	if (!g.isActive)
		return false;

	if (g_nav.is_pressed(NAV_BACK))
		g.isActive = false;

	switch (data_type)
	{
	case DATA_TYPE_FLOAT:
		return sliderBehaviorT<float, float, float>(data_type, (float*)value, *(const float*)min, *(const float*)max, format);
	case DATA_TYPE_INT:
		return sliderBehaviorT<int, int, float>(data_type, (int*)value, *(const int*)min, *(const int*)max, format);
	}

	return false;
}

template<typename T>
bool menu::slider(const char* label, const char* fmt, T* values, int count, int data_type, T min, T max)
{
	menu_context& g = *gMenuCtx;
	menu_style& style = g.style;
	menu_window& window = *g.currentWindow;

	const int height = 10 * ASPECT_RATIO;

	bool modified = false;

	const float fontScale = window.globalFontScale - 0.07;

	vec2_t size = vec2_t(120, 20);

	for (int i = 0; i < count; i++)
	{
		if (i > 0)
			same_line(0, style.itemInnerSpacing.x);

		size = calc_item_size(vec2_t(), (360 / count) + (g.style.framePadding.x * 2.0f), (height + 10) + (g.style.framePadding.y * 2.0f));

		auto id = get_id(fmt_str("%s_%i", label, i));
		const char* str = fmt_str(fmt, values[i], max);

		const bool active = g.activeId == id;

		GRect rect(window.cursorPos.x, window.cursorPos.y, size.x, size.y);
		GRect rect_calc(window.cursorPos.x, window.cursorPos.y, ((((float)values[i] - (float)min) / ((float)max - (float)min)) * size.x), size.y);

		item_size(vec2_t(size.x, size.y));
		if (!item_add(rect, id))
			return false;

		if (active)
		{
			if (is_down(NAV_ACTIVATE, 1.f))
			{
				g.isActive = true;
				g.activeIdJustActivated = true;
			}
			modified = slider_behaviour((void*)&values[i], data_type, fmt, (const void*)&min, (const void*)&max);
		}

		render::add_filled_rect(rect, style.colors[COL_ITEM_BACKGROUND], style.frameRounding, cornerFlags_all);
		if (active) render::add_rect(rect, 1, style.colors[COL_TEXT]);
		if (values[i] != min)
			render::add_filled_rect(rect_calc, style.colors[COL_ITEM_SLIDE], style.frameRounding, cornerFlags_all);
		render::add_text(str, rect, 1, vert_center | horz_center, fontScale, fontScale * ASPECT_RATIO, style.colors[COL_TEXT]);
	}

	return modified;
}

bool menu::sliderf(const char* label, float* value, float inc, float min, float max, vec2_t b_size)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	bool edited = slider(label, "%.2f/%.2f", value, 1, DATA_TYPE_FLOAT, min, max);

	same_line();

	if (!(window.itemFlags & ITEM_FLAG_SLIDER_NO_TEXT))
		text(label);

	return edited;
}

bool menu::sliderf2(const char* label, float* values, float inc, float min, float max, vec2_t b_size)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	bool edited = slider(label, "%.2f/%.2f", values, 2, DATA_TYPE_FLOAT, min, max);

	same_line();

	if (!(window.itemFlags & ITEM_FLAG_SLIDER_NO_TEXT))
		text(label);

	return edited;
}

bool menu::sliderf3(const char* label, float* values, float inc, float min, float max, vec2_t b_size)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	bool edited = slider(label, "%.2f/%.2f", values, 3, DATA_TYPE_FLOAT, min, max);

	same_line();

	if (!(window.itemFlags & ITEM_FLAG_SLIDER_NO_TEXT))
		text(label);

	return edited;
}

bool menu::sliderf4(const char* label, float* values, float inc, float min, float max, vec2_t b_size)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	bool edited = slider(label, "%.2f/%.2f", values, 4, DATA_TYPE_FLOAT, min, max);

	same_line();

	if (!(window.itemFlags & ITEM_FLAG_SLIDER_NO_TEXT))
		text(label);

	return edited;
}

bool menu::slideru(const char* label, int* value, int inc, int min, int max, vec2_t b_size)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	bool edited = slider(label, "%u/%u", value, 1, DATA_TYPE_INT, min, max);

	same_line();

	if (!(window.itemFlags & ITEM_FLAG_SLIDER_NO_TEXT))
		text(label);

	return edited;
}


bool menu::slideru2(const char* label, int* values, int inc, int min, int max, vec2_t b_size)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	bool edited = slider(label, "%u/%u", values, 2, DATA_TYPE_INT, min, max);

	same_line();

	if (!(window.itemFlags & ITEM_FLAG_SLIDER_NO_TEXT))
		text(label);

	return edited;
}

bool menu::slideru3(const char* label, int* values, int inc, int min, int max, vec2_t b_size)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	bool edited = slider(label, "%u/%u", values, 3, DATA_TYPE_INT, min, max);

	same_line();

	if (!(window.itemFlags & ITEM_FLAG_SLIDER_NO_TEXT))
		text(label);

	return edited;
}

bool menu::slideru4(const char* label, int* values, int inc, int min, int max, vec2_t b_size)
{
	menu_context& g = *gMenuCtx;
	menu_window& window = *g.currentWindow;

	bool edited = slider(label, "%u/%u", values, 4, DATA_TYPE_INT, min, max);

	same_line();

	if (!(window.itemFlags & ITEM_FLAG_SLIDER_NO_TEXT))
		text(label);

	return edited;
}


void menu::end_frame()
{
	menu_context& g = *gMenuCtx;

	overlay_drawlist->render();

	g_nav.end_frame();

	g.stopNavRequest();
}

bool menu::is_down(int id, float repeat)
{
	return g_nav.is_down(id, repeat);
}

bool menu::is_pressed(int id)
{
	return g_nav.is_pressed(id);
}

bool menu::is_released(int id)
{
	return g_nav.is_released(id);
}

void menu::update(void* arg)
{
	g_nav.new_frame();

	menu_context& g = *gMenuCtx;
	menu_style& style = g.style;

	if (!g.open && is_down(NAV_R3) && is_down(NAV_R1))
		g.open = true;

	if (!g.open)
		return;

	if (!g.isActive && is_down(NAV_BACK, 1.f))
		g.open = false;

	new_frame();

	menu_window& window = *get_current_window();

	text("boosie ui by faultz :)");
	text("g.isActive: %i", g.isActive); same_line(); 
	push_style_color(COL_TEXT, GColor(255, 0, 0));
	text("g.activeId: 0x%08X", g.activeId); same_line();
	pop_style_color();
	text("g.navRequest: %i", g.navRequest);

	text("window.Pos(%.1f, %.1f)", window.Pos.x, window.Pos.y);
	text("window.Size(%.1f, %.1f)", window.Size.x, window.Size.y);
	text("g.style.framePadding(%.1f, %.1f)", style.framePadding.x, style.framePadding.y);
	text("g.style.frameRounding(%.1f)", style.frameRounding);
	text("g.style.itemSpacing(%.1f, %.1f)", style.itemSpacing.x, style.itemSpacing.y);
	text("g.style.itemInnerSpacing(%.1f, %.1f)", style.itemInnerSpacing.x, style.itemInnerSpacing.y);

	sliderf("frameRounding", &style.frameRounding, 0.5f, 0.0, 35.0f);
	sliderf2("framePadding", style.framePadding, 0.5f, 0.0, 15.0f);
	sliderf2("itemSpacing", style.itemSpacing, 0.5f, 0.0, 15.0f);
	sliderf2("itemInnerSpacing", style.itemInnerSpacing, 0.5f, 0.0, 15.0f);

	static float val3[2];
	sliderf2("slider 3", val3, 1.0f, 0.0f, 1000.0f);

	static int i_size;
	slideru("label", &i_size, 0, 0, 100);

	static float val;
	sliderf("slider 1", &val, 1.0f, 0.0f, 100.0f);

	if (begin_tab_bar("settings#bar"))
	{
		if (begin_tab_item("settings"))
		{
			sliderf4("Background color", style.colors[COL_BACKGROUND], .03f, 0.f, 1.f);
			sliderf4("Item color", style.colors[COL_ITEM_BACKGROUND], .03f, 0.f, 1.f);
			sliderf4("Active color", style.colors[COL_ACTIVE], .03f, 0.f, 1.f);
			sliderf4("Active slider color", style.colors[COL_ITEM_SLIDE], .03f, 0.f, 1.f);
			sliderf4("Text color", style.colors[COL_TEXT], .03f, 0.f, 1.f);
			sliderf4("Active text color", style.colors[COL_TEXT_ACTIVE], .03f, 0.f, 1.f);

			static bool v;
			checkbox("new checkbox", &v);
			same_line();
			checkbox("new checkbox3", &v);

			end_tab_item();
		}
		if (begin_tab_item("new item 2"))
		{
			button("tab_item 2");

			if (begin_tab_bar("tabs2"))
			{
				if (begin_tab_item("new item2"))
				{
					button("tab_item 12");
					sliderf4("Color background2", style.colors[COL_BACKGROUND], .03f, 0.f, 1.f);

					end_tab_item();
				}
				if (begin_tab_item("new item 22"))
				{
					button("tab_item 22");

					end_tab_item();
				}

				end_tab_bar();
			}

			end_tab_item();
		}

		end_tab_bar();
	}

	static int count;
	if (button(fmt_str("button outside of tab item #%i", count)))
		count++;

	end();

	//begin("hex memory");
	//set_window_pos(g.currentWindow, 100, 100);
	//set_window_size(g.currentWindow, 300, 400);

	//text("very early test version");

	//int index = 0;
	//char value = 0;
	//menu_list_clipper clipper;
	//clipper.begin(10);         // We have 1000 elements, evenly spaced.
	//while (clipper.step())
	//{
	//	for (int i = clipper.displayStart; i < clipper.displayEnd; i++)
	//	{
	//		char vb[16];
	//		libpsutil::memory::get(0x10040000 + index, &vb, 1);

	//		text("0x%08X: %*s", 0x10040000 + index, 16, "");
	//		index++;
	//	}
	//}

	//end();

	end_frame();
}

void menu::start()
{
	g_materialCommands = reinterpret_cast<materialCommands_t*>(materialCommands_a);
	g_materialCommands->indexCount = g_materialCommands->indexCount;

	for (int i = 0; i < 12; i++)
	{
		const float a = ((float)i * 2 * M_PI) / (float)12;
		circle_vtx_fast[i] = vec2_t(cosf(a), sinf(a));
	}

	create_context();

	scheduler::schedule(update, 0u, scheduler::render);

	cell_pad_get_data_d = new detour(cellPad_GetData_t, cellPad_GetData);
}

void menu::stop()
{
	destroy_context();
	delete cell_pad_get_data_d;
}
