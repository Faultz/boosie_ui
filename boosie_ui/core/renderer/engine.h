#pragma once

extern materialCommands_t* g_materialCommands;

class renderables
{
public:
	static void start();
	static void stop();
};

void add_vertex_data(int quadCount, std::uint16_t* indices, int& indexCount, unsigned int& vertCount, GfxVertex* vert, GfxQuadVertex*& verts);
