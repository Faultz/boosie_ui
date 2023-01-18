#pragma once

// hook addresses
#define cellPad_GetData_t 0x08DDD84 
#define R_EndFrame_t 0x076BD90

// function addresses
#define R_AddCmdDrawStretchPic_t 0x076A3E8
#define CG_DrawRotatedPicPhysical_t 0x00456F8
#define CL_DrawStretchPicPhysicalRotateXYInternal_t 0x010B378
#define R_AddCmdDrawText_t 0x076A910
#define Material_RegisterHandle_t 0x0763220
#define R_RegisterFont_t 0x075A2C0
#define R_TextWidth_t 0x075A338
#define Com_Sprintf_t 0x03E4970
#define DB_GetAllXAssetOfType_t 0x018C4A4
#define DB_FindXAssetHeader_t 0x018AC04
#define DrawText2D_t 0x079AE00
#define R_AddCmdDrawTextWithEffects_t 0x076AB48
#define R_ConvertColor_t 0x0794980

#define R_AddCmdSetScissorRect_t 0x076B7D4

#define RB_TessOverflow_t 0x07B4750
#define RB_EndTessSurface_t 0x07B4238
#define R_SetTessMaterial_t 0x07B46BC

#define R_GetCommandBuffer_t 0x0769F6C

// quad pic id
#define RB_DrawQuadPic_id_t 14

// structure addresses
#define GfxCmdArray_t 0x22AC990
#define RenderCmdBufSize_t 0x02176E08
#define RenderCommandTable_t 0x09AFF30
#define materialCommands_a 0x2F71620

static color c_white{ 1.f, 1.f, 1.f, 1.f };

static symbol<void(char*, int, const char* ...)> Com_Sprintf{ Com_Sprintf_t };

static symbol<void* (bool, int, int, int, int)> R_AddCmdSetScissorRect{ R_AddCmdSetScissorRect_t };

static symbol<void(float x, float y, float w, float h, float s0, float t0, float s1, float t1, const float* color, Material* material)> R_AddCmdDrawStretchPic{ R_AddCmdDrawStretchPic_t };
static symbol<void(ScreenPlacement* scrPlace, float x, float y, float width, float height, float angle, const float* color, Material* material)> CG_DrawRotatedPicPhysical{ CG_DrawRotatedPicPhysical_t };
static symbol<void(float x, float y, float w, float h, float s1, float t1, float s2, float t2, float angle, const float* color, Material* material)>  CL_DrawStretchPicPhysicalRotateXYInternal{ CL_DrawStretchPicPhysicalRotateXYInternal_t };
static symbol<void(const char* text, unsigned int maxChars, Font* font, float x, float y, float xScale, float yScale, float rotation, const float* color, int style)> R_AddCmdDrawText{ R_AddCmdDrawText_t };

static symbol<int(int localClient, const char* text, int maxChars, Font* font)> R_TextWidth{ R_TextWidth_t };
static symbol<Material* (const char* name, int imageTrack, bool, int)> Material_RegisterHandle{ Material_RegisterHandle_t };
static symbol<Font* (const char* text, int track)> R_RegisterFont{ R_RegisterFont_t };
static symbol<void(const char* text,
	float x, float y, float w,
	Font* font, float xScale, float yScale,
	float sinAngle, float cosAngle, unsigned int color,
	int maxLength, int renderFlags, int cursorPos,
	char cursorLetter, float padding, unsigned int glowForcedColor,
	int fxBirthTime, int fxLetterTime, int fxDecayStartTime,
	int fxDecayDuration, int fxRedactDecayStartTime, int fxRedactDecayDuration,
	Material* fxMaterial, Material* fxMaterialGlow)> DrawText2D{ DrawText2D_t };
static symbol<void(const char*, int, Font*, float, float, float, float, float, float, float*, int, float*, Material*, Material*, int, int, int, int)> R_AddCmdDrawTextWithEffects{ R_AddCmdDrawTextWithEffects_t };
static symbol<void(float*, uint32_t*)> R_ConvertColor{ R_ConvertColor_t };


static symbol<void()> RB_TessOverflow{ RB_TessOverflow_t };
static symbol<void()> RB_EndTessSurface{ RB_EndTessSurface_t };
static symbol<void(Material*, int, int)> R_SetTessMaterial{ R_SetTessMaterial_t };
static symbol<void* (int cmd, int size)> R_GetCommandBuffer{ R_GetCommandBuffer_t };