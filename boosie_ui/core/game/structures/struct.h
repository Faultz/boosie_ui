#pragma once

struct pointer_t
{
	template<class T>
	T get(uint32_t pos)
	{
		return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(this) + pos);
	}
	template<class T>
	void set(T* pointer)
	{
		*(T**)this = (T*)pointer;
	}
};

struct Font
{
	const char* fontName;
	int pixelHeight;
};

struct Material 
{
	const char* materialName;
};

struct ScreenPlacement : pointer_t
{
	vec2_t scaleVirtualToReal;
	vec2_t scaleVirtualToFull;
	vec2_t scaleRealToVirtual;
	vec2_t virtualViewableMin;
	vec2_t virtualViewableMax;
	vec2_t virtualTweakableMin;
	vec2_t virtualTweakableMax;
	vec2_t realViewportBase;
	vec2_t realViewportSize;
	vec2_t realViewportMid;
	vec2_t realViewableMin;
	vec2_t realViewableMax;
	vec2_t realTweakableMin;
	vec2_t realTweakableMax;
	vec2_t subScreen;
	float hudSplitscreenScale;
}; 

union GfxColor
{
	GfxColor(unsigned int packed) : packed(packed) {}
	char array[4];
	unsigned int packed;
};

struct GfxQuadVertex
{
	vec2_t xy;
	vec2_t st;
	GfxColor color;
};

union PackedUnitVec
{
	unsigned int packed;
	char array[4];
};

struct GfxVertex
{
	vec4_t xyzw;
	GfxColor color;
	vec2_t texCoord;
	PackedUnitVec normal;
};

struct __declspec(align(8)) materialCommands_t
{
	GfxVertex verts[2048];
	unsigned short indices[3072];
	int vertDeclType;
	unsigned int vertexSize;
	unsigned int indexCount;
	unsigned int vertexCount;
	unsigned int firstVertex;
	unsigned int lastVertex;
	bool finishedFilling;
};

struct GfxRenderCommandExecState
{
	int cmd;
};

struct GfxCmdHeader
{
	unsigned short id;
	unsigned short byteCount;
};

struct GfxCmdDrawQuadPicOriginal
{
	GfxCmdHeader header;
	Material* material;
	vec2_t verts[4];
	GfxColor color;
};

struct GfxCmdDrawQuadPic
{
	GfxCmdHeader header;
	Material* material;
	vec2_t verts[4];
	GfxColor color[4];
	float s0;
	float t0;
	float s1;
	float t1;
	float rotation;
	//int multicolor;
};

struct GfxCmdArray
{
	unsigned char* cmds;
	int usedTotal;
	int usedCritical;
	GfxCmdHeader* lastCmd;
};

struct GfxCmdDrawQuadList2D
{
	GfxCmdHeader header;
	Material* material;
	int quadCount;
};