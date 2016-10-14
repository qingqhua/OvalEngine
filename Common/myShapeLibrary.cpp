#include "myShapeLibrary.h"

using namespace DirectX;

void myShapeLibrary::CreateBox(float width, float height, float depth,MeshData &meshData)
{
	Vertex v[24];
	
	v[0] = Vertex(-1.0f, 1.0f, -1.0f);
	v[1] = Vertex(1.0f, 1.0f, -1.0f);
	v[2] = Vertex(1.0f, 1.0f, 1.0f);
	v[3] = Vertex(-1.0f, 1.0f, 1.0f);

	v[4] = Vertex(-1.0f, -1.0f, -1.0f);
	v[5] = Vertex(1.0f, -1.0f, -1.0f);
	v[6] = Vertex(1.0f, -1.0f, 1.0f);
	v[7] = Vertex(-1.0f, -1.0f, 1.0f);

	v[8] = Vertex(-1.0f, -1.0f, 1.0f);
	v[9] = Vertex(-1.0f, -1.0f, -1.0f);
	v[10] = Vertex(-1.0f, 1.0f, -1.0f);
	v[11] = Vertex(-1.0f, 1.0f, 1.0f);

	v[12] = Vertex(1.0f, -1.0f, 1.0f);
	v[13] = Vertex(1.0f, -1.0f, -1.0f);
	v[14] = Vertex(1.0f, 1.0f, -1.0f);
	v[15] = Vertex(1.0f, 1.0f, 1.0f);

	v[16] = Vertex(-1.0f, -1.0f, -1.0f);
	v[17] = Vertex(1.0f, -1.0f, -1.0f);
	v[18] = Vertex(1.0f, 1.0f, -1.0f);
	v[19] = Vertex(-1.0f, 1.0f, -1.0f);

	v[20] = Vertex(-1.0f, -1.0f, 1.0f);
	v[21] = Vertex(1.0f, -1.0f, 1.0f);
	v[22] = Vertex(1.0f, 1.0f, 1.0f);
	v[23] = Vertex(-1.0f, 1.0f, 1.0f);

	meshData.vertices.assign(&v[0], &v[24]);

	unsigned int i[] = {
		3,1,0,
		2,1,3,

		6,4,5,
		7,4,6,

		11,9,8,
		10,9,11,

		14,12,13,
		15,12,14,

		19,17,16,
		18,17,19,

		22,20,21,
		23,20,22
	};

	meshData.indices.assign(i,i+36);

	getNorm(meshData);
};

void myShapeLibrary::getNorm(MeshData &meshData)
{
	int trianglenum = meshData.indices.size() / 3;

	for (int i =0; i < trianglenum; i++)
	{
		int i0 = meshData.indices[3 * i];
		int i1 = meshData.indices[3 * i+1];
		int i2 = meshData.indices[3 * i+2];
		XMFLOAT3 a = myMathLibrary::sub(meshData.vertices[i1].Position, meshData.vertices[i0].Position);
		XMFLOAT3 b = myMathLibrary::sub(meshData.vertices[i2].Position, meshData.vertices[i0].Position);
		XMFLOAT3 facenormal = myMathLibrary::cross(a, b);

		meshData.vertices[i0].Normal = myMathLibrary::add(facenormal, meshData.vertices[i0].Normal);
		meshData.vertices[i1].Normal = myMathLibrary::add(facenormal, meshData.vertices[i1].Normal);
		meshData.vertices[i2].Normal = myMathLibrary::add(facenormal, meshData.vertices[i2].Normal);

// 		XMVECTOR c = XMLoadFloat3(&meshData.vertices[i0].Position);
// 		XMVECTOR d = XMLoadFloat3(&meshData.vertices[i1].Position);
// 		XMVECTOR e = XMLoadFloat3(&meshData.vertices[i2].Position);
// 
// 		XMVECTOR sub1 = XMVectorSubtract(d, c);
// 		XMVECTOR sub2 = XMVectorSubtract(e, c);
// 		XMVECTOR cross = XMVector3Cross(sub1, sub2);
// 		c = XMVectorAdd(c, cross);
// 		d= XMVectorAdd(d, cross);
// 		e= XMVectorAdd(e, cross);
// 		meshData.vertices[i0].Normal = XMFLOAT3(XMVectorGetX(c) , XMVectorGetY(c), XMVectorGetZ(c));
// 		meshData.vertices[i1].Normal = XMFLOAT3(XMVectorGetX(d), XMVectorGetY(d), XMVectorGetZ(d));
// 		meshData.vertices[i2].Normal = XMFLOAT3(XMVectorGetX(e), XMVectorGetY(e), XMVectorGetZ(e));
	}

	for (int i = 0; i < meshData.vertices.size(); i++)
		meshData.vertices[i].Normal = myMathLibrary::normlize(meshData.vertices[i].Normal);

}

