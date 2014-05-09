/*
*(C)2011 Symplatonix Software
*/

#ifndef _MOF_SUBMESH_H_
#define _MOF_SUBMESH_H_

#include <algorithm>
#include <map>
#include <vector>
#include "utils.h"

using namespace std;

class Bone
{
	public:
		std::string name;
		int parentIndex;
		float matrix[16];
};

class MOFSubmesh
{
	public:
		MOFSubmesh();
		~MOFSubmesh();

		void addVertex(Vertex *newVertex);
		Vertex* getVertex(unsigned int index);
		vector<Vertex*> getVertices();
		int findVertex(Vertex *vertex);

		void addFace(Face *newFace);
		Face* getFace(unsigned int index);
		vector<Face*> getFaces();

		vector<Material*> getMaterials();
		void addMaterial(Material *newMaterial);
		
		void setName(const char *name);
		char* getName();

		void addBone(Bone *newBone);
		vector<Bone*> getBones();

	private:
		char m_name[150];
		std::map<std::string, Vertex*> m_vertexTable;
		vector<Vertex*> m_vertices;
		vector<Face*> m_faces;
		vector<Material*> m_materials;
		vector<Bone*> m_bones;
};

#endif
