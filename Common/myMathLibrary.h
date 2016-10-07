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
		A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
		return DirectX::XMMatrixTranspose(XMMatrixInverse(&det, A));
	}

	static const float Infinity;
	static const float Pi;

};