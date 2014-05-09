/*
*(C)2011 Symplatonix Software
*/

#ifndef _OBJ_FILE_H_
#define _OBJ_FILE_H_

#include <map>
#include <stdio.h>

#include <assimp.hpp>
#include <aiScene.h>
#include <aiPostProcess.h>

#include "mofmesh.h"

typedef std::map<std::string, bool> BoneUsageMap;

class Converter
{
	public:
		Converter();
		~Converter();

		int read(const char *filename);
		int convertToMof(const char *filename);

	private:
		
		MOFMesh *m_mofMesh;
		char *m_baseFileAddr;

		int m_curMaterial; //current material used for the face

		vector<Vector3D*> m_normals;
		vector<Vector3D*> m_positions;
		vector<Vector3D*> m_textureUV;

		std::string extractFilename(std::string filePath);
		int readVertex(const aiScene *scene, int meshIndex, MOFSubmesh *submesh);
		int readFace(const aiScene *scene, int meshIndex, MOFSubmesh *submesh);
		int readBones(const aiScene *scene, int meshIndex, MOFSubmesh *submesh);
		int buildSkeleton(const aiMesh *curAiMesh, MOFSubmesh *submesh, aiNode *curNode, BoneUsageMap *boneMap, int parentId);
		int readMaterials(const aiScene *scene);
};

#endif