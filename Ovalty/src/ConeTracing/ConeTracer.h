#ifndef Cone_Tracer_H
#define Cone_Tracer_H

#include <DirectXMath.h>
#include "Core/d3dApp.h"

class ConeTracer
{
public:
	ConeTracer();
	~ConeTracer();

	void Init(ID3D11Device* idevice, ID3D11DeviceContext* ideviceContext, float res, float voxelsize, DirectX::XMFLOAT3 offset);
	void SetMatrix(const DirectX::XMMATRIX* iWorld, const DirectX::XMMATRIX * iWorldInverTrans, const DirectX::XMMATRIX* iView, const DirectX::XMMATRIX * iProj, const DirectX::XMFLOAT3 icamPos);
	void Render(ID3D11ShaderResourceView* iVoxelList, float totalTime);
private:
	void BuildFX();
	void BuildVertexLayout();

private:

	//input var
	ID3D11Device* md3dDevice;
	ID3D11DeviceContext* mDeviceContext;

	//effect var
	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;

	ID3DX11EffectMatrixVariable* mfxView;
	ID3DX11EffectMatrixVariable* mfxWorldInverTrans;
	ID3DX11EffectMatrixVariable* mfxWorld;
	ID3DX11EffectMatrixVariable* mfxProj;
	ID3DX11EffectVectorVariable* mfxEyePos;

	ID3DX11EffectVariable* mfxPointLight;
	ID3DX11EffectVariable* mfxMat;
	
	ID3DX11EffectShaderResourceVariable* mfxVoxelList;

	ID3DX11EffectScalarVariable* mfxVoxelSize;
	ID3DX11EffectVectorVariable* mfxVoxelOffset;
	ID3DX11EffectScalarVariable* mfxDim;

	ID3DX11EffectScalarVariable* mfxTime;

	//buffer
	ID3D11InputLayout* mInputLayout;

	//init class
	MyLightLibrary::PointLightBRDF mPointLight;
	MyLightLibrary::MaterialBRDF mMat;

	DirectX::XMFLOAT3 mEyePos;
	float mRes;
	float mVoxelSize;
	DirectX::XMFLOAT3 mOffset;
};

#endif // Cone_Tracer_H