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
		Vertex(float px, float py, float pz)
			: Position(px, py, pz) {}
		Vertex(const DirectX::XMFLOAT3 &n)
			: Normal(n.x, n.y, n.z) {}
		Vertex(float u, float v)
			: Texture(u, v) {}
		Vertex(const DirectX::XMFLOAT3 &p,const DirectX::XMFLOAT2 &uv)
			: Position(p),Texture(uv) {}
		Vertex(float px, float py, float pz, float n1, float n2, float n3)
			: Position(px,py,pz), Normal(n1, n2, n3) {}
		Vertex(float px, float py, float pz, float n1, float n2, float n3,float u, float v)
			: Position(px, py, pz), Normal(n1, n2, n3),Texture(u, v) {}

		DirectX::XMFLOAT3 Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT3 Normal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT2 Texture = DirectX::XMFLOAT2(0.0f, 0.0f);
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
