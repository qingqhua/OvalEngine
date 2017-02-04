#include "myShapeLibrary.h"

using namespace DirectX;

void myShapeLibrary::CreateBox(XMFLOAT3 center,float extent,MeshData &meshData)
{
	Vertex v[8];
	
	v[0] = Vertex(XMFLOAT3(center.x - extent, center.y + extent, center.z - extent), XMFLOAT2(0.0f, 0.0f));
	v[1] = Vertex(XMFLOAT3(center.x + extent, center.y + extent, center.z - extent), XMFLOAT2(0.0f, 0.0f));
	v[2] = Vertex(XMFLOAT3(center.x + extent, center.y + extent, center.z + extent), XMFLOAT2(0.0f, 0.0f));
	v[3] = Vertex(XMFLOAT3(center.x - extent, center.y + extent, center.z + extent), XMFLOAT2(0.0f, 0.0f));

	v[4] = Vertex(XMFLOAT3(center.x - extent, center.y - extent, center.z - extent), XMFLOAT2(0.0f, 0.0f));
	v[5] = Vertex(XMFLOAT3(center.x + extent, center.y - extent, center.z - extent), XMFLOAT2(0.0f, 0.0f));
	v[6] = Vertex(XMFLOAT3(center.x + extent, center.y - extent, center.z + extent), XMFLOAT2(0.0f, 0.0f));
	v[7] = Vertex(XMFLOAT3(center.x - extent, center.y - extent, center.z + extent), XMFLOAT2(0.0f, 0.0f));

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

	Vertex *m_vertices = new Vertex[m_vertexcount];
	unsigned int *m_indices = new unsigned int[m_indexcount];
	std::vector<XMFLOAT3> vpos;

	//TODO:if num_mesh and num_indices aren`t the same at every loop, it will cause incorrect result
	//it wont work if there are two meshes
	for (int cur_mesh = 0; cur_mesh < scene->mNumMeshes; cur_mesh++)
	{
		//get the position of each vertex
		int num_vertices = scene->mMeshes[cur_mesh]->mNumVertices;
		for (int v = 0; v < num_vertices; v++)
		{
			//m_vertices.push_back(XMFLOAT3(scene->mMeshes[cur_mesh]->mVertices[v].x, scene->mMeshes[cur_mesh]->mVertices[v].y, scene->mMeshes[cur_mesh]->mVertices[v].z));
			m_vertices[cur_mesh *num_vertices +v].Position = XMFLOAT3(scene->mMeshes[cur_mesh]->mVertices[v].x, scene->mMeshes[cur_mesh]->mVertices[v].y, scene->mMeshes[cur_mesh]->mVertices[v].z);
			vpos.push_back(m_vertices[cur_mesh *num_vertices + v].Position);
		}

		//get each index
		int num_face = scene->mMeshes[cur_mesh]->mNumFaces;
		for (int cur_face = 0; cur_face < num_face; cur_face++)
		{
			int num_indices = scene->mMeshes[cur_mesh]->mFaces[cur_face].mNumIndices;

			for (int i=0;i<num_indices;i++)
			{
				//m_indices.push_back(scene->mMeshes[cur_mesh]->mFaces[cur_face].mIndices[i]);
				m_indices[cur_face*num_indices +i] = scene->mMeshes[cur_mesh]->mFaces[cur_face].mIndices[i];
			}
		}
	}
		meshData.vertices.assign(&m_vertices[0], &m_vertices[m_vertexcount]);
		meshData.indices.assign(&m_indices[0], &m_indices[m_indexcount]);

		Octree octree;
		octree.Build(vpos, meshData.indices);

		
		//meshData.vertices.assign(&octree.m_vertices[0], &octree.m_vertices[octree.m_vertices.size()]);
		//meshData.indices.assign(&octree.indices[0], &octree.indices[octree.indices.size()]);

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


