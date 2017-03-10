#ifndef Cone_Tracer_H
#define Cone_Tracer_H

#include <DirectXMath.h>
#include "Common/d3dApp.h"

class ConeTracer
{
public:
	ConeTracer();
	~ConeTracer();

	void Init(ID3D11Device* idevice, ID3D11DeviceContext* ideviceContext);
	void SetMatrix(const DirectX::XMMATRIX* iWorld, const DirectX::XMMATRIX* iView, const DirectX::XMMATRIX * iProj, const DirectX::XMFLOAT3 icamPos);
	void Render(ID3D11ShaderResourceView* iVoxelList);
private:
	void BuildFX();
	void BuildVertexLayout();

	void InitLightMat();
private:

	//input var
	ID3D11Device* md3dDevice;
	ID3D11DeviceContext* mDeviceContext;

	//effect var
	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;

	ID3DX11EffectMatrixVariable* mfxView;
	ID3DX11EffectMatrixVariable* mfxWorld;
	ID3DX11EffectMatrixVariable* mfxProj;

	ID3DX11EffectVariable* mfxPointLight;
	ID3DX11EffectVariable* mfxMat;
	ID3DX11EffectVectorVariable* mfxEyePos;
	ID3DX11EffectShaderResourceVariable* mfxVoxelList;

	//buffer
	ID3D11InputLayout* mInputLayout;

	//init class
	PointLight mPointLight;
	Material mMat;

	DirectX::XMFLOAT3 mEyePos;
};

#endif // Cone_Tracer_H