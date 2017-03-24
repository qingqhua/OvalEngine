#pragma once

#include <vector>
#include "DirectXCollision.h"

class OctreeNode
{
public:
	OctreeNode();
	~OctreeNode();
		
	OctreeNode *children[8];

	//every leaf holds the attributes
	std::vector<unsigned int> indices;
	bool IsLeaf;

	void subDivide(DirectX::BoundingBox box[8]);
	DirectX::BoundingBox bound;

private:

};


