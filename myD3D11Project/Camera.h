#pragma once

#include "d3dUtil.h"

class Camera
{
public:
	Camera();
	~Camera();
	DirectX::XMVECTOR GetPositionXM() const { return DirectX::XMLoadFloat3(&mPosition); }
	DirectX::XMFLOAT3 GetPosition() const{ return mPosition; }
	void setPosition(float x, float y, float z) { mPosition = DirectX::XMFLOAT3(x, y, z); }
	void setPostion(const DirectX::XMFLOAT3 &v){ mPosition = v; }

	DirectX::XMVECTOR GetRightXM() const { return DirectX::XMLoadFloat3(&mRight); }
	DirectX::XMFLOAT3 GetRight() const { return mRight; }
	DirectX::XMVECTOR GetUpXM() const { return DirectX::XMLoadFloat3(&mUp); }
	DirectX::XMFLOAT3 GetUp() const { return mUp; }
	DirectX::XMVECTOR GetLookXM() const { return DirectX::XMLoadFloat3(&mLook); }
	DirectX::XMFLOAT3 GetLook() const { return mLook; }

	float GetNearWindowWidth()const { return mNearWindowHeight *mAspect; }
	float GetNearWindowHeight()const { return mNearWindowHeight; }
	float GetFarWindowWidth()const { return mFarWindowHeight*mAspect; }
	float GetFarWindowHeight()const { return mFarWindowHeight; }

	float GetNearZ()const { return mNearZ; }
	float GetFarZ()const { return mFarZ; }
	float GetAspect()const { return mAspect; }
	float GetFovY()const { return mFovY; }
	float GetFovX()const { return 2.0f*atan(GetNearWindowWidth()/2/mNearZ); }

	void setLens(float fovY, float aspect, float zn, float zf);
	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

	void Walk(float d);
	void Strafe(float d);
	void Pitch(float angle);
	void RotateY(float angle);

	void UpdateViewMatrix();

	DirectX::XMMATRIX View() const { return DirectX::XMLoadFloat4x4(&mView); }
	DirectX::XMMATRIX Proj() const { return DirectX::XMLoadFloat4x4(&mProj); }
	DirectX::XMMATRIX ViewProj() const { return DirectX::XMMatrixMultiply(View(), Proj()); }

private:
	DirectX::XMFLOAT3 mPosition;
	DirectX::XMFLOAT3 mRight;
	DirectX::XMFLOAT3 mLook;
	DirectX::XMFLOAT3 mUp;

	float mNearZ;
	float mFarZ;
	float mAspect;
	float mFovY;
	float mNearWindowHeight;
	float mFarWindowHeight;
	
	DirectX::XMFLOAT4X4 mView;
	DirectX::XMFLOAT4X4 mProj;
};
