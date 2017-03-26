#pragma once

#include <Windows.h>
#include <DirectXMath.h>


class myMathLibrary
{
public:

	template<typename T>
	static float Clamp(const T &x, const T &low, const T &high)
	{
		return x < low ? low : (x > high ? high : x);
	}

	static float Random(float min, float max)
	{
		return min + (float)(rand())/RAND_MAX * (max-min);
	}

	static DirectX::XMMATRIX InverseTranspose(DirectX::CXMMATRIX M)
	{
		DirectX::XMMATRIX A = M;

		//prevent 4th column to multiply to view matrix later
		A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
		return DirectX::XMMatrixTranspose(XMMatrixInverse(&det, A));
	}

	static DirectX::XMFLOAT3 myMathLibrary::add(DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2)
	{
		return DirectX::XMFLOAT3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
	}

	static DirectX::XMFLOAT3 myMathLibrary::sub(DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2)
	{
		return DirectX::XMFLOAT3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
	}

	static DirectX::XMFLOAT3 myMathLibrary::cross(DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2)
	{
		return DirectX::XMFLOAT3(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
	}

	static DirectX::XMFLOAT3 myMathLibrary::normlize(DirectX::XMFLOAT3 v)
	{
		float tmp = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
		return DirectX::XMFLOAT3(v.x / tmp, v.y / tmp, v.z / tmp);
	}

	static const float Infinity;
	static const float Pi;
};