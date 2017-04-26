//-----------------------------------
//FILE : model.cpp
//-----------------------------------

#include "model.h"

using namespace DirectX;

model::model()
	: m_vertexBuffer(0),
	  m_indexBuffer(0),
	  m_texture(0)
{ 
}

model::~model()
{
}

bool model::Init(ID3D11Device* device, const char* filename, const char* basepath)
{

	// Initialize the vertex and index buffers.
	if (!InitBuffer(device,filename,basepath))
	{
		return false;
	}
	return true;
}

void model::Shutdown()
{
	ShutdownBuffer();
}

void model::Render(ID3D11DeviceContext* context)
{
	RenderBuffer(context);
}

bool model::InitBuffer(ID3D11Device* device, const char* filename, const char* basepath)
{
 		//load model from TinyObj
 		tinyobj::attrib_t attrib;
 		std::vector<tinyobj::shape_t> shapes;
 		std::vector<tinyobj::material_t> materials;
 			
 		std::string err;
 		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename,
 			basepath, true);
 	
 		if (!ret) {
 			MessageBox(0, L"Load Model Failed.", L"error", MB_OK);
 			return false;
 		}
 	
 		unsigned long* indices;
 	
 		std::vector<int> v_index_temp;
 		std::vector<int> n_index_temp;
 		std::vector<int> t_index_temp;
 	
 		// For each shape, get index of every attribute
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
 	
 		m_vertices = new Vertex[v_index_temp.size()];
 		m_vertexCount = v_index_temp.size();
 	
 		if (!m_vertices)
 		{
 			return false;
 		}
 	
 		indices = new unsigned long[v_index_temp.size()];
 		m_indexCount = v_index_temp.size();
 	
 		if (!indices)
 		{
 			return false;
 		}
 	
 		//remap triangle
 		//index: 0-last 1,2,3,4,...
 		//vertex: v[v_index_temp] normal: n[n_index_temp] texture: t[t_index_temp]
 		for (size_t i = 0; i < v_index_temp.size(); i++)
 		{
 			int v_id = v_index_temp[i];
 			int n_id = n_index_temp[i];
 			int t_id = t_index_temp[i];
 	
 			m_vertices[i].position = XMFLOAT3(attrib.vertices[3 * v_id], attrib.vertices[3 * v_id + 1], attrib.vertices[3 * v_id + 2]);
 			m_vertices[i].normal=	XMFLOAT3(attrib.normals[3 * n_id], attrib.normals[3 * n_id + 1], attrib.normals[3 * n_id + 2]);
 			m_vertices[i].texture=XMFLOAT2(attrib.texcoords[2 * t_id], attrib.texcoords[2 * t_id + 1]);
 	
 			indices[i] = i;
 		}

	// build geometry buffer
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// create vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.

	delete[] indices;
	indices = 0;

	return true;
}

void model::ShutdownBuffer()
{
	if (m_vertices)
	{
		delete[] m_vertices;
		m_vertices = 0;
	}

	// Release the texture resource.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void model::RenderBuffer(ID3D11DeviceContext* context)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(Vertex);
	offset = 0;

	context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool model::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* context, char* filename)
{
	return true;
}


int model::GetIndexCount()
{
	return m_indexCount;
}

DirectX::BoundingBox model::GetAABB()
{
	//infinity
	XMVECTOR min = XMVectorReplicate(+FLT_MAX);
	XMVECTOR max = XMVectorReplicate(-FLT_MAX);

	for (size_t i = 0; i < m_vertexCount; ++i)
	{
		XMVECTOR pos = XMLoadFloat3(&m_vertices[i].position);
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

ID3D11ShaderResourceView* model::GetTexture()
{
	return m_texture;
}
