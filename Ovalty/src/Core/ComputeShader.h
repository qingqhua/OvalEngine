#ifndef COMPUTE_SHADER_H
#define COMPUTE_SHADER_H

#include <DirectXMath.h>
#include "Core/d3dApp.h"

class Compute
{
public:
	Compute();
	~Compute();

	void Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float res, float voxelsize, DirectX::XMFLOAT3 offset);
	void Render(ID3D11ShaderResourceView* voxelList, const DirectX::XMMATRIX* view, const DirectX::XMMATRIX * proj, const DirectX::XMMATRIX * world, const DirectX::XMMATRIX * worldInverTrans, const DirectX::XMFLOAT3 camPos, float totalTime);

private:
	void BuildFX();

private:

	//input var
	ID3D11Device* md3dDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11InputLayout* mInputLayout;

	//effect var
	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;

	//coordinates
	ID3DX11EffectMatrixVariable* mfxView;
	ID3DX11EffectMatrixVariable* mfxProj;
	ID3DX11EffectMatrixVariable* mfxWorld;
	ID3DX11EffectMatrixVariable* mfxWorldInverTrans;
	ID3DX11EffectVectorVariable* mfxEyePosW;

	//voxel
	ID3DX11EffectScalarVariable* mfxVoxelSize;
	ID3DX11EffectVectorVariable* mfxVoxelOffset;
	ID3DX11EffectScalarVariable* mfxDim;

	//voxel list
	ID3DX11EffectShaderResourceVariable* mfxVoxelList;

	//time
	ID3DX11EffectScalarVariable* mfxTime;


	float mRes;
	float mVoxelSize;
	DirectX::XMFLOAT3 mOffset;
};

#endif // COMPUTE_SHADER_H