/*
*(C)2011 Symplatonix Software
*/

#include "mofmesh.h"

MOFMesh::MOFMesh():m_windingOrder(MOF_WINDING_ORDER_CLOCKWISE),
					m_index32(true)
{

}

MOFMesh::~MOFMesh()
{

}

int MOFMesh::write(const char *filename)
{
	printf("\n\nOpen file for output\n");

	//nothing to write, if there are no meshes
	if(this->m_submeshes.size()<=0)
	{
	    printf("no meshes to write\n");
		return -1;
	}

	FILE *outputFile;
	fopen_s(&outputFile,filename, "wb");
	if(!outputFile)
	{
		printf("can not open file for writing: %s\n",filename);
		return -1;
	}

	
	printf("write header\n");
	this->writeHeader(outputFile);

	for(unsigned int i=0; i<this->m_submeshes.size(); ++i)
	{
		printf("\nwrite mesh[%d]\n",i);

		MOFSubmesh *curMesh = this->m_submeshes[i];
		printf("mesh name: %s\n",curMesh->getName());

		MOF_MESH_CHUNK_HEADER meshChunk;
		meshChunk.id = MOF_MESH_CHUNK_ID;
		meshChunk.size = 0; //TODO: proper size
		fwrite(&meshChunk, sizeof(MOF_MESH_CHUNK_HEADER),1,outputFile);

		//write submesh name
		char *submeshName = curMesh->getName();
		char meshNameSize = strlen(curMesh->getName());
		if(!submeshName)
			meshNameSize = 0;
		fwrite(&meshNameSize,1,1,outputFile);

		if(submeshName)
			fwrite(submeshName,1,meshNameSize,outputFile);

		printf("write vertices: %d\n",curMesh->getVertices().size());
		this->writeVertexChunk(outputFile,this->m_submeshes[i]);
		printf("write indices: %d\n", curMesh->getFaces().size()*3);
		this->writeIndexChunk(outputFile,this->m_submeshes[i]);
		printf("write material Chunk: %d\n", this->m_materials.size());
		this->writeMaterialChunk(outputFile, curMesh);
		//printf("write bone Chunk\n");
		//this->writeBoneChunk(outputFile, curMesh);
	}

	fclose(outputFile);


	printf("MOF File was written to: %s\n",filename);

	return 0;
}

int MOFMesh::writeHeader(FILE *outputFile)
{
	MOF_HEADER header;
	header.id			= MOF_FILE_ID;
	header.version		= MOF_CURRENT_VERSION;
	header.endianess	= MOF_LITTLE_ENDIAN;
	header.numMeshes	= this->m_submeshes.size();
	//TODO: add error checking
	fwrite(&header, sizeof(MOF_HEADER),1,outputFile);

	return 0;
}

int MOFMesh::writeVertexChunk(FILE *outputFile, MOFSubmesh *submesh)
{
	//general chunk header
	MOF_MESH_CHUNK_HEADER chunkHeader;
	chunkHeader.id = MOF_VERTEX_CHUNK_ID;
	chunkHeader.size = 0; //TODO: set proper size
	fwrite(&chunkHeader,sizeof(MOF_MESH_CHUNK_HEADER),1,outputFile);

	vector<Vertex*> vertices = submesh->getVertices();
	//vertex specific header
	MOF_VERTEX_CHUNK_HEADER vertexChunkHeader;
	vertexChunkHeader.numVerts = vertices.size();
	vertexChunkHeader.numElementVertex = 4;
	vertexChunkHeader.topology         = MOF_VERTEX_TOPOLOGY_TRIANGLELIST;
	vertexChunkHeader.windingOrder     = this->m_windingOrder;

	fwrite(&vertexChunkHeader,sizeof(MOF_VERTEX_CHUNK_HEADER),1,outputFile);

	//write vertex data
	for(unsigned int i=0; i<vertices.size(); ++i)
	{
		//vertex position
		char typeId = MOF_VERTEX_POSITION;
		fwrite(&typeId,1,1,outputFile);
		float position[3] = {vertices[i]->pos.x,
							 vertices[i]->pos.y,
							 vertices[i]->pos.z};
		fwrite(position,3*4,1,outputFile);

		//vertex texture uv coords
		typeId = MOF_VERTEX_TEX;
		fwrite(&typeId,1,1,outputFile);
		fwrite(&vertices[i]->texUV.x,4,1,outputFile);
		fwrite(&vertices[i]->texUV.y,4,1,outputFile);

		//vertex normals
		typeId = MOF_VERTEX_NORMAL;
		fwrite(&typeId,1,1,outputFile);
		float normals[3] = {vertices[i]->normal.x,
							 vertices[i]->normal.y,
							 vertices[i]->normal.z};
		fwrite(normals,3*4,1,outputFile);

		//vertex tangent
		typeId = MOF_VERTEX_TANGENT;
		fwrite(&typeId,1,1,outputFile);
		float tangent[3] = {vertices[i]->tangent.x,
							 vertices[i]->tangent.y,
							 vertices[i]->tangent.z};
		fwrite(tangent,3*4,1,outputFile);


		/*printf(" normal[%d]: %f, %f, %f\n", i, vertices[i]->normal.x,
											   vertices[i]->normal.y,
											   vertices[i]->normal.z);*/

		
	}

	return 0;
}

//need for face sorting
bool faceCompare(const Face *face1, const Face *face2)
{
	return face1->matId < face2->matId;
};

int MOFMesh::writeIndexChunk(FILE *outputFile, MOFSubmesh *submesh)
{
	//general chunk header
	MOF_MESH_CHUNK_HEADER chunkHeader;
	chunkHeader.id = MOF_INDEX_CHUNK_ID;
	chunkHeader.size = 0; //TODO: set proper size

	fwrite(&chunkHeader,sizeof(MOF_MESH_CHUNK_HEADER),1,outputFile);

	vector<Face*> faces = submesh->getFaces();
	//vertex specific header
	MOF_INDEX_CHUNK_HEADER indexChunkHeader;
	indexChunkHeader.numIndices = faces.size()*3;
	indexChunkHeader.indexFormat = MOF_INDEX_FORMAT_32;

	fwrite(&indexChunkHeader,sizeof(MOF_INDEX_CHUNK_HEADER),1,outputFile);

	//sort faces by material id
	std::sort(faces.begin(), faces.end(), faceCompare);

	//write vertex positions
	for(unsigned int i=0; i<faces.size(); ++i)
	{
		fwrite(&faces[i]->index1,4,1,outputFile);
		fwrite(&faces[i]->index2,4,1,outputFile);
		fwrite(&faces[i]->index3,4,1,outputFile);

		//printf(" Write FACE: %d, %d, %d\n", faces[i]->index1, faces[i]->index2, faces[i]->index3);
	}

	return 0;
}

int MOFMesh::writeMaterialChunk(FILE *outputFile, MOFSubmesh *submesh)
{
	if(this->m_materials.size()<=0)
		return -1;

	vector<Face*> submeshFaces = submesh->getFaces();
	
	if(submeshFaces.empty() || submeshFaces[0]->matId<0)
		return -1;
	
	//general chunk header
	MOF_MESH_CHUNK_HEADER chunkHeader;
	chunkHeader.id = MOF_MATERIAL_CHUNK_ID;
	chunkHeader.size = 0; //TODO: set proper size

	fwrite(&chunkHeader,sizeof(MOF_MESH_CHUNK_HEADER),1,outputFile);

	std::vector<Material*> materials = submesh->getMaterials();

	int numMaterials = materials.size();
	fwrite(&numMaterials,4,1,outputFile);

	//sort faces by material id
	std::sort(submeshFaces.begin(), submeshFaces.end(), faceCompare);

	for(unsigned int mat=0; mat<materials.size()/*this->m_materials.size()*/; ++mat)
	{
		Material *curMaterial = materials[mat];//this->m_materials[mat];

		//write materialId
		int matId = mat;//curMaterial->id;
		fwrite(&matId,1,1,outputFile);

		//write start face index
		int startFace = curMaterial->startFace;
		fwrite(&startFace,4,1,outputFile);

		//write number of faces
		int numFaces = curMaterial->numFaces;
		fwrite(&numFaces,4,1,outputFile);

		printf("matId: %d start face: %d numFaces: %d\n",matId, startFace, numFaces);

		/*float diffuseColor[4] = {curMaterial->diffuse.r,
								curMaterial->diffuse.g,
								curMaterial->diffuse.b,
								curMaterial->diffuse.a};*/

		int textureSize = curMaterial->textureName.size();
		if(textureSize>0)
		{
			char textureId = MOF_MATERIAL_TEXTURE;
			fwrite(&textureId,1,1,outputFile);

			fwrite(&textureSize,1,1,outputFile);

			fwrite(curMaterial->textureName.c_str(),1,textureSize,outputFile);
			
			printf("write texture: %s\n",curMaterial->textureName.c_str());
		}

		char diffuseId = MOF_MATERIAL_DIFFUSE;
		fwrite(&diffuseId,1,1,outputFile);
		//fwrite(diffuseColor,4,4,outputFile);
		fwrite(&curMaterial->diffuse.r,4,1,outputFile);
		fwrite(&curMaterial->diffuse.g,4,1,outputFile);
		fwrite(&curMaterial->diffuse.b,4,1,outputFile);
		fwrite(&curMaterial->diffuse.a,4,1,outputFile);

		/*float specularColor[4] = {curMaterial->specular.r,
								  curMaterial->specular.g,
								  curMaterial->specular.b,
								  curMaterial->specular.a};*/
		char specularId = MOF_MATERIAL_SPECULAR;
		fwrite(&specularId,1,1,outputFile);
		//fwrite(specularColor,4,4,outputFile);
		fwrite(&curMaterial->specular.r,4,1,outputFile);
		fwrite(&curMaterial->specular.g,4,1,outputFile);
		fwrite(&curMaterial->specular.b,4,1,outputFile);
		fwrite(&curMaterial->specular.a,4,1,outputFile);

		/*float ambientColor[4] = {curMaterial->ambient.r,
								  curMaterial->ambient.g,
								  curMaterial->ambient.b,
								  curMaterial->ambient.a};*/
		char ambientId = MOF_MATERIAL_AMBIENT;
		fwrite(&ambientId,1,1,outputFile);
		//fwrite(ambientColor,4,4,outputFile);
		fwrite(&curMaterial->ambient.r,4,1,outputFile);
		fwrite(&curMaterial->ambient.g,4,1,outputFile);
		fwrite(&curMaterial->ambient.b,4,1,outputFile);
		fwrite(&curMaterial->ambient.a,4,1,outputFile);

		textureSize = curMaterial->specTexName.size();
		if(textureSize>0)
		{
			char textureId = MOF_MATERIAL_SPECULAR_TEXTURE;
			fwrite(&textureId,1,1,outputFile);

			fwrite(&textureSize,1,1,outputFile);

			fwrite(curMaterial->specTexName.c_str(),1,textureSize,outputFile);
			
			printf("write specular texture: %s\n",curMaterial->specTexName.c_str());
		}

		textureSize = curMaterial->heightTexName.size();
		if(textureSize>0)
		{
			char textureId = MOF_MATERIAL_HEIGHT_TEXTURE;
			fwrite(&textureId,1,1,outputFile);

			fwrite(&textureSize,1,1,outputFile);

			fwrite(curMaterial->heightTexName.c_str(),1,textureSize,outputFile);
			
			printf("write height texture: %s\n",curMaterial->heightTexName.c_str());
		}

		textureSize = curMaterial->normalTexName.size();
		if(textureSize>0)
		{
			char textureId = MOF_MATERIAL_NORMALS_TEXTURE;
			fwrite(&textureId,1,1,outputFile);

			fwrite(&textureSize,1,1,outputFile);

			fwrite(curMaterial->normalTexName.c_str(),1,textureSize,outputFile);
			
			printf("write normals texture: %s\n",curMaterial->normalTexName.c_str());
		}

		char endOfEnumeration = MOF_MATERIAL_END_ENUMERATION;
		fwrite(&endOfEnumeration,1,1,outputFile);
	}

	return 0;
}

int MOFMesh::writeBoneChunk(FILE *outputFile, MOFSubmesh *submesh)
{
	std::vector<Bone*> bones = submesh->getBones();

	if(bones.size()<=0)
		return -1;

	MOF_MESH_CHUNK_HEADER chunkHeader;
	chunkHeader.id = MOF_BONE_CHUNK_ID;
	chunkHeader.size = 0;

	fwrite(&chunkHeader,sizeof(MOF_MESH_CHUNK_HEADER),1,outputFile);

	
	int numBones = bones.size();
	//write number of bones
	fwrite(&numBones,4,1,outputFile);

	//now write of the bone data
	for(unsigned int i=0; i<bones.size(); ++i)
	{
		Bone *curBone = bones[i];

		//bone name
		char boneNameSize = curBone->name.size();
		fwrite(&boneNameSize,1,1,outputFile);
		fwrite(curBone->name.c_str(),1,boneNameSize,outputFile);

		//parent index
		fwrite(&curBone->parentIndex,4,1,outputFile);

		//transformation matrix
		fwrite(&curBone->matrix,4,16,outputFile);
	}

	return 0;
}

void MOFMesh::setWindingOrder(char windingOrder)
{
	this->m_windingOrder = windingOrder;
}

void MOFMesh::setIndex32(bool index32)
{
	this->m_index32 = index32;
}

bool MOFMesh::isIndex32()
{
	return this->m_index32;
}

int MOFMesh::addSubmesh(MOFSubmesh *newSubmesh)
{
	this->m_submeshes.push_back(newSubmesh);
	return 0;
}

vector<Material*> MOFMesh::getMaterials()
{
	return this->m_materials;
}

void MOFMesh::addMaterial(Material *newMaterial)
{
	this->m_materials.push_back(newMaterial);
}


