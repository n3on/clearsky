/*
*(C)2011 Symplatonix Software
*/

#include <iostream>
#include "converter.h"

Converter::Converter():m_curMaterial(-1)
{
	this->m_mofMesh = new MOFMesh();
	this->m_baseFileAddr = NULL;
}

Converter::~Converter()
{
	if(this->m_mofMesh)
	{
		delete this->m_mofMesh;
		this->m_mofMesh = NULL;
	}

	for(unsigned int i=0; i<this->m_normals.size(); ++i)
	{
		if(this->m_normals[i])
		{
			delete this->m_normals[i];
			this->m_normals[i] = NULL;
		}
	}
}

int Converter::read(const char *filename)
{
	//import the file
	Assimp::Importer importer;  
	importer.SetExtraVerbose(true);
	const aiScene* scene = importer.ReadFile(filename,aiProcess_Triangulate | aiProcess_RemoveRedundantMaterials | 
														aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices | 
														 /*aiProcess_ConvertToLeftHanded*/aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | 
														aiProcess_FindInvalidData | aiProcess_GenUVCoords | 
														aiProcess_CalcTangentSpace);
	if(!scene)
	{
		printf("Error: %s\n",importer.GetErrorString());
		return 1;
	}

	//get materials
	this->readMaterials(scene);
	printf("Loaded materials\n");

	//get the meshes
	aiMesh **meshes = scene->mMeshes;
	for(unsigned int i=0; i<scene->mNumMeshes; ++i)
	{
		printf("\nLoad mesh %d\n", i);
		printf("Mesh name: %s\n", meshes[i]->mName.data);

		MOFSubmesh *submesh = new MOFSubmesh();
		std::string meshName(meshes[i]->mName.data);
		if(meshName.empty())
		{
			std::ostringstream nameStr;
			nameStr<<"M"<<i;
			meshName.assign(nameStr.str());
		}

		submesh->setName(meshName.c_str());
        
		this->m_mofMesh->addSubmesh(submesh);

		//read vertices
		readVertex(scene,i,submesh);
		printf("Loades mesh vertices\n", i);

		readFace(scene,i,submesh);
		printf("Loades mesh faces\n", i);

		readBones(scene,i,submesh);

		submesh->addMaterial(this->m_mofMesh->getMaterials()[meshes[i]->mMaterialIndex]);
		printf("Material: index: %d size: %d\n", meshes[i]->mMaterialIndex, this->m_mofMesh->getMaterials().size());
		//set the number of faces for the material attached to the mesh
		if(meshes[i]->mMaterialIndex<this->m_mofMesh->getMaterials().size())
			this->m_mofMesh->getMaterials()[meshes[i]->mMaterialIndex]->numFaces = meshes[i]->mNumFaces;

		printf("Mesh: %d NumVerts: %d NumNormals: %d NumTexCoord: %d\n",i, meshes[i]->mNumVertices,
																			this->m_normals.size(),
																			this->m_textureUV.size());
	}//end get meshes

	return 0;
}

int Converter::readVertex(const aiScene *scene, int meshIndex, MOFSubmesh *submesh)
{
	//read vertices
	aiMesh **meshes = scene->mMeshes;
	for(unsigned int iVerts=0; iVerts<meshes[meshIndex]->mNumVertices; ++iVerts)
	{
		Vertex *newVertex = new Vertex();
		aiVector3D position = meshes[meshIndex]->mVertices[iVerts];
		newVertex->pos.x = position.x;
		newVertex->pos.y = position.y;
		newVertex->pos.z = position.z;

		if(submesh->findVertex(newVertex)>=0)
		{
			delete newVertex;
			continue;
		}

		if(meshes[meshIndex]->mNormals!=NULL)
		{
			aiVector3D normal = meshes[meshIndex]->mNormals[iVerts];
			newVertex->normal.x = normal.x;
			newVertex->normal.y = normal.y;
			newVertex->normal.z = normal.z;
		}

		if(meshes[meshIndex]->mTextureCoords[0])
		{
			aiVector3D textureCoord = meshes[meshIndex]->mTextureCoords[0][iVerts];
			newVertex->texUV.x = textureCoord.x;
			newVertex->texUV.y = textureCoord.y;
			newVertex->texUV.z = textureCoord.z;
		}

		if(meshes[meshIndex]->mTangents!=NULL)
		{
			aiVector3D tangent = meshes[meshIndex]->mTangents[iVerts];
			newVertex->tangent.x = tangent.x;
			newVertex->tangent.y = tangent.y;
			newVertex->tangent.z = tangent.z;
			
		}

		submesh->addVertex(newVertex);
	}

	return 0;
}

int Converter::readFace(const aiScene *scene, int meshIndex, MOFSubmesh *submesh)
{
	//read faces
	aiMesh **meshes = scene->mMeshes;
	for(unsigned int iFaces=0; iFaces<meshes[meshIndex]->mNumFaces; ++iFaces)
	{
		Face *newFace = new Face();
		aiFace face = meshes[meshIndex]->mFaces[iFaces];

		newFace->index1 = face.mIndices[0];
		newFace->index2 = face.mIndices[1];
		newFace->index3 = face.mIndices[2];
		newFace->matId	= 0;//meshes[i]->mMaterialIndex;

		//printf("Face: %d %d %d\n",newFace->index1,newFace->index2,newFace->index3);
	
		submesh->addFace(newFace);
	}

	return 0;
}

int Converter::readBones(const aiScene *scene, int meshIndex, MOFSubmesh *submesh)
{
	aiMesh *curMesh = scene->mMeshes[meshIndex];
	if(!curMesh)
		return -1;

	aiBone **bones = curMesh->mBones;
	printf("NumBones: %d\n",curMesh->mNumBones);
	
	std::map<std::string, bool> bonesForSkeleton;

	for(unsigned int i=0; i<scene->mMeshes[meshIndex]->mNumBones; ++i)
	{
		bonesForSkeleton.insert(std::pair<std::string, bool>(std::string(bones[i]->mName.data), false));
		//printf("BoneName: %s\n",bones[i]->mName);
	}
	
	std::map<std::string, bool>::iterator iter;
	for(unsigned int i=0; i<scene->mMeshes[meshIndex]->mNumBones; ++i)
	{
		aiBone *curBone = bones[i];
		printf("BoneName: %s\n",curBone->mName.data);
		std::string boneName = std::string(curBone->mName.data);
		aiNode *boneNode = scene->mRootNode->FindNode(curBone->mName);
		if(boneNode==NULL)
			continue;
		
		iter = bonesForSkeleton.find(boneName);
		if(iter!=bonesForSkeleton.end())
		{
			//printf("found\n");
			iter->second = true;
		}
		
		//iterate over all parents
		//to mark active bones
		aiNode *boneParent = boneNode->mParent;
		bool hasParent = true;
		while(hasParent)
		{
			if(boneParent == NULL)
				hasParent = false;
			iter = bonesForSkeleton.find(std::string(boneParent->mName.data));
			if(iter!=bonesForSkeleton.end())
			{
				//printf(" found parent %s\n",boneParent->mName.data);
				iter->second = true;
			}
			else
				hasParent = false;

			boneParent = boneParent->mParent;
		}
	
		//aiNode *boneNode = scene->mRootNode->FindNode(bonesForSkeleton.find(curBone->mName
	    
		//bonesForSkeleton.insert(std::pair<std::string, bool>(std::string(bones[i]->mName.data), false));
		//printf("BoneName: %s\n",bones[i]->mName);
	}

	buildSkeleton(curMesh,submesh,scene->mRootNode,&bonesForSkeleton,-1);


	return 0;
}

int Converter::buildSkeleton(const aiMesh *curAiMesh, MOFSubmesh *submesh, aiNode *curNode, BoneUsageMap *boneMap, int parentId)
{
	if((curNode==NULL) || (curAiMesh==NULL) || (!submesh))
		return -1;

	BoneUsageMap::iterator iter;
	iter = boneMap->find(std::string(curNode->mName.data));
	int index = -1;
	
	if(iter!=boneMap->end() && (iter->second==true))
	{
		//find bone index
		for(int i=0; i<curAiMesh->mNumBones; ++i)
		{
			if(curNode->mName==curAiMesh->mBones[i]->mName)
			{
				index = i;
			}
		}

		if(index>=0)
		{
			aiBone *bone = curAiMesh->mBones[index];
			Bone *newBone = new Bone;
			newBone->name = std::string(bone->mName.data);
			newBone->parentIndex = parentId;
			newBone->matrix[0] = *bone->mOffsetMatrix[0]; 
			newBone->matrix[1] = *bone->mOffsetMatrix[1]; 
			newBone->matrix[2] = *bone->mOffsetMatrix[2];
			newBone->matrix[3] = *bone->mOffsetMatrix[3];

			newBone->matrix[4] = *bone->mOffsetMatrix[4];
			newBone->matrix[5] = *bone->mOffsetMatrix[5];
			newBone->matrix[6] = *bone->mOffsetMatrix[6];
			newBone->matrix[7] = *bone->mOffsetMatrix[7];

			newBone->matrix[8] = *bone->mOffsetMatrix[8];
			newBone->matrix[9] = *bone->mOffsetMatrix[9];
			newBone->matrix[10] = *bone->mOffsetMatrix[10];
			newBone->matrix[11] = *bone->mOffsetMatrix[11];

			newBone->matrix[12] = *bone->mOffsetMatrix[12];
			newBone->matrix[13] = *bone->mOffsetMatrix[13];
			newBone->matrix[14] = *bone->mOffsetMatrix[14];
			newBone->matrix[15] = *bone->mOffsetMatrix[15];

			submesh->addBone(newBone);
			
			//printf("make skeleton: name: %s parent: %d\n",curAiMesh->mBones[index]->mName.data,parentId);
		}

		
	}
	
	for(int i=0; i<curNode->mNumChildren; ++i)
	{
		curNode = curNode->mChildren[i];
		buildSkeleton(curAiMesh,submesh,curNode,boneMap, index);
	}

	return 0;
}

int Converter::readMaterials(const aiScene *scene)
{
	//get materials
	aiMaterial **materials = scene->mMaterials;
	for(unsigned int iMaterial=0; iMaterial<scene->mNumMaterials; ++iMaterial)
	{
		aiMaterial *curMaterial = materials[iMaterial];
		Material *newMaterial = new Material;

		float alpha = 1.0f;
		curMaterial->Get(AI_MATKEY_OPACITY,alpha);

		aiColor3D color(0.f,0.f,0.f);
		curMaterial->Get(AI_MATKEY_COLOR_DIFFUSE,color);
		newMaterial->diffuse = Color(color.r, color.g, color.b,alpha);

		curMaterial->Get(AI_MATKEY_COLOR_AMBIENT,color);
		newMaterial->ambient = Color(color.r, color.g, color.b,alpha);

		curMaterial->Get(AI_MATKEY_COLOR_SPECULAR,color);
		newMaterial->specular = Color(color.r, color.g, color.b,alpha);

		newMaterial->id = iMaterial;

		//diffuse map
		aiString path;
		if(aiGetMaterialTexture(curMaterial,aiTextureType_DIFFUSE,0,&path,0,0,0,0,0,0)==AI_SUCCESS)
		{
			std::string filePath(path.data);
			newMaterial->textureName = this->extractFilename(filePath);
			
			printf(" diffuse texture: %s\n",newMaterial->textureName.c_str());
			//strncpy(newMaterial->textureName,/*path.data*/filename.c_str(),140);
		}
		
		//specular map
		path.Clear();
		if(aiGetMaterialTexture(curMaterial,aiTextureType_SPECULAR,0,&path,0,0,0,0,0,0)==AI_SUCCESS)
		{
			std::string filePath(path.data);
			newMaterial->specTexName = this->extractFilename(filePath);

			printf(" specular texture: %s\n",newMaterial->specTexName.c_str());
			//strncpy(newMaterial->specTexName,filename.c_str(),140);
		}

		//height map
		path.Clear();
		if(aiGetMaterialTexture(curMaterial,aiTextureType_HEIGHT,0,&path,0,0,0,0,0,0)==AI_SUCCESS)
		{
			std::string filePath(path.data);
			newMaterial->heightTexName = this->extractFilename(filePath);

			printf(" height texture: %s\n", newMaterial->heightTexName.c_str());
			//strncpy(newMaterial->specTexName,filename.c_str(),140);
		}

		//normal map
		path.Clear();
		if(aiGetMaterialTexture(curMaterial,aiTextureType_NORMALS,0,&path,0,0,0,0,0,0)==AI_SUCCESS)
		{
			std::string filePath(path.data);
			newMaterial->normalTexName = this->extractFilename(filePath);

			printf(" normal texture: %s\n", newMaterial->normalTexName.c_str());
			//strncpy(newMaterial->specTexName,filename.c_str(),140);
		}

		this->m_mofMesh->addMaterial(newMaterial);
	}

	return 0;
}

std::string Converter::extractFilename(std::string filePath)
{
	size_t pos = filePath.find_last_of('\\');
	if(pos==string::npos)
			pos = filePath.find_last_of('/');

	std::string filename = filePath.substr(pos+1);

	return filename;
}

int Converter::convertToMof(const char *filename)
{
	this->m_mofMesh->setWindingOrder(MOF_WINDONG_ORDER_CCLOCKWISE);

	return this->m_mofMesh->write(filename);
}
