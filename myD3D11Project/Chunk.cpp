#include"Chunk.h"

Chunk::Chunk()
{
	myShapeLibrary::MeshData voxel;
	myShapeLibrary shape;
	shape.Voxel(voxel);
	for (int i = 0; i < chunk_size; i++)
		for (int j = 0; j < chunk_size; j++)
				meshData[i][j] = voxel;
}