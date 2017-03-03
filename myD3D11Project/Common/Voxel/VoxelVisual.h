#ifndef VOXEL_VISUAL_H
#define VOXEL_VISUAL_H

#include <DirectXMath.h>
#include "d3dApp.h"

class Visualizer
{
public:
	Visualizer();
	~Visualizer();

	void Init(ID3D11Device* idevice, ID3D11DeviceContext* ideviceContext);

	void Render(ID3D11ShaderResourceView* iVoxelList, float iRes, const DirectX::XMMATRIX* iView, const DirectX::XMMATRIX * iProj, const DirectX::XMMATRIX * iWorld);
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
	ID3DX11EffectMatrixVariable* mfxProj;
	ID3DX11EffectMatrixVariable* mfxWorld;

	ID3DX11EffectScalarVariable* mfxVoxelSize;
	ID3DX11EffectShaderResourceVariable* mfxEdgeTex;
	ID3DX11EffectShaderResourceVariable* mfxVoxelList;
};

#endif // VOXEL_VISUAL_H