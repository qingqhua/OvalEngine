#ifndef SHAPE_LIBRARY_H
#define SHAPE_LIBRARY_H

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include "Util/myMathLibrary.h"
#include "3rdParty/tiny_obj_loader.h"

class myShapeLibrary {
private:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texture;
	};

public:
	myShapeLibrary();
	~myShapeLibrary();

	void Init(ID3D11Device* device, const char* filename, const char* basepath);
	void Render(ID3D11DeviceContext* context);

	int GetIndexCount();

	DirectX::BoundingBox GetAABB();

private:
	void InitBuffer(ID3D11Device* device, const char* filename, const char* basepath);
	void RenderBuffer(ID3D11DeviceContext* context);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;

	int m_vertexCount, m_indexCount;

	Vertex* m_vertices;
};

#endif // SHAPE_LIBRARY_H
