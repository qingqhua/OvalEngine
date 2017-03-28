
#ifndef OCTREE_H
#define OCTREE_H

#include "Util/myMathLibrary.h"
#include "OctreeNode.h"

class Octree
{
public:
	Octree();
	~Octree();
	void Build(const std::vector<DirectX::XMFLOAT3>& vertices, const std::vector<unsigned int>& indices);

private:
	OctreeNode *m_parent;
	DirectX::BoundingBox BuildAABB();
	void BuildOctree(OctreeNode * p, const std::vector<unsigned int>& i);
	std::vector<DirectX::XMFLOAT3> m_vertices;
	std::vector<unsigned int> m_indices;
};



#endif // OCTREE_H