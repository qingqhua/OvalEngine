#include "myShapeLibrary.h"

using namespace DirectX;

void myShapeLibrary::CreateBox(float width, float height, float depth,MeshData &meshData)
{
	Vertex v[24];
	
	v[0] = Vertex(XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f));
	v[1] = Vertex(XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f));
	v[2] = Vertex(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f));
	v[3] = Vertex(XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f));

	v[4] = Vertex(XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f));
	v[5] = Vertex(XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f));
	v[6] = Vertex(XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f));
	v[7] = Vertex(XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f));

	v[8] = Vertex(XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f));
	v[9] = Vertex(XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f));
	v[10] = Vertex(XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f));
	v[11] = Vertex(XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f));

	v[12] = Vertex(XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f));
	v[13] = Vertex(XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f));
	v[14] = Vertex(XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f));
	v[15] = Vertex(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f));

	v[16] = Vertex(XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f));
	v[17] = Vertex(XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f));
	v[18] = Vertex(XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f));
	v[19] = Vertex(XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f));

	v[20] = Vertex(XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f));
	v[21] = Vertex(XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f));
	v[22] = Vertex(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f));
	v[23] = Vertex(XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f));

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

	ComputeNorm(meshData);
};

void myShapeLibrary::CreateSphere(float radius)
{

}

void myShapeLibrary::CreateCylinder(float radius)
{

}

void myShapeLibrary::CreateQuad(MeshData &meshdata) {
	Vertex v[4];
	v[0] = Vertex(-1.0f, -1.0f, 0.0f);
	v[1] = Vertex(-1.0f, 1.0f, 0.0f);
	v[2] = Vertex(1.0f, 1.0f, 0.0f);
	v[3] = Vertex(1.0f, -1.0f, 0.0f);

	meshdata.vertices.assign(&v[0], &v[4]);

	int i[] = {
		0,1,2,
		0,2,3
	};

	meshdata.indices.assign(&i[0], &i[6]);

	ComputeNorm(meshdata);
}


void myShapeLibrary::ComputeNorm(MeshData &meshData)
{
	int trianglenum = meshData.indices.size() / 3;

	for (int i = 0; i < trianglenum; i++)
	{
		int i0 = meshData.indices[3 * i];
		int i1 = meshData.indices[3 * i + 1];
		int i2 = meshData.indices[3 * i + 2];
		XMFLOAT3 a = myMathLibrary::sub(meshData.vertices[i1].Position, meshData.vertices[i0].Position);
		XMFLOAT3 b = myMathLibrary::sub(meshData.vertices[i2].Position, meshData.vertices[i0].Position);
		XMFLOAT3 facenormal = myMathLibrary::cross(a, b);

		meshData.vertices[i0].Normal = myMathLibrary::add(facenormal, meshData.vertices[i0].Normal);
		meshData.vertices[i1].Normal = myMathLibrary::add(facenormal, meshData.vertices[i1].Normal);
		meshData.vertices[i2].Normal = myMathLibrary::add(facenormal, meshData.vertices[i2].Normal);
	}

	for (int i = 0; i < meshData.vertices.size(); i++)
		meshData.vertices[i].Normal = myMathLibrary::normlize(meshData.vertices[i].Normal);
}


