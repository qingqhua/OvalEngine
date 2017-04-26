//-----------------------------------
//FILE			: model.h
//DESCRIPTION	: Model class
//PARENT		: graphics class 
//NODE			: EMPTY
//REFERENCE		: d3dcoder.net, rastertek.com
//-----------------------------------

#ifndef MODEL_H
#define MODEL_H

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "tiny_obj_loader.h"

class model
{
private:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texture;
	};

public:
	model();
	~model();
	bool Init(ID3D11Device* device, const char* filename, const char* basepath);
	void Shutdown();
	void Render(ID3D11DeviceContext* context);

	int GetIndexCount();

	DirectX::BoundingBox GetAABB();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitBuffer(ID3D11Device* device, const char* filename, const char* basepath);
	void ShutdownBuffer();
	void RenderBuffer(ID3D11DeviceContext* context);

	bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* context, char* filename);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	ID3D11ShaderResourceView *m_texture;

	Vertex* m_vertices;
};

#endif //MODEL_H

