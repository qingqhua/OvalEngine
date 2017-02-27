#ifndef VOXELIZER_H
#define VOXELIZER_H

#include <DirectXMath.h>
#include "d3dApp.h"

struct VoxelData
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	float ID;
};

class Voxelizer
{
public:
	Voxelizer();
	~Voxelizer();

	void Init(ID3D11Device* idevice, ID3D11DeviceContext* ideviceContext, UINT iRes);

	void SetMatrix(const DirectX::XMMATRIX* iWorld, const DirectX::XMMATRIX* iView, const DirectX::XMMATRIX * iProj);
	void Render();

private:
	void BuildFX();
	void BuildVertexLayout();
	void BuildTexture();

private:

	//input var
	ID3D11Device* md3dDevice;
	ID3D11DeviceContext* mDeviceContext;

	UINT mWidth;
	UINT mHeight;
	UINT mDepth;

	//effect var
	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;

	ID3DX11EffectMatrixVariable* mfxView;
	ID3DX11EffectMatrixVariable* mfxWorld;
	ID3DX11EffectMatrixVariable* mfxProj;

	ID3DX11EffectScalarVariable* mfxRes;
	ID3DX11EffectScalarVariable* mfxObjID;
	ID3DX11EffectVectorVariable* mfxVoxelSize;
	ID3DX11EffectUnorderedAccessViewVariable* mfxTargetUAV;

	//texture var
	ID3D11Texture3D* mTex3D;
	ID3D11ShaderResourceView* mSRV;
	ID3D11ShaderResourceView* mSrvList;
	ID3D11UnorderedAccessView* mUAV;
	ID3D11UnorderedAccessView *mUavList;

	//buffer
	ID3D11InputLayout* mInputLayout;

	//shader var
	DirectX::XMFLOAT3 mVoxelSize;
	float mID;
};

#endif // VOXELIZER_H