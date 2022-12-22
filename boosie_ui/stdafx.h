#pragma once

#include <cellstatus.h>
#include <sys/prx.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <sys/prx.h>
#include <sys/syscall.h>
#include <sys/ppu_thread.h>
#include <sys/sys_time.h>
#include <sys/time_util.h>
#include <sys/process.h>
#include <sys/memory.h>
#include <sys/timer.h>
#include <sys/types.h>
#include <math.h>
#include <fastmath.h>
#include <cellstatus.h>
#include <sys/timer.h>
#include <cell/sysmodule.h>
#include <sys/random_number.h>
#include <ppu_intrinsics.h>
#include <spu_printf.h>
#include <ctype.h>
#include <libpsutil.h>
#include <cell\pad.h>
#include <cell\padfilter.h>
#include <cell\pad.h>
#include <cellstatus.h>
#include <sys/integertypes.h>
#include <algorithm>
#include <stack>
#include <limits>
#include <sysutil/sysutil_sysparam.h>
#include <sys/synchronization.h>
#include <cell/gcm.h>

static cell::Gcm::Inline::CellGcmContext& g_gcmContext = *reinterpret_cast<cell::Gcm::Inline::CellGcmContext*>(0x02530CCC);

#define BREAK __asm("tw 31, %r1, %r1")
#define zero_memory(a, b) memset(a, b, sizeof(a));

inline uint64_t get_time()
{
	return sys_time_get_system_time() / 1000;
}

inline uint32_t hash(const char* str)
{
    uint64_t hash = 0xCBF29CE484222325;

    while (*str)
    {
        hash =
            (hash ^ uint32_t((*str >= 'A' && *str <= 'Z') ? *str - ('A' - 'a') : *str)) * 0x100000001B3;
        str++;
    }

    return hash;
}
typedef void(*menu_callback_t)(void* arg);

using namespace libpsutil;
using namespace libpsutil::math;

template <typename T = float> static T clamp(const T& value, const T& low, const T& high)
{
	return value < low ? low : (value > high ? high : value);
}
inline vec2_t clampV(const vec2_t& f, const vec2_t& mn, vec2_t mx) { return vec2_t(clamp(f.x, mn.x, mx.x), clamp(f.y, mn.y, mx.y)); }
inline vec2_t minV(const vec2_t& lhs, const vec2_t& rhs) { return vec2_t(lhs.x < rhs.x ? lhs.x : rhs.x, lhs.y < rhs.y ? lhs.y : rhs.y); }
inline vec2_t maxV(const vec2_t& lhs, const vec2_t& rhs) { return vec2_t(lhs.x >= rhs.x ? lhs.x : rhs.x, lhs.y >= rhs.y ? lhs.y : rhs.y); }

typedef memory::detour detour;

#include "core/game/structures/struct.h"
#include "util/color.h"
#include "core/game/globals.h"

extern float ASPECT_RATIO;

namespace globals
{
	extern uint64_t tick;
	extern float delta_time;
	extern float globalAlpha;
	extern bool exitModule;
	extern Material* gameWhite;
	extern Font* fontNormal;
	extern CellVideoOutResolution resolution;
	extern GColor clr_white;
	extern symbol<int(char*, size_t, const char*, ...)> game_snprintf;
	extern symbol<int(char*, size_t, const char*, std::_Va_list)> game_vsnprintf;
}

#ifdef snprintf
#undef snprintf
#endif

#define snprintf globals::game_snprintf
#define vsnprintf globals::game_vsnprintf
//#ifdef vsnprintf
////#undef vsnprintf
//#endif

//#define vsnprintf globals::game_vsnprintf

template<int size = 256>
class fmt_str_n
{
public:
	fmt_str_n() = default;
	template<typename ...args>
	fmt_str_n(const char* buf, args... arguments)
	{
		snprintf(data, size, buf, arguments...);
	}

	fmt_str_n& operator=(const char* value)
	{
		return *this;
	}

	fmt_str_n& operator=(const std::string& value)
	{
		return *this;
	}

	operator const char* ()
	{
		return data;
	}

	operator char* ()
	{
		return data;
	}

	operator std::string()
	{
		return data;
	}

	char data[size];
};
typedef fmt_str_n<256> fmt_str;

#define IM_FREE _sys_free
#define IM_ALLOC _sys_malloc
#define IM_ASSERT(x) x

template<typename T>
struct vector
{
	int Size;
	int Capacity;
	T* Data;

	// Provide standard typedefs but we don't use them ourselves.
	typedef T value_type;
	typedef value_type* iterator;
	typedef const value_type* const_iterator;

	// Constructors, destructor
	inline vector() { Size = Capacity = 0; Data = NULL; }
	inline vector(const vector<T>& src) { Size = Capacity = 0; Data = NULL; operator=(src); }
	inline vector(const std::initializer_list<T>& src) { clear(); resize(src.size() - 1); if (src.begin()) memcpy(Data, src.begin(), (size_t)Size * sizeof(T); return *this; ) }
	inline vector<T>& operator=(const vector<T>& src) { clear(); resize(src.Size); if (src.Data) memcpy(Data, src.Data, (size_t)Size * sizeof(T)); return *this; }
	inline ~vector() { if (Data) IM_FREE(Data); } // Important: does not destruct anything

	inline void         clear() { if (Data) { Size = Capacity = 0; IM_FREE(Data); Data = NULL; } }  // Important: does not destruct anything
	inline void         clear_delete() { for (int n = 0; n < Size; n++) IM_DELETE(Data[n]); clear(); }     // Important: never called automatically! always explicit.
	inline void         clear_destruct() { for (int n = 0; n < Size; n++) Data[n].~T(); clear(); }           // Important: never called automatically! always explicit.

	inline bool         empty() const { return Size == 0; }
	inline int          size() const { return Size; }
	inline int          size_in_bytes() const { return Size * (int)sizeof(T); }
	inline int          max_size() const { return 0x7FFFFFFF / (int)sizeof(T); }
	inline int          capacity() const { return Capacity; }
	inline T& operator[](int i) { return Data[i]; }
	inline const T& operator[](int i) const { IM_ASSERT(i >= 0 && i < Size); return Data[i]; }

	inline T* begin() { return Data; }
	inline const T* begin() const { return Data; }
	inline T* end() { return Data + Size; }
	inline const T* end() const { return Data + Size; }
	inline T& front() { IM_ASSERT(Size > 0); return Data[0]; }
	inline const T& front() const { IM_ASSERT(Size > 0); return Data[0]; }
	inline T& back() { return Data[Size - 1]; }
	inline const T& back() const { IM_ASSERT(Size > 0); return Data[Size - 1]; }
	inline void         swap(vector<T>& rhs) { int rhs_size = rhs.Size; rhs.Size = Size; Size = rhs_size; int rhs_cap = rhs.Capacity; rhs.Capacity = Capacity; Capacity = rhs_cap; T* rhs_data = rhs.Data; rhs.Data = Data; Data = rhs_data; }

	inline int          _grow_capacity(int sz) const { int new_capacity = Capacity ? (Capacity + Capacity / 2) : 8; return new_capacity > sz ? new_capacity : sz; }
	inline void         resize(int new_size) { if (new_size > Capacity) reserve(_grow_capacity(new_size)); Size = new_size; }
	inline void         resize(int new_size, const T& v) { if (new_size > Capacity) reserve(_grow_capacity(new_size)); if (new_size > Size) for (int n = Size; n < new_size; n++) memcpy(&Data[n], &v, sizeof(v)); Size = new_size; }
	inline void         shrink(int new_size) { IM_ASSERT(new_size <= Size); Size = new_size; } // Resize a vector to a smaller size, guaranteed not to cause a reallocation
	inline void         reserve(int new_capacity) { if (new_capacity <= Capacity) return; T* new_data = (T*)IM_ALLOC((size_t)new_capacity * sizeof(T)); if (Data) { memcpy(new_data, Data, (size_t)Size * sizeof(T)); IM_FREE(Data); } Data = new_data; Capacity = new_capacity; }
	inline void         reserve_discard(int new_capacity) { if (new_capacity <= Capacity) return; if (Data) IM_FREE(Data); Data = (T*)IM_ALLOC((size_t)new_capacity * sizeof(T)); Capacity = new_capacity; }

	// NB: It is illegal to call push_back/push_front/insert with a reference pointing inside the ImVector data itself! e.g. v.push_back(v[10]) is forbidden.
	inline void         push_back(const T& v) { if (Size == Capacity) reserve(_grow_capacity(Size + 1)); memcpy(&Data[Size], &v, sizeof(v)); Size++; }
	inline void         pop_back() { Size--; }
	inline void         push_front(const T& v) { if (Size == 0) push_back(v); else insert(Data, v); }
	inline T* erase(const T* it) { const ptrdiff_t off = it - Data; memmove(Data + off, Data + off + 1, ((size_t)Size - (size_t)off - 1) * sizeof(T)); Size--; return Data + off; }
	inline T* erase(const T* it, const T* it_last) { const ptrdiff_t count = it_last - it; const ptrdiff_t off = it - Data; memmove(Data + off, Data + off + count, ((size_t)Size - (size_t)off - (size_t)count) * sizeof(T)); Size -= (int)count; return Data + off; }
	inline T* erase_unsorted(const T* it) { const ptrdiff_t off = it - Data; if (it < Data + Size - 1) memcpy(Data + off, Data + Size - 1, sizeof(T)); Size--; return Data + off; }
	inline T* insert(const T* it, const T& v) { const ptrdiff_t off = it - Data; if (Size == Capacity) reserve(_grow_capacity(Size + 1)); if (off < (int)Size) memmove(Data + off + 1, Data + off, ((size_t)Size - (size_t)off) * sizeof(T)); memcpy(&Data[off], &v, sizeof(v)); Size++; return Data + off; }
	inline bool         contains(const T& v) const { const T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data++ == v) return true; return false; }
	inline T* find(const T& v) { T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data == v) break; else ++data; return data; }
	inline const T* find(const T& v) const { const T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data == v) break; else ++data; return data; }
	inline bool         find_erase(const T& v) { const T* it = find(v); if (it < Data + Size) { erase(it); return true; } return false; }
	inline bool         find_erase_unsorted(const T& v) { const T* it = find(v); if (it < Data + Size) { erase_unsorted(it); return true; } return false; }
	inline int          index_from_ptr(const T* it) const { const ptrdiff_t off = it - Data; return (int)off; }
};

template <typename Key, typename Value>
class map {
public:
	typedef vector<std::pair<Key, Value>> value_list;

	// constructor
	map() : Data() {}
	~map() { Data.clear(); }
	void insert(const Key& key, const Value& value) {
		Data.push_back(std::make_pair(key, value));
	}
	void clear() {
		Data.clear();
	}
	bool empty() const {
		return Data.empty();
	}
	bool contains(const Key& key) const {
		for (const auto& pair : Data) {
			if (pair.first == key) {
				return true;
			}
		}
		return false;
	}		
	Value find_by_index(const int& idx) {
		for (int i = 0; i < Data.size(); i++)
		{
			if (i == idx)
				return Data[i].second;
		}

		return nullptr;
	}
	Value* find(const Key& key) {
		for (auto& pair : Data) {
			if (pair.first == key) {
				return &pair.second;
			}
		}
		return nullptr;
	}
	Value& at(const Key& key) {
		for (auto& pair : Data) {
			if (pair.first == key) {
				return pair.second;
			}
		}
	}

	std::vector<Key> keys() const {
		std::vector<Key> keys;
		for (const auto& pair : Data) {
			keys.push_back(pair.first);
		}
		return keys;
	}
	void erase(const Key& key) {
		for (auto it = Data.begin(); it != Data.end(); ++it) {
			if (it->first == key) {
				Data.erase(it);
				break;
			}
		}
	}
	size_t size() const {
		return Data.size();
	}

	Value& operator[](const Key& key) {
		for (auto& pair : Data) {
			if (pair.first == key) {
				return pair.second;
			}
		}
		Data.push_back(std::make_pair(key, Value()));
		return Data.back().second;
	}

	value_list Data;
};
struct menu_storage
{
	// [Internal]
	struct menu_storage_pair
	{
		int key;
		union { int val_i; float val_f; void* val_p; };
		menu_storage_pair(int _key, int _val_i) { key = _key; val_i = _val_i; }
		menu_storage_pair(int _key, float _val_f) { key = _key; val_f = _val_f; }
		menu_storage_pair(int _key, void* _val_p) { key = _key; val_p = _val_p; }
	};

	vector<menu_storage_pair> Data;

	void Clear() { Data.clear(); }
	int GetInt(int key, int default_val = 0) const;
	void SetInt(int key, int val);
	bool GetBool(int key, bool default_val = false) const;
	void SetBool(int key, bool val);
	float GetFloat(int key, float default_val = 0.0f) const;
	void SetFloat(int key, float val);
	void* GetVoidPtr(int key) const; // default_val is NULL
	void SetVoidPtr(int key, void* val);

	int* GetIntRef(int key, int default_val = 0);
	bool* GetBoolRef(int key, bool default_val = false);
	float* GetFloatRef(int key, float default_val = 0.0f);
	void** GetVoidPtrRef(int key, void* default_val = NULL);

	void SetAllInt(int val);

	void BuildSortByKey();
};

struct timer
{
public:

	bool ready()
	{
		return ((get_time() - ticker) >= waiter);
	};

	void wait(int timeout)
	{
		ticker = get_time();
		waiter = timeout;
	};
	int ticker;
	int waiter;
};

#define WHILE_LOADED while(!globals::exitModule) {
#define END_WHILE }

#include "util/std_util.h"
#include "util/navigation.h"
#include "util/easing.h"
#include "util/animation.h"
#include "util/scheduler.h"

#include "core/game/functions.h"

#include "core/renderer/engine.h"
#include "core/renderer/render.h"
#include "core/renderer/logging.h"

#include "core/renderer/draw/draw_list.h"

#include "core/menu/menu.h"