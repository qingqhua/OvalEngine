#pragma once
#include"myShapeLibrary.h"
class Chunk
{
public:
	Chunk();
	static const int chunk_size = 16;
	myShapeLibrary::MeshData meshData[chunk_size][chunk_size];

};