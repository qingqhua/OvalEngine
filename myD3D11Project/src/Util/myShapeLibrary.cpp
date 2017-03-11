#include "myShapeLibrary.h"

using namespace DirectX;

void myShapeLibrary::CreateBox(XMFLOAT3 center,float extent,MeshData &meshData)
{
	Vertex v[8];
	
	v[0] = Vertex(XMFLOAT3(center.x - extent, center.y + extent, center.z - extent), XMFLOAT2(0.0f, 1.0f));
	v[1] = Vertex(XMFLOAT3(center.x + extent, center.y + extent, center.z - extent), XMFLOAT2(1.0f, 0.0f));
	v[2] = Vertex(XMFLOAT3(center.x + extent, center.y + extent, center.z + extent), XMFLOAT2(-1.0f, 0.0f));
	v[3] = Vertex(XMFLOAT3(center.x - extent, center.y + extent, center.z + extent), XMFLOAT2(0.0f, -1.0f));

	v[4] = Vertex(XMFLOAT3(center.x - extent, center.y - extent, center.z - extent), XMFLOAT2(1.0f, 0.0f));
	v[5] = Vertex(XMFLOAT3(center.x + extent, center.y - extent, center.z - extent), XMFLOAT2(0.0f, 1.0f));
	v[6] = Vertex(XMFLOAT3(center.x + extent, center.y - extent, center.z + extent), XMFLOAT2(0.0f, 0.0f));
	v[7] = Vertex(XMFLOAT3(center.x - extent, center.y - extent, center.z + extent), XMFLOAT2(0.0f, -1.0f));

	meshData.vertices.assign(&v[0], &v[8]);

	unsigned int i[] = {
		//bottom
		4,5,7,
		7,5,6,

		//up
		0,2,1,
		0,3,2,

		//left
		3,0,7,
		7,0,4,
		
		//right
		1,2,5,
		5,2,6,

		//back
		2,3,6,
		6,3,7,

		//forward
		0,1,4,
		4,1,5
	};

	meshData.indices.assign(i,i+36);

	ComputeNorm(meshData);

	std::vector<XMFLOAT3> vpos;
	for(size_t j=0;j<meshData.vertices.size();j++)
		vpos.push_back(v[j].Position);
	
	Octree octree;
	octree.Build(vpos, meshData.indices);
};

void myShapeLibrary::LoadModel(const char *file,MeshData &meshData)
{

	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(file, aiProcess_ConvertToLeftHanded | aiProcess_Triangulate);

	int m_indexcount = 0;
	int m_vertexcount = 0;
	//get indexcount and vertexcount
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		m_vertexcount += scene->mMeshes[i]->mNumVertices;

		for (int j = 0; j < scene->mMeshes[i]->mNumFaces;j++)
		{
			m_indexcount += scene->mMeshes[i]->mFaces[j].mNumIndices;
		}
	}

	std::vector<XMFLOAT3> vpos;
	std::vector<unsigned int> m_indices;
	std::vector<Vertex> m_vertices;

	for (int cur_mesh = 0; cur_mesh < scene->mNumMeshes; cur_mesh++)
	{
		//get the position of each vertex
		int num_vertices = scene->mMeshes[cur_mesh]->mNumVertices;
		for (int v = 0; v < num_vertices; v++)
		{
			//if mesh has normal and normal!=0
			//if normal =0 will cause assert
			if (scene->mMeshes[cur_mesh]->HasNormals())
			{
				m_vertices.push_back(Vertex(
					//position
					scene->mMeshes[cur_mesh]->mVertices[v].x, scene->mMeshes[cur_mesh]->mVertices[v].y, scene->mMeshes[cur_mesh]->mVertices[v].z,
					//normal
					scene->mMeshes[cur_mesh]->mNormals[v].x, scene->mMeshes[cur_mesh]->mNormals[v].y, scene->mMeshes[cur_mesh]->mNormals[v].z
				));
			}
			else 				
				m_vertices.push_back(Vertex(
				//position
				scene->mMeshes[cur_mesh]->mVertices[v].x, scene->mMeshes[cur_mesh]->mVertices[v].y, scene->mMeshes[cur_mesh]->mVertices[v].z
			));
			
				vpos.push_back(m_vertices[m_vertices.size()-1].Position);
		}
		//get each index
		int num_face = scene->mMeshes[cur_mesh]->mNumFaces;
		for (int cur_face = 0; cur_face < num_face; cur_face++)
		{
			int num_indices = scene->mMeshes[cur_mesh]->mFaces[cur_face].mNumIndices;

			for (int i=0;i<num_indices;i++)
			{
				m_indices.push_back(scene->mMeshes[cur_mesh]->mFaces[cur_face].mIndices[i]);
			}
		}
	}

	//push one more data to avoid vector outrange
	m_vertices.push_back(Vertex(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
	m_indices.push_back(0);

	//assign vertices and indices to meshdata
	meshData.vertices.assign(&m_vertices[0], &m_vertices[m_vertexcount]);
	meshData.indices.assign(&m_indices[0], &m_indices[m_indexcount]);

	//Octree octree;
	//octree.Build(vpos, meshData.indices);
}

BoundingBox myShapeLibrary::GetAABB(MeshData meshdata)
{
	
	XMVECTOR min = XMVectorReplicate(+myMathLibrary::Infinity);
	XMVECTOR max = XMVectorReplicate(-myMathLibrary::Infinity);

	for (size_t i = 0; i < meshdata.vertices.size(); ++i)
	{
		XMVECTOR pos = XMLoadFloat3(&meshdata.vertices[i].Position);
		min = XMVectorMin(min, pos);
		max = XMVectorMax(max, pos);
	}

	BoundingBox m_bound;
	XMVECTOR center = 0.5*(min + max);
	XMVECTOR extent = 0.5*(max - min);

	XMStoreFloat3(&m_bound.Center, center);
	XMStoreFloat3(&m_bound.Extents, extent);
	return m_bound;
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

//todo method is wrong
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

void myShapeLibrary::CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData)
{
	meshData.vertices.clear();
	meshData.indices.clear();

	//
	// Build Stacks.
	// 

	float stackHeight = height / stackCount;

	// Amount to increment radius as we move up each stack level from bottom to top.
	float radiusStep = (topRadius - bottomRadius) / stackCount;

	UINT ringCount = stackCount + 1;

	// Compute vertices for each stack ring starting at the bottom and moving up.
	for (UINT i = 0; i < ringCount; ++i)
	{
		float y = -0.5f*height + i*stackHeight;
		float r = bottomRadius + i*radiusStep;

		// vertices of ring
		float dTheta = 2.0f*XM_PI / sliceCount;
		for (UINT j = 0; j <= sliceCount; ++j)
		{
			Vertex vertex;

			float c = cosf(j*dTheta);
			float s = sinf(j*dTheta);

			vertex.Position = XMFLOAT3(r*c, y, r*s);

			// This is unit length.
			XMFLOAT3 TangentU = XMFLOAT3(-s, 0.0f, c);

			float dr = bottomRadius - topRadius;
			XMFLOAT3 bitangent(dr*c, -height, dr*s);

			XMVECTOR T = XMLoadFloat3(&TangentU);
			XMVECTOR B = XMLoadFloat3(&bitangent);
			XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
			XMStoreFloat3(&vertex.Normal, N);

			meshData.vertices.push_back(vertex);
		}
	}

	// Add one because we duplicate the first and last vertex per ring
	// since the texture coordinates are different.
	UINT ringVertexCount = sliceCount + 1;

	// Compute indices for each stack.
	for (UINT i = 0; i < stackCount; ++i)
	{
		for (UINT j = 0; j < sliceCount; ++j)
		{
			meshData.indices.push_back(i*ringVertexCount + j);
			meshData.indices.push_back((i + 1)*ringVertexCount + j);
			meshData.indices.push_back((i + 1)*ringVertexCount + j + 1);

			meshData.indices.push_back(i*ringVertexCount + j);
			meshData.indices.push_back((i + 1)*ringVertexCount + j + 1);
			meshData.indices.push_back(i*ringVertexCount + j + 1);
		}
	}

	BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
	BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
}

void myShapeLibrary::BuildCylinderTopCap(float bottomRadius, float topRadius, float height,
	UINT sliceCount, UINT stackCount, MeshData& meshData)
{
	UINT baseIndex = (UINT)meshData.vertices.size();

	float y = 0.5f*height;
	float dTheta = 2.0f*XM_PI / sliceCount;

	// Duplicate cap ring vertices because the texture coordinates and normals differ.
	for (UINT i = 0; i <= sliceCount; ++i)
	{
		float x = topRadius*cosf(i*dTheta);
		float z = topRadius*sinf(i*dTheta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		meshData.vertices.push_back(Vertex(x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
	}

	// Cap center vertex.
	meshData.vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f));

	// Index of center vertex.
	UINT centerIndex = (UINT)meshData.vertices.size() - 1;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		meshData.indices.push_back(centerIndex);
		meshData.indices.push_back(baseIndex + i + 1);
		meshData.indices.push_back(baseIndex + i);
	}
}

void myShapeLibrary::BuildCylinderBottomCap(float bottomRadius, float topRadius, float height,
	UINT sliceCount, UINT stackCount, MeshData& meshData)
{
	// 
	// Build bottom cap.
	//

	UINT baseIndex = (UINT)meshData.vertices.size();
	float y = -0.5f*height;

	// vertices of ring
	float dTheta = 2.0f*XM_PI / sliceCount;
	for (UINT i = 0; i <= sliceCount; ++i)
	{
		float x = bottomRadius*cosf(i*dTheta);
		float z = bottomRadius*sinf(i*dTheta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		meshData.vertices.push_back(Vertex(x, y, z, 0.0f, -1.0f, 0.0f, 1.0f,0.0f));
	}

	// Cap center vertex.
	meshData.vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f));

	// Cache the index of center vertex.
	UINT centerIndex = (UINT)meshData.vertices.size() - 1;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		meshData.indices.push_back(centerIndex);
		meshData.indices.push_back(baseIndex + i);
		meshData.indices.push_back(baseIndex + i + 1);
	}
}
