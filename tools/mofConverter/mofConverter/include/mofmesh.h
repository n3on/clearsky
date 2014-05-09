/*
*(C)2011 Symplatonix Software
*/

#ifndef _MOF_MESH_H_
#define _MOF_MESH_H_

#include <string>
#include <algorithm>

#include "mofsubmesh.h"

#define MOF_FILE_ID				0x67
#define MOF_CURRENT_VERSION		0x01
#define MOF_LITTLE_ENDIAN		0x00
	#define MOF_MESH_CHUNK_ID	0x10
	#define MOF_VERTEX_CHUNK_ID		0x20
	#define MOF_VERTEX_TOPOLOGY_TRIANGLELIST  0x0
	#define MOF_VERTEX_TOPOLOGY_TRIANGLESTRIP 0x01
	#define MOF_VERTEX_TOPOLOGY_POINTLIST	  0x02
	#define MOF_VERTEX_TOPOLOGY_LINELIST	  0x03
    #define MOF_VERTEX_TOPOLOGY_LINESTRIP	  0x04
	#define MOF_WINDING_ORDER_CLOCKWISE		  0x00
    #define MOF_WINDONG_ORDER_CCLOCKWISE	  0x01
		#define MOF_VERTEX_POSITION 0x01
		#define MOF_VERTEX_NORMAL	0x02
		#define MOF_VERTEX_TEX		0x03
		#define MOF_VERTEX_COLOR	0x04
		#define MOF_VERTEX_TANGENT	0x05
	#define MOF_INDEX_CHUNK_ID		0x30
	#define MOF_INDEX_FORMAT_16     0x00
	#define MOF_INDEX_FORMAT_32     0x01
	#define MOF_MATERIAL_CHUNK_ID	0x40
		#define MOF_MATERIAL_TEXTURE 0x01
		#define MOF_MATERIAL_DIFFUSE 0x02
		#define MOF_MATERIAL_SPECULAR 0x03
		#define MOF_MATERIAL_AMBIENT  0x04
		#define MOF_MATERIAL_SPECULAR_TEXTURE  0x05
		#define MOF_MATERIAL_HEIGHT_TEXTURE    0x06
		#define MOF_MATERIAL_NORMALS_TEXTURE   0x07
		#define MOF_MATERIAL_END_ENUMERATION 0x4F
	#define MOF_BONE_CHUNK_ID		0x50

#pragma pack(push, 1)
struct MOF_HEADER
{
	char id;
	char version;
	char endianess;
	int numMeshes;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct MOF_MESH_CHUNK_HEADER
{
	char id;
	int size;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct MOF_VERTEX_CHUNK_HEADER
{
	int  numVerts; //number of vertices
	char numElementVertex; //number of elements in the vertex
	char topology; //what kind of polygon
	char windingOrder;

};
#pragma pack(pop)

#pragma pack(push, 1)
struct MOF_INDEX_CHUNK_HEADER
{
	int numIndices;
	char indexFormat; //16 or 32 bit
};
#pragma pack(pop)

#pragma pack(push, 1)
struct MOF_MATERIAL_CHUNK
{
	int startFace;
	int endFace;
};
#pragma pack(pop)

//this class represents an object in mof file format
class MOFMesh
{
	public:
		MOFMesh();
		~MOFMesh();

		void setWindingOrder(char windingOrder);
		int write(const char *filename);
		int addSubmesh(MOFSubmesh *newSubmesh);

		void setIndex32(bool index32);
		bool isIndex32();

		vector<Material*> getMaterials();
		void addMaterial(Material *newMaterial);

	private:
		vector<MOFSubmesh*> m_submeshes; //submeshes/object of the object
		vector<Material*> m_materials;

		char m_windingOrder; 
		bool m_index32; //do we have 32 bit indices?

		//write MOF file header
		int writeHeader(FILE *outputFile);
		int writeVertexChunk(FILE *outputFile, MOFSubmesh *submesh);
		int writeIndexChunk(FILE *outputFile, MOFSubmesh *submesh);
		int writeMaterialChunk(FILE *outputFile, MOFSubmesh *submesh);
		int writeBoneChunk(FILE *outputFile, MOFSubmesh *submesh);
};

#endif
