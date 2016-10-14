#pragma once

#include <DirectXMath.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include "myMathLibrary.h"

class myShapeLibrary {
public:
	struct Vertex
	{
		Vertex()
			:Position(0.0f,0.0f,0.0f),Normal(0.0f,0.0f,0.0f) {}
		Vertex(const DirectX::XMFLOAT3 &p)
			: Position(p),Normal(0.0f,0.0f,0.0f) {}
		Vertex(float px, float py, float pz)
			: Position(px,py,pz), Normal(0.0f, 0.0f, 0.0f) {}

		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Normal;
	};

	struct MeshData
	{
		std::vector <Vertex> vertices;
		std::vector <UINT> indices;
	};

	void CreateBox(float width, float height, float depth,MeshData &meshData);
private:
	void getNorm(MeshData &meshData);
};
