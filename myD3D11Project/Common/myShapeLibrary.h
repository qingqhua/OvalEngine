#pragma once

#include <DirectXMath.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include "myMathLibrary.h"
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>
#include "octree.h"

class myShapeLibrary {
public:
	struct Vertex
	{
		Vertex() {}
		Vertex(float px, float py, float pz)
			: Position(px, py, pz) {}
		Vertex(const DirectX::XMFLOAT3 &p)
			: Position(p.x, p.y, p.z) {}
		Vertex(const DirectX::XMFLOAT3 &p,const DirectX::XMFLOAT2 &uv)
			: Position(p),Texture(uv) {}
		Vertex(float px, float py, float pz,float u,float v)
			: Position(px,py,pz),Texture(u,v){}

		DirectX::XMFLOAT3 Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT3 Normal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT2 Texture = DirectX::XMFLOAT2(0.0f, 0.0f);
	};

	struct MeshData
	{
		std::vector <Vertex> vertices;
		std::vector <unsigned int> indices;
	};

	void CreateQuad(MeshData &meshdata);
	void CreateBox(DirectX::XMFLOAT3 center, float extent, MeshData &meshData);
	void LoadModel(const char *file, MeshData &meshData);
private:
	void ComputeNorm(MeshData &meshData);
};
