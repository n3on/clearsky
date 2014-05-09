/*
*(C)2011 Symplatonix Software
*/

#include "mofsubmesh.h"

MOFSubmesh::MOFSubmesh()
{
	
}

MOFSubmesh::~MOFSubmesh()
{

}

void MOFSubmesh::addVertex(Vertex *newVertex)
{
	this->m_vertices.push_back(newVertex);
	this->m_vertexTable.insert(std::make_pair<std::string, Vertex*>(newVertex->toString(), newVertex));

}

Vertex* MOFSubmesh::getVertex(unsigned int index)
{
	if(index<this->m_vertices.size())
		return this->m_vertices[index];

	return NULL;
}

vector<Vertex*> MOFSubmesh::getVertices()
{
	return this->m_vertices;
}

int MOFSubmesh::findVertex(Vertex *vertex)
{
	for(int i=(this->m_vertices.size()-1); i>=0; --i)
	{
		if(this->m_vertices[i]->equal(vertex))
			return i;
	}
	/*std::map<std::string, Vertex*>::iterator vertexIter;
	vertexIter = this->m_vertexTable.find(vertex->toString());
	if (vertexIter != this->m_vertexTable.end())
	{
		return std::distance(this->m_vertexTable.begin(), vertexIter);
	}*/

	return -1;
}

void MOFSubmesh::addFace(Face *newFace)
{
	this->m_faces.push_back(newFace);
}

Face* MOFSubmesh::getFace(unsigned int index)
{
	if(index<this->m_faces.size())
		return this->m_faces[index];

	return NULL;
}

vector<Face*> MOFSubmesh::getFaces()
{
	return this->m_faces;
}

vector<Material*> MOFSubmesh::getMaterials()
{
	return this->m_materials;
}

void MOFSubmesh::addMaterial(Material *newMaterial)
{
	this->m_materials.push_back(newMaterial);
}

void MOFSubmesh::setName(const char *name)
{
	strncpy(this->m_name,name,149);
}

char* MOFSubmesh::getName()
{
	return this->m_name;
}

void MOFSubmesh::addBone(Bone *newBone)
{
	this->m_bones.push_back(newBone);
}

vector<Bone*> MOFSubmesh::getBones()
{
	return this->m_bones;
}