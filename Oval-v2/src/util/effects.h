//-----------------------------------
//FILE			: EFFECTS_H_
//DESCRIPTION	: compile shader
//PARENT		: shaderApp class 
//NODE			: EMPTY
//REFERENCE		: d3dCoder.net
//-----------------------------------
#ifndef EFFECTS_H_
#define EFFECTS_H_

#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>

#include "d3dx11effect.h"

//default : matrix constant buffer	: cb_Matrix

class Effect
{
private:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	struct CommonBufferType
	{
		DirectX::XMFLOAT3 eyeposW;
		float time;
	};

	struct VoxelBufferType
	{
		DirectX::XMFLOAT3 offset;
		float dimension;
		float size;
	};

	struct MaterialBufferType
	{
		DirectX::XMFLOAT3 ka;
		DirectX::XMFLOAT3 kd;
		DirectX::XMFLOAT3 ks;
	};

public:
	Effect();
	~Effect();

	//compile shader
	void Init(ID3D11Device* device, LPCWSTR filename);

	//apply shader and draw
	void Render(ID3D11DeviceContext* context);

	void Shutdown();

	//---
	//setup
	//---
	//create per vertex inputlayout, default is pos, normal, tex
	void CreateInputLayout(ID3D11Device* device);

	//set srv to load uav data
	void SetSRVList();

	//set texture3d and unordered access view
	void SetTex3dUAV(ID3D11Device* device, float width);


	//---
	//set constant buffer paramter
	//---
	//voxel
	void SetUpdateVoxelParameter(float dim, const DirectX::BoundingBox* AABB);
	//time and eyeposw
	void SetCommonParamter();
	//world proj view matrix
	void SetMatrixParamter();

	//reset viewport
	void ResetViewport(ID3D11DeviceContext* context, float width, float height);


	//---
	//update constant buffer paramter
	//---
	//world proj view matrix
	void UpdateMatrixParameters(const DirectX::XMMATRIX *world, const DirectX::XMMATRIX *view, const DirectX::XMMATRIX *proj);
	//time and eyeposw
	void UpdateCommonParameters(float time, DirectX::XMFLOAT3 eyeposW);

	//update uav  data to shader
	void UpdateUAVParameters();

	//load srv from saved uav
	void UpdateSRVParamters(ID3D11ShaderResourceView* srvList);

	//return srv to read uav data
	ID3D11ShaderResourceView* GetSRV();

	void UpdateRenderTarget(ID3D11DeviceContext* context);
	void ClearRenderTargetDepth(ID3D11DeviceContext* deviceContext);



private:
	//compile fx
	void Compile(ID3D11Device* device, LPCWSTR filename);

	//set tech : index 0
	void SetTechParamter();
	
private:
	ID3DX11Effect* m_FX;

	ID3DX11EffectTechnique* m_Tech;

	ID3DX11EffectVariable *m_cbMatrix;
	ID3DX11EffectVariable *m_cbCommon;
	ID3DX11EffectVariable *m_cbVoxel;

	ID3DX11EffectUnorderedAccessViewVariable* m_fxUAV;
	ID3DX11EffectShaderResourceVariable* m_fxSRVList;

	ID3D11InputLayout* m_InputLayout;
	ID3D11SamplerState* m_sampleState;

	ID3D11Texture3D* m_Tex3D;
	ID3D11ShaderResourceView* m_SRV;
	ID3D11UnorderedAccessView* m_UAV;

	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RenderTargetView* m_renderTargetView;
};

#endif //EFFECTS_H_