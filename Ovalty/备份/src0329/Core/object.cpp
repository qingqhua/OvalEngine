#include "object.h"

using namespace DirectX;

Object::Object()
{

}

Object::~Object()
{

}

void Object::LoadModel(const char* filename)
{
	shapes.LoadFromTinyObj(filename, "data/Model/", true, model);
}

DirectX::BoundingBox Object::boundingbox()
{
	return shapes.GetAABB(model);
}

int Object::indiceNum()
{
	return model.indiceindex.size();
}

unsigned int Object::verticeByteWidth()
{
	return sizeof(myShapeLibrary::Vertex)*model.vertices.size();
}

unsigned int Object::indiceByteWidth()
{
	return sizeof(int)*model.indiceindex.size();
}