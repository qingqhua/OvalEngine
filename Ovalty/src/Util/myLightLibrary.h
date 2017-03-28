#pragma once

#include <windows.h>
#include <DirectXMath.h>
#include <iostream>

class MyLightLibrary
{
public:
	static struct Material
	{
		Material() { ZeroMemory(this, sizeof(this)); }
		DirectX::XMFLOAT4 Diffuse;
		DirectX::XMFLOAT4 Specular;
	};

	static struct DirectionalLight
	{
		DirectionalLight() { ZeroMemory(this, sizeof(this)); }
		DirectX::XMFLOAT4 Diffuse;
		DirectX::XMFLOAT4 Ambient;
		DirectX::XMFLOAT4 Specular;
		DirectX::XMFLOAT3 Direction;
	};

	static struct PointLight
	{
		PointLight() { ZeroMemory(this, sizeof(this)); }

		DirectX::XMFLOAT4 Diffuse;
		DirectX::XMFLOAT4 Specular;

		DirectX::XMFLOAT3 Position;
		float Range;

		DirectX::XMFLOAT3 Attenuation;
		float Pad;

	};

	static struct MaterialBRDF
	{
		MaterialBRDF() { ZeroMemory(this, sizeof(this)); }
		DirectX::XMFLOAT3 DiffAlbedo;
		float metallic;
		DirectX::XMFLOAT3 SpecAlbedo;
		float roughness;
	};

	static struct PointLightBRDF
	{
		PointLightBRDF() { ZeroMemory(this, sizeof(this)); }

		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Color;
	};

	static void SetLightMaterial(ID3DX11EffectVariable* fxL, ID3DX11EffectVariable* fxM, float t)
	{
		//update light
		PointLightBRDF L[2];
		MaterialBRDF M;

		L[0].Position = DirectX::XMFLOAT3(0.5f*cosf(0.7f*t), 1.0f, -2.0f+0.7f*sinf(0.5f*t));
		//L[0].Position = DirectX::XMFLOAT3(0.5f, 1.0f, -5.0f);
		L[0].Color = DirectX::XMFLOAT3(0.6f, 0.6f, 0.6f);
		L[1].Position = DirectX::XMFLOAT3(0.5f, 1.0f, -2.0f);
		L[1].Color = DirectX::XMFLOAT3(0.8f, 0.5f, 0.3f);
		fxL->SetRawValue(&L, 0, sizeof(L));

		//Update Material
		M.DiffAlbedo = DirectX::XMFLOAT3(0.8f, 0.5f, 0.3f);
		M.SpecAlbedo = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
		M.metallic = 0.0f;
		M.roughness = 0.2f;
		fxM->SetRawValue(&M, 0, sizeof(M));
	}
};






