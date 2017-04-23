//-----------------------------------
//FILE			: camera.h
//DESCRIPTION	: CAMERA class
//PARENT		: graphics class 
//NODE			: EMPTY
//REFERENCE		: d3dcoder.net
//-----------------------------------

#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>
#include <math.h>

class camera
{
public:
	camera();
	~camera();

	// Get world position of camera.
	DirectX::XMVECTOR GetPositionXM()const;
	DirectX::XMFLOAT3 GetPosition()const;

	// Set world position of camera.
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& v);

	// Get camera basis vectors.
	DirectX::XMVECTOR GetRightXM()const;
	DirectX::XMFLOAT3 GetRight()const;
	DirectX::XMVECTOR GetUpXM()const;
	DirectX::XMFLOAT3 GetUp()const;
	DirectX::XMVECTOR GetLookXM()const;
	DirectX::XMFLOAT3 GetLook()const;

	// Get frustum properties.
	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float GetFovX()const;

	// Get near and far plane dimensions in view space coordinates.
	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;

	// Set frustum.
	void SetLens(float fovY, float aspect, float zn, float zf);

	// Define camera space via LookAt parameters.
	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

	// Get World matrices.
	DirectX::XMMATRIX World()const;
	// Get View matrices.
	DirectX::XMMATRIX View()const;
	// Get Projection matrices.
	DirectX::XMMATRIX Proj()const;
	

	// Strafe/Walk the camera a distance d.
	void Strafe(float d);
	void Walk(float d);
	void FlyVertical(float d);

	// Rotate the camera.
	void Pitch(float angle);
	void RotateY(float angle);

	// After modifying camera position/orientation, call to rebuild the view matrix.
	void UpdateViewMatrix();

private:

	// camera coordinate system with coordinates relative to world space.
	DirectX::XMFLOAT3 mPosition;
	DirectX::XMFLOAT3 mRight;
	DirectX::XMFLOAT3 mUp;
	DirectX::XMFLOAT3 mLook;

	// Cache frustum properties.
	float mNearZ;
	float mFarZ;
	float mAspect;
	float mFovY;
	float mNearWindowHeight;
	float mFarWindowHeight;

	// Cache View/Proj matrices.
	DirectX::XMFLOAT4X4 mView;
	DirectX::XMFLOAT4X4 mProj;
};

#endif //CAMERA_H

