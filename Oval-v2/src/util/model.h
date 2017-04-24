//-----------------------------------
//FILE			: model.h
//DESCRIPTION	: Model class
//PARENT		: graphics class 
//NODE			: EMPTY
//REFERENCE		: d3dcoder.net, rastertek.com
//-----------------------------------

#ifndef MODEL_H
#define MODEL_H

#include "tiny_obj_loader.h"

#include <d3d11.h>
#include <DirectXMath.h>

class model
{
public:
	model();
	~model();
	bool Init(ID3D11Device* device, const char* filename, const char* basepath);
	void Shutdown();
	void Render(ID3D11DeviceContext* context);

	int GetIndexCount();
	//ID3D11ShaderResourceView* GetTexture();

private:
	bool InitBuffer(ID3D11Device* device, const char* filename, const char* basepath);
	void ShutdownBuffer();
	void RenderBuffer(ID3D11DeviceContext* context);

	bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* context, char* filename);
	void ReleaseTexture();

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

private:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texture;
	};
	
};

#endif //MODEL_H

