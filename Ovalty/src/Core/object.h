//***************************************************************************************
// render different objects
//***************************************************************************************
#ifndef Object_H
#define Object_H

#include "Core/d3dApp.h"

class Object
{
public:
	Object();
	~Object();

	void Init(ID3D11Device* idevice, ID3D11DeviceContext* ideviceContext, float res, float voxelsize, DirectX::XMFLOAT3 offset);
	void SetMatrix(const DirectX::XMMATRIX* iWorld, const DirectX::XMMATRIX * iWorldInverTrans, const DirectX::XMMATRIX* iView, const DirectX::XMMATRIX * iProj, const DirectX::XMFLOAT3 icamPos);
	void Render(ID3D11ShaderResourceView* iVoxelList, float totalTime);
	void BuildFX();
	void BuildVertexLayout();
	void LoadModel(const char* filename);

	DirectX::BoundingBox boundingbox();
	int indiceNum();
	unsigned int verticeByteWidth();
	unsigned int indiceByteWidth();

	myShapeLibrary::MeshData model;
private:
	
	myShapeLibrary shapes;
};

#endif // Cone_Tracer_H