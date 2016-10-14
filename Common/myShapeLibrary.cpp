#include "myShapeLibrary.h"

using namespace DirectX;

void myShapeLibrary::CreateBox(float width, float height, float depth,MeshData &meshData)
{
	Vertex v[8];
	
	v[0] = Vertex(-1.0f, -1.0f, -1.0f);
	v[1] = Vertex(-1.0f, +1.0f, -1.0f);
	v[2] = Vertex(+1.0f, +1.0f, -1.0f);
	v[3] = Vertex(+1.0f, -1.0f, -1.0f);

	v[4] = Vertex(-1.0f, -1.0f, +1.0f);
	v[5] = Vertex(-1.0f, +1.0f, +1.0f);
	v[6] = Vertex(1.0f, 1.0f, 1.0f);
	v[7] = Vertex(+1.0f, -1.0f, +1.0f);

	meshData.vertices.assign(&v[0], &v[8]);

	unsigned int i[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
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
		XMFLOAT3 b = myMathLibrary::sub(meshData.vertices[i2].Position, meshData.vertices[i1].Position);
		XMFLOAT3 facenormal = myMathLibrary::cross(a, b);

		meshData.vertices[i0].Normal = myMathLibrary::add(facenormal, meshData.vertices[i0].Normal);
		meshData.vertices[i1].Normal = myMathLibrary::add(facenormal, meshData.vertices[i1].Normal);
		meshData.vertices[i2].Normal = myMathLibrary::add(facenormal, meshData.vertices[i2].Normal);
	}

	for (int i = 0; i < meshData.vertices.size(); i++)
		meshData.vertices[i].Normal = myMathLibrary::normlize(meshData.vertices[i].Normal);

}

