#include "Octree.h"

using namespace DirectX;

Octree::Octree():m_parent(0)
{
}

Octree::~Octree()
{
	delete(m_parent);
}

void Octree::Build(const std::vector<XMFLOAT3>& vertices, const std::vector<unsigned int>& indices)
{
	m_vertices = vertices;
	
	//get the bound of the scene
	BoundingBox sceneBound = BuildAABB();

	//reset the parent and children
	m_parent = new OctreeNode();
	m_parent->bound = sceneBound;

	BuildOctree(m_parent,indices);
}

BoundingBox Octree::BuildAABB()
{
	XMVECTOR min = XMVectorReplicate(+myMathLibrary::Infinity);
	XMVECTOR max = XMVectorReplicate(-myMathLibrary::Infinity);

	for (size_t i = 0; i < m_vertices.size(); ++i)
	{
		XMVECTOR pos = XMLoadFloat3(&m_vertices[i]);
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

void Octree::BuildOctree(OctreeNode * p, const std::vector<unsigned int>& indices)
{
	size_t faceCount = indices.size() / 3;
	
	if (faceCount < 6)
	{
		p->IsLeaf = true;
		p->indices = indices;
	}
	else if(faceCount >= 6)
	{
		p->IsLeaf = false;
		BoundingBox subbox[8];
		p->subDivide(subbox);

		for (int i = 0; i < 8; i++)
		{
 			p->children[i] = new OctreeNode();
			p->children[i]->bound = subbox[i];

			//all the indices of the face in children[i]
			std::vector<unsigned int> faceIntersect;

			for (size_t j = 0; j < faceCount; j++)
			{
				int i0 = indices[j * 3 + 0];
				int i1 = indices[j * 3 + 1];
				int i2 = indices[j * 3 + 2];

				XMVECTOR v0 = XMLoadFloat3(&m_vertices[i0]);
				XMVECTOR v1 = XMLoadFloat3(&m_vertices[i1]);
				XMVECTOR v2 = XMLoadFloat3(&m_vertices[i2]);


				//check every face intersect children[i]
				if (subbox[i].Intersects(v0, v1, v2))
				{
					faceIntersect.push_back(i0);
					faceIntersect.push_back(i1);
					faceIntersect.push_back(i2);
				}
			}
			
			BuildOctree(p->children[i], faceIntersect);

		}
	}
}
