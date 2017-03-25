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

	int i[] = {
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

	meshData.indiceindex.assign(i,i+36);

	ComputeNorm(meshData);

	std::vector<XMFLOAT3> vpos;
	for(size_t j=0;j<meshData.vertices.size();j++)
		vpos.push_back(v[j].Position);
	
	//Octree octree;
	//octree.Build(vpos, meshData.indiceindex);
};

void myShapeLibrary::LoadFromTinyObj(const char* filename, const char* basepath /*= NULL*/, bool triangulate /*= true*/, MeshData &meshData)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename,
		basepath, triangulate);

	if (!ret) {
		MessageBox(0, L"RegisterClass Failed.", L"error", MB_OK);
		return;
	}
	if (!attrib.normals.size()||!attrib.texcoords.size() || !attrib.vertices.size()) {
		MessageBox(0, L"obj file wrong.", L"error", MB_OK);
		return;
	}

	std::vector<int> v_index_temp;
	std::vector<int> n_index_temp;
	std::vector<int> t_index_temp;

	// For each shape
	for (size_t i = 0; i < shapes.size(); i++)
	{
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
			size_t fnum = shapes[i].mesh.num_face_vertices[f];

			// For each indice in the face
			for (size_t v = 0; v < fnum; v++) {
				tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];
				v_index_temp.push_back(idx.vertex_index);
				n_index_temp.push_back(idx.normal_index);
				t_index_temp.push_back(idx.texcoord_index);
			}
			index_offset += fnum;
		}
	}

	//remap triangle
	//index: 0-last 1,2,3,4,...
	//vertex: v[v_index_temp] normal: n[n_index_temp] texture: t[t_index_temp]
	for (size_t i = 0; i < v_index_temp.size() ; i++)
	{
		int v_id = v_index_temp[i];
		int n_id = n_index_temp[i];
		int t_id = t_index_temp[i];
		meshData.vertices.push_back(Vertex(attrib.vertices[3 * v_id], attrib.vertices[3 * v_id + 1], attrib.vertices[3 * v_id + 2],
									attrib.normals[3 * n_id], attrib.normals[3 * n_id + 1], attrib.normals[3 * n_id + 2],
									attrib.texcoords[2 * t_id], attrib.texcoords[2 * t_id + 1]
		));
		meshData.indiceindex.push_back(i);
	}
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

//todo method is wrong
void myShapeLibrary::ComputeNorm(MeshData &meshData)
{
	int trianglenum = meshData.indiceindex.size() / 3;

	for (int i = 0; i < trianglenum; i++)
	{
		int i0 = meshData.indiceindex[3 * i];
		int i1 = meshData.indiceindex[3 * i + 1];
		int i2 = meshData.indiceindex[3 * i + 2];
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

