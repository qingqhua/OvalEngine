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

	void Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float res, float voxelsize, DirectX::XMFLOAT3 voxeloffset);
	void SetMatrix(const DirectX::XMMATRIX* world, const DirectX::XMMATRIX * worldInverTrans, const DirectX::XMMATRIX* view, const DirectX::XMMATRIX * proj, const DirectX::XMFLOAT3 camPos);

	void Render(float totalTime, int indexcount);

	void resetOMTargetsAndViewport();

	void Clear();

	ID3D11ShaderResourceView* GetSRV();
	float GetRes();
	float GetvoxelSize();
	
	DirectX::XMMATRIX GetProjMatrix();
private:
	void BuildFX();
	void BuildVertexLayout();
	void BuildTexture();

	void BuildRenderTarget();
private:

	//input var
	ID3D11Device* md3dDevice;
	ID3D11DeviceContext* mDeviceContext;

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
	ID3DX11EffectScalarVariable* mfxVoxelSize;
	ID3DX11EffectVectorVariable* mfxVoxelOffset;
	ID3DX11EffectUnorderedAccessViewVariable* mfxUAVColor;
	ID3DX11EffectScalarVariable* mfxTime;

	//LIGHTING
	ID3DX11EffectVariable* mfxPointLight;
	ID3DX11EffectVariable* mfxMat;
	ID3DX11EffectVectorVariable* mfxEyePos;

	//init class
	MyLightLibrary::PointLightBRDF mPointLight;
	MyLightLibrary::MaterialBRDF mMat;

	DirectX::XMFLOAT3 mEyePos;

	//buffer
	ID3D11InputLayout* mInputLayout;

	//shader var
	float mVoxelSize;
	DirectX::XMFLOAT3 mVoxelOffset;
	float mRes;

	// render texture variable
	ID3D11Texture3D* mTex3D;
	ID3D11ShaderResourceView* mSRV;
	ID3D11UnorderedAccessView* mUAV;

	ID3D11Texture2D* mTex2D;
	ID3D11RenderTargetView* mRenderTargetView;

	ID3D11ShaderResourceView* m_Tex2DSRV;
	ID3D11Texture2D* mdepthStencilBuffer;
	ID3D11DepthStencilView* mdepthStencilView;

	D3D11_VIEWPORT mViewport;

	DirectX::XMMATRIX mprojectionMatrix;
};

#endif // VOXELIZER_H