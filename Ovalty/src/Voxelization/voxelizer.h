#ifndef VOXELIZER_H
#define VOXELIZER_H

#include <DirectXMath.h>
#include "Core/d3dApp.h"

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

	void Init(ID3D11Device* idevice, ID3D11DeviceContext* ideviceContext, float iRes, float imaxSize);
	void SetMatrix(const DirectX::XMMATRIX* iWorld, const DirectX::XMMATRIX * iWorldInverTrans, const DirectX::XMMATRIX* iView, const DirectX::XMMATRIX * iProj, const DirectX::XMFLOAT3 icamPos);
	void Render(float totalTime);

	ID3D11ShaderResourceView* SRV();
	float Res();

private:
	void BuildFX();
	void BuildVertexLayout();
	void BuildTexture();

private:

	//input var
	ID3D11Device* md3dDevice;
	ID3D11DeviceContext* mDeviceContext;
	D3D11_VIEWPORT mViewport;

	float mWidth;
	float mHeight;
	float mDepth;

	//effect var
	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;

	ID3DX11EffectMatrixVariable* mfxView;
	ID3DX11EffectMatrixVariable* mfxWorld;
	ID3DX11EffectMatrixVariable* mfxWorldInverTrans;
	ID3DX11EffectMatrixVariable* mfxProj;

	ID3DX11EffectScalarVariable* mfxDim;
	ID3DX11EffectVectorVariable* mfxVoxelSize;
	ID3DX11EffectUnorderedAccessViewVariable* mfxUAVColor;
	//ID3DX11EffectUnorderedAccessViewVariable* mfxUAVPosW;

	//texture var
	ID3D11Texture3D* mTex3D;
	ID3D11ShaderResourceView* mSRV;
	ID3D11UnorderedAccessView* mUAV;

	//LIGHTING
	ID3DX11EffectVariable* mfxPointLight;
	ID3DX11EffectVariable* mfxMat;
	ID3DX11EffectVectorVariable* mfxEyePos;

	//init class
	PointLightBRDF mPointLight;
	MaterialBRDF mMat;

	DirectX::XMFLOAT3 mEyePos;

	//buffer
	ID3D11InputLayout* mInputLayout;

	//shader var
	DirectX::XMFLOAT3 mVoxelSize;
	float mRes;
};

#endif // VOXELIZER_H