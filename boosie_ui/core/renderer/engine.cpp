#include "stdafx.h"

typedef struct
{
	float m[2][2];
} mat2_t;

struct opd_t
{
	unsigned int address;
	unsigned int toc;
};

opd_t** g_renderCommandTable = reinterpret_cast<opd_t**>(RenderCommandTable_t);
materialCommands_t* g_materialCommands;

void add_vertex_data(int quadCount, std::uint16_t* indices, int& indexCount, unsigned int& vertCount, GfxVertex* vert, GfxQuadVertex*& verts)
{
	for (int i = 0; i < quadCount; i++)
	{
		indices[indexCount] = vertCount + 3;
		indices[indexCount + 1] = vertCount;
		indices[indexCount + 2] = vertCount + 2;
		indices[indexCount + 3] = vertCount + 2;
		indices[indexCount + 4] = vertCount;
		indices[indexCount + 5] = vertCount + 1;

		R_SetVertex4D(&vert[vertCount], verts[0].xy.x, verts[0].xy.y, 0.0f, 1.0f, 0.f, 0.f, verts[0].color.packed);
		R_SetVertex4D(&vert[vertCount + 1], verts[1].xy.x, verts[1].xy.y, 0.0f, 1.0f, 1.f, 0.f, verts[1].color.packed);
		R_SetVertex4D(&vert[vertCount + 2], verts[2].xy.x, verts[2].xy.y, 0.0f, 1.0f, 1.f, 1.f, verts[2].color.packed);
		R_SetVertex4D(&vert[vertCount + 3], verts[3].xy.x, verts[3].xy.y, 0.0f, 1.0f, 0.f, 1.f, verts[3].color.packed);

		verts += 4;
		vertCount += 4;
		indexCount += 6;
	}
}

void RB_DrawQuadList2D(GfxRenderCommandExecState* execState)
{
	GfxCmdDrawQuadList2D* cmd = (GfxCmdDrawQuadList2D*)execState->cmd;

	auto materialCmd = g_materialCommands;

	unsigned int vertCount = materialCmd->vertexCount;
	int indexCount = materialCmd->indexCount;

	GfxVertex* vert = materialCmd->verts;
	uint16_t* indices = materialCmd->indices;

	int quadCount = cmd->quadCount;

	R_SetTessMaterial(cmd->material, 4);

	if (((vertCount * quadCount) + 4) > 0x800 || ((indexCount * quadCount) + 6) > 0xC00)
	{
		//RB_TessOverflow();
		vertCount = materialCmd->vertexCount;
		indexCount = materialCmd->indexCount;
	}

	materialCmd->vertexCount += 4 * quadCount;
	materialCmd->indexCount += 6 * quadCount;

	GfxQuadVertex* verts = reinterpret_cast<GfxQuadVertex*>(&cmd[1]);

	add_vertex_data(quadCount, indices, indexCount, vertCount, vert, verts);
}

detour* RB_DrawQuadPic_d = nullptr;
void RB_DrawQuadPic(GfxRenderCommandExecState* execState)
{
	GfxCmdDrawQuadPic* cmd = (GfxCmdDrawQuadPic*)execState->cmd;

	if (cmd->header.byteCount == sizeof(GfxCmdDrawQuadPic))
	{
		auto materialCmd = g_materialCommands;

		int vertCount = materialCmd->vertexCount;
		int indexCount = materialCmd->indexCount;

		GfxVertex* vert = materialCmd->verts;
		uint16_t* indices = materialCmd->indices;

		float rotation_angle = cmd->rotation;

		// Create 2D rotation matrix from rotation angle
		mat2_t rotation_matrix;
		rotation_matrix.m[0][0] = cosf(rotation_angle);
		rotation_matrix.m[0][1] = -sinf(rotation_angle);
		rotation_matrix.m[1][0] = sinf(rotation_angle);
		rotation_matrix.m[1][1] = cosf(rotation_angle);

		// Apply rotation matrix to each vertex position
		vec2_t rotated_positions[4];
		for (int i = 0; i < 4; i++)
		{
			rotated_positions[i].x = cmd->verts[i].x * rotation_matrix.m[0][0] + cmd->verts[i].y * rotation_matrix.m[0][1];
			rotated_positions[i].y = cmd->verts[i].x * rotation_matrix.m[1][0] + cmd->verts[i].y * rotation_matrix.m[1][1];
		}

		R_SetTessMaterial(cmd->material, 4);

		if ((vertCount + 4) > 0x800 || (indexCount + 6) > 0xC00)
		{
			//RB_TessOverflow();
			vertCount = materialCmd->vertexCount;
			indexCount = materialCmd->indexCount;
		}

		materialCmd->vertexCount += 4;
		materialCmd->indexCount += 6;

		indices[indexCount] = vertCount + 3;
		indices[indexCount + 1] = vertCount;
		indices[indexCount + 2] = vertCount + 2;
		indices[indexCount + 3] = vertCount + 2;
		indices[indexCount + 4] = vertCount;
		indices[indexCount + 5] = vertCount + 1;

		R_SetVertex4D(&vert[vertCount], rotated_positions[0].x, rotated_positions[0].y, 0.0f, 1.0f, cmd->s0, cmd->t0, cmd->color[0].packed);
		R_SetVertex4D(&vert[vertCount + 1], rotated_positions[1].x, rotated_positions[1].y, 0.0f, 1.0f, cmd->s1, cmd->t0, cmd->color[1].packed);
		R_SetVertex4D(&vert[vertCount + 2], rotated_positions[2].x, rotated_positions[2].y, 0.0f, 1.0f, cmd->s1, cmd->t1, cmd->color[2].packed);
		R_SetVertex4D(&vert[vertCount + 3], rotated_positions[3].x, rotated_positions[3].y, 0.0f, 1.0f, cmd->s0, cmd->t1, cmd->color[3].packed);

		execState->cmd = execState->cmd + cmd->header.byteCount;
	}
	else if(cmd->header.byteCount == 0x5C)
	{
		RB_DrawQuadList2D(execState);
	}
	else
	{
		RB_DrawQuadPic_d->invoke(execState);
	}
}

void renderables::start()
{
	g_materialCommands = reinterpret_cast<materialCommands_t*>(materialCommands_a);
	g_materialCommands->indexCount = g_materialCommands->indexCount;

	RB_DrawQuadPic_d = new detour(g_renderCommandTable[RB_DrawQuadPic_id_t]->address, RB_DrawQuadPic);
}

void renderables::stop()
{
	delete RB_DrawQuadPic_d;
}