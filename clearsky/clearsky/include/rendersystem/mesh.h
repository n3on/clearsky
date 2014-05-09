/*
*
*/
#pragma once
#ifndef _CLEARSKY_OBJECT_H_
#define _CLEARSKY_OBJECT_H_


//#include "core/precompiled.h"

#include "core/utils.h"
#include "core/globals.h"

#include "rendersystem/submesh.h"

#include "rendersystem/irenderer.h"
#include "rendersystem/ibuffer.h"
#include "rendersystem/texturemanager.h"

#include "math/mathutils.h"
#include "math/viewfrustum.h"

namespace clearsky
{
	#define MOF_FILE_ID				0x67
	#define MOF_CURRENT_VERSION		0x01
	#define MOF_MESH_CHUNK			0x10
	#define MOF_VERTEX_CHUNK_ID		0x20
		#define MOF_VERTEX_POSITION 0x01
		#define MOF_VERTEX_NORMAL	0x02
		#define MOF_VERTEX_TEX		0x03
		#define MOF_VERTEX_COLOR	0x04
		#define MOF_VERTEX_TANGENT	0x05
	#define MOF_INDEX_CHUNK_ID		0x30
	#define MOF_MATERIAL_CHUNK_ID	0x40
		#define MOF_MATERIAL_TEXTURE  0x01
		#define MOF_MATERIAL_DIFFUSE  0x02
		#define MOF_MATERIAL_SPECULAR 0x03
		#define MOF_MATERIAL_AMBIENT  0x04
		#define MOF_MATERIAL_SPECULAR_TEXTURE  0x05
		#define MOF_MATERIAL_HEIGHT_TEXTURE    0x06
		#define MOF_MATERIAL_NORMALS_TEXTURE   0x07
		#define MOF_MATERIAL_END_ENUMERATION 0x4F
	#define MOF_BONE_CHUNK_ID		0x50

	struct CLEARSKY_API MOF_HEADER
	{
		char id;
		char version;
		char endianess;
		int numMeshes;
	};

	struct CLEARSKY_API MOF_MESH_CHUNK_HEADER
	{
		char id;
		int size;
	};

	struct CLEARSKY_API MOF_CHUNK_HEADER
	{
		char id;
		int  size;
		int  num;
	};

	struct CLEARSKY_API MOF_MATERIAL_CHUNK
	{
		int startFace;
		int endFace;
	};

	class CLEARSKY_API MOFMesh
	{
		public:
			MOFMesh();
			~MOFMesh();

			RETURN_VALUE load(const char* filename);
			RETURN_VALUE save(const char* filename);

			void draw();
			void draw(int meshNum);

			void rotateY(float deg);
			void rotateX(float deg);
			void rotateZ(float deg);
			void translate(float x, float y, float z);
			void scale(float factorX, float factorY, float factorZ);
			void transform(Matrix *transformMat);

			void setRotation(float degX, float degY, float degZ);
			void setPosition(float x, float y, float z);
			void setSize(float factorX, float factorY, float factorZ); 

			void resetTransform();

			MOFSubmesh *getSubmesh(int meshNum);

			AABB getBoudingBox();

			//ray must be in world space
			//TODO: make collision in object space for efficianty
			bool rayCollision(const Vector3D rayOrigin, const Vector3D rayDirection);

			void setName(std::string name);
			std::string getName();

			std::string getFilename();

			void setId(unsigned long long id);
			unsigned long long getId();

			void release();

		private:
			MOFSubmesh **m_meshes;
			int m_numMeshes;
			char *m_baseFileAddr;

			unsigned long long m_id;
			std::string m_name;
			std::string m_filename;

			Matrix m_world;

			FILE *m_file;

			RETURN_VALUE readMesh(MOF_HEADER *header, MOF_MESH_CHUNK_HEADER *meshHeader, int meshNum);

			int loadVertexChunk(MOF_CHUNK_HEADER *header,int meshNum);
			int loadIndexChunk(MOF_CHUNK_HEADER *header,int meshNum);
			int loadMaterialChunk(MOF_CHUNK_HEADER *header,int meshNum);

			RETURN_VALUE writeMesh(FILE *file, int meshNum);

			RETURN_VALUE writeVertexChunk(FILE *file, int meshNum);
			RETURN_VALUE writeIndexChunk(FILE *file, int meshNum);
			RETURN_VALUE writeMaterialChunk(FILE *file, int meshNum);
	};
}

#endif

