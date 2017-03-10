#pragma once

#include <windows.h>
#include <DirectXMath.h>
#include <iostream>

struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); }
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;
};

struct DirectionalLight
{
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Specular;
	DirectX::XMFLOAT3 Direction;
};

struct PointLight
{
	PointLight() { ZeroMemory(this, sizeof(this)); }
	//XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;

	DirectX::XMFLOAT3 Position;
	float Range;

	DirectX::XMFLOAT3 Attenuation;
	float Pad;
};


