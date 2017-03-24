#include "OctreeNode.h"

using namespace DirectX;

OctreeNode::OctreeNode()
{
	for (int i = 0; i < 8; i++)
		children[i] = 0;
	bound.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	bound.Extents = XMFLOAT3(0.0f, 0.0f, 0.0f);
}


OctreeNode::~OctreeNode()
{
	for (int i = 0; i < 8; i++)
		delete(children[i]);
}

void OctreeNode::subDivide(BoundingBox box[8])
{
	XMFLOAT3 halfExtent(0.5f*bound.Extents.x, 0.5f*bound.Extents.y, 0.5f*bound.Extents.z);
	box[0].Center = XMFLOAT3(bound.Center.x + halfExtent.x, bound.Center.y + halfExtent.y, bound.Center.z + halfExtent.z);
	box[0].Extents = halfExtent;
	box[1].Center = XMFLOAT3(bound.Center.x - halfExtent.x, bound.Center.y + halfExtent.y, bound.Center.z + halfExtent.z);
	box[1].Extents = halfExtent;
	box[2].Center = XMFLOAT3(bound.Center.x - halfExtent.x, bound.Center.y + halfExtent.y, bound.Center.z - halfExtent.z);
	box[2].Extents = halfExtent;
	box[3].Center = XMFLOAT3(bound.Center.x + halfExtent.x, bound.Center.y + halfExtent.y, bound.Center.z - halfExtent.z);
	box[3].Extents = halfExtent;
	box[4].Center = XMFLOAT3(bound.Center.x + halfExtent.x, bound.Center.y - halfExtent.y, bound.Center.z + halfExtent.z);
	box[4].Extents = halfExtent;
	box[5].Center = XMFLOAT3(bound.Center.x - halfExtent.x, bound.Center.y - halfExtent.y, bound.Center.z + halfExtent.z);
	box[5].Extents = halfExtent;
	box[6].Center = XMFLOAT3(bound.Center.x - halfExtent.x, bound.Center.y - halfExtent.y, bound.Center.z - halfExtent.z);
	box[6].Extents = halfExtent;
	box[7].Center = XMFLOAT3(bound.Center.x + halfExtent.x, bound.Center.y - halfExtent.y, bound.Center.z - halfExtent.z);
	box[7].Extents = halfExtent;

}
