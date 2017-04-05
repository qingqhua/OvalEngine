#pragma once

#include <DirectXMath.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include "Util/myMathLibrary.h"
#include "Octree/octree.h"
#include "3rdParty/tiny_obj_loader.h"

class myShapeLibrary {
public:
	struct Vertex
	{
		Vertex() {}
		Vertex(DirectX::XMFLOAT3 p)
			: Position(p.x, p.y, p.z) {}
		Vertex(float px, float py, float pz, float n1, float n2, float n3)
			: Position(px, py, pz), Normal(n1, n2, n3) {}

		DirectX::XMFLOAT3 Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT3 Normal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	};

	struct MeshData
	{
		std::vector <Vertex> vertices;
		std::vector <int> indiceindex;
	};

	void CreateBox(DirectX::XMFLOAT3 center, float extent, MeshData &meshData);

	void LoadFromTinyObj(const char* filename, const char* basepath /*= NULL*/, bool triangulate /*= true*/, MeshData &meshData);

	DirectX::BoundingBox GetAABB(MeshData meshdata);
private:
	void ComputeNorm(MeshData &meshData);
};
