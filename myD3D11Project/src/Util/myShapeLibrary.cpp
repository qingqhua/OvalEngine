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
	meshData.vertices.assign(&m_vertices[0], &m_vertices[0]+ m_vertices.size());
	//meshData.indices.assign(&m_indices[0], &m_indices[m_indexcount]);

	//Octree octree;
	//octree.Build(vpos, meshData.indices);
}

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
	
	//push vertexs & normals
	for (size_t v = 0; v < attrib.vertices.size() / 3; v++) {
		meshData.vertices.push_back(Vertex(attrib.vertices[3 * v + 0], attrib.vertices[3 * v + 1], attrib.vertices[3 * v + 2]
			//,attrib.normals[3 * v + 0], attrib.normals[3 * v + 1], attrib.normals[3 * v + 2]
			));
	}
		

	// For each shape
	for (size_t i = 0; i < shapes.size(); i++)
	{
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
			size_t fnum = shapes[i].mesh.num_face_vertices[f];

			// For each indice in the face
			for (size_t v = 0; v < fnum; v++) {
				tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];
				meshData.indiceindex.push_back(idx.vertex_index);
			}
			index_offset += fnum;
		}
	}
}

void myShapeLibrary::PrintInfo(const tinyobj::attrib_t& attrib,
	const std::vector<tinyobj::shape_t>& shapes,
	const std::vector<tinyobj::material_t>& materials) {
	std::cout << "# of vertices  : " << (attrib.vertices.size() / 3) << std::endl;
	std::cout << "# of normals   : " << (attrib.normals.size() / 3) << std::endl;
	std::cout << "# of texcoords : " << (attrib.texcoords.size() / 2)
		<< std::endl;

	std::cout << "# of shapes    : " << shapes.size() << std::endl;
	std::cout << "# of materials : " << materials.size() << std::endl;

	for (size_t v = 0; v < attrib.vertices.size() / 3; v++) {
		printf("  v[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
			static_cast<const double>(attrib.vertices[3 * v + 0]),
			static_cast<const double>(attrib.vertices[3 * v + 1]),
			static_cast<const double>(attrib.vertices[3 * v + 2]));
	}

	for (size_t v = 0; v < attrib.normals.size() / 3; v++) {
		printf("  n[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
			static_cast<const double>(attrib.normals[3 * v + 0]),
			static_cast<const double>(attrib.normals[3 * v + 1]),
			static_cast<const double>(attrib.normals[3 * v + 2]));
	}

	for (size_t v = 0; v < attrib.texcoords.size() / 2; v++) {
		printf("  uv[%ld] = (%f, %f)\n", static_cast<long>(v),
			static_cast<const double>(attrib.texcoords[2 * v + 0]),
			static_cast<const double>(attrib.texcoords[2 * v + 1]));
	}

	// For each shape
	for (size_t i = 0; i < shapes.size(); i++) {
		printf("shape[%ld].name = %s\n", static_cast<long>(i),
			shapes[i].name.c_str());
		printf("Size of shape[%ld].indices: %lu\n", static_cast<long>(i),
			static_cast<unsigned long>(shapes[i].mesh.indices.size()));

		size_t index_offset = 0;

		assert(shapes[i].mesh.num_face_vertices.size() ==
			shapes[i].mesh.material_ids.size());

		printf("shape[%ld].num_faces: %lu\n", static_cast<long>(i),
			static_cast<unsigned long>(shapes[i].mesh.num_face_vertices.size()));

		// For each face
		for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
			size_t fnum = shapes[i].mesh.num_face_vertices[f];

			printf("  face[%ld].fnum = %ld\n", static_cast<long>(f),
				static_cast<unsigned long>(fnum));

			// For each vertex in the face
			for (size_t v = 0; v < fnum; v++) {
				tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];
				printf("    face[%ld].v[%ld].idx = %d/%d/%d\n", static_cast<long>(f),
					static_cast<long>(v), idx.vertex_index, idx.normal_index,
					idx.texcoord_index);
			}

			printf("  face[%ld].material_id = %d\n", static_cast<long>(f),
				shapes[i].mesh.material_ids[f]);

			index_offset += fnum;
		}

		printf("shape[%ld].num_tags: %lu\n", static_cast<long>(i),
			static_cast<unsigned long>(shapes[i].mesh.tags.size()));
		for (size_t t = 0; t < shapes[i].mesh.tags.size(); t++) {
			printf("  tag[%ld] = %s ", static_cast<long>(t),
				shapes[i].mesh.tags[t].name.c_str());
			printf(" ints: [");
			for (size_t j = 0; j < shapes[i].mesh.tags[t].intValues.size(); ++j) {
				printf("%ld", static_cast<long>(shapes[i].mesh.tags[t].intValues[j]));
				if (j < (shapes[i].mesh.tags[t].intValues.size() - 1)) {
					printf(", ");
				}
			}
			printf("]");

			printf(" floats: [");
			for (size_t j = 0; j < shapes[i].mesh.tags[t].floatValues.size(); ++j) {
				printf("%f", static_cast<const double>(
					shapes[i].mesh.tags[t].floatValues[j]));
				if (j < (shapes[i].mesh.tags[t].floatValues.size() - 1)) {
					printf(", ");
				}
			}
			printf("]");

			printf(" strings: [");
			for (size_t j = 0; j < shapes[i].mesh.tags[t].stringValues.size(); ++j) {
				printf("%s", shapes[i].mesh.tags[t].stringValues[j].c_str());
				if (j < (shapes[i].mesh.tags[t].stringValues.size() - 1)) {
					printf(", ");
				}
			}
			printf("]");
			printf("\n");
		}
	}

	for (size_t i = 0; i < materials.size(); i++) {
		printf("material[%ld].name = %s\n", static_cast<long>(i),
			materials[i].name.c_str());
		printf("  material.Ka = (%f, %f ,%f)\n",
			static_cast<const double>(materials[i].ambient[0]),
			static_cast<const double>(materials[i].ambient[1]),
			static_cast<const double>(materials[i].ambient[2]));
		printf("  material.Kd = (%f, %f ,%f)\n",
			static_cast<const double>(materials[i].diffuse[0]),
			static_cast<const double>(materials[i].diffuse[1]),
			static_cast<const double>(materials[i].diffuse[2]));
		printf("  material.Ks = (%f, %f ,%f)\n",
			static_cast<const double>(materials[i].specular[0]),
			static_cast<const double>(materials[i].specular[1]),
			static_cast<const double>(materials[i].specular[2]));
		printf("  material.Tr = (%f, %f ,%f)\n",
			static_cast<const double>(materials[i].transmittance[0]),
			static_cast<const double>(materials[i].transmittance[1]),
			static_cast<const double>(materials[i].transmittance[2]));
		printf("  material.Ke = (%f, %f ,%f)\n",
			static_cast<const double>(materials[i].emission[0]),
			static_cast<const double>(materials[i].emission[1]),
			static_cast<const double>(materials[i].emission[2]));
		printf("  material.Ns = %f\n",
			static_cast<const double>(materials[i].shininess));
		printf("  material.Ni = %f\n", static_cast<const double>(materials[i].ior));
		printf("  material.dissolve = %f\n",
			static_cast<const double>(materials[i].dissolve));
		printf("  material.illum = %d\n", materials[i].illum);
		printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
		printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
		printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
		printf("  material.map_Ns = %s\n",
			materials[i].specular_highlight_texname.c_str());
		printf("  material.map_bump = %s\n", materials[i].bump_texname.c_str());
		printf("    bump_multiplier = %f\n", static_cast<const double>(materials[i].bump_texopt.bump_multiplier));
		printf("  material.map_d = %s\n", materials[i].alpha_texname.c_str());
		printf("  material.disp = %s\n", materials[i].displacement_texname.c_str());
		printf("  <<PBR>>\n");
		printf("  material.Pr     = %f\n", static_cast<const double>(materials[i].roughness));
		printf("  material.Pm     = %f\n", static_cast<const double>(materials[i].metallic));
		printf("  material.Ps     = %f\n", static_cast<const double>(materials[i].sheen));
		printf("  material.Pc     = %f\n", static_cast<const double>(materials[i].clearcoat_thickness));
		printf("  material.Pcr    = %f\n", static_cast<const double>(materials[i].clearcoat_thickness));
		printf("  material.aniso  = %f\n", static_cast<const double>(materials[i].anisotropy));
		printf("  material.anisor = %f\n", static_cast<const double>(materials[i].anisotropy_rotation));
		printf("  material.map_Ke = %s\n", materials[i].emissive_texname.c_str());
		printf("  material.map_Pr = %s\n", materials[i].roughness_texname.c_str());
		printf("  material.map_Pm = %s\n", materials[i].metallic_texname.c_str());
		printf("  material.map_Ps = %s\n", materials[i].sheen_texname.c_str());
		printf("  material.norm   = %s\n", materials[i].normal_texname.c_str());
		std::map<std::string, std::string>::const_iterator it(
			materials[i].unknown_parameter.begin());
		std::map<std::string, std::string>::const_iterator itEnd(
			materials[i].unknown_parameter.end());

		for (; it != itEnd; it++) {
			printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
		}
		printf("\n");
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
