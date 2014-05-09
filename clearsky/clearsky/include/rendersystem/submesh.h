/*
*
*/
#pragma once
#ifndef _CLEARSKY_MESH_H_
#define _CLEARSKY_MESH_H_

//#include "core/precompiled.h"

#include "core/globals.h"
#include "core/utils.h"

//#include "rendersystem/ResourceManager.h"

//#include "rendersystem/resourcemanager.h"
#include "rendersystem/irenderer.h"
#include "rendersystem/ibuffer.h"
#include "rendersystem/itexture2D.h"
#include "rendersystem/skeletonanim.h"

#include "math/mathutils.h"
#include "math/boundingbox.h"

namespace clearsky
{
	class CLEARSKY_API MOFSubmesh
	{
		public:
			MOFSubmesh();
			~MOFSubmesh();


			void draw();

			void rotateY(float deg);
			void rotateX(float deg);
			void rotateZ(float deg);
			void translate(float x, float y, float z);
			void scale(float factorX, float factorY, float factorZ);
			void transform(Matrix *transformMat);
			void resetTransform();

			Vector3D getPosition();

			void setRotation(float degX, float degY, float degZ);
			void setPosition(float x, float y, float z);
			void setSize(float factorX, float factorY, float factorZ); 

			void addTexture2D(ITexture2D *texture);
			std::vector<ITexture2D*> getTextures();

			void addSpecTexture2D(ITexture2D *texture);
			std::vector<ITexture2D*> getSpecTextures();

			void addHeightTexture2D(ITexture2D *texture);
			std::vector<ITexture2D*> getHeightTextures();

			void setTextureBase(std::string textureBasePath);

			void setVertexBuffer(IBuffer *vertexBuffer);
			IBuffer* getVertexBuffer();

			void setIndexBuffer(IBuffer *indexBuffer);
			IBuffer* getIndexBuffer();

			void setVisible(bool visible);
			bool isVisible();

			int getNumVertices();
			void setNumVertices(int num);

			int getNumElementsVertex();
			void setNumElementsVertex(int num);

			int getNumIndices();
			void setNumIndices(int num);

			int getNumMaterials();
			void setNumMaterials(int num);

			void addMaterial(Material *material);

			void addVertexData(Vertex *vertexData, int numVerts);
			void addIndexData(DWORD *indexData, int numIndices);

			RETURN_VALUE buildVBuffer();
			RETURN_VALUE buildIBuffer();

			PRIMITIVE_TOPOLOGY setPrimitiveTopology(PRIMITIVE_TOPOLOGY newTopology);
			PRIMITIVE_TOPOLOGY getPrimitiveTopology();

			void setFrontOrder(bool ccw);
			bool frontOrderCCW();

			void setBoundingBox(AABB &aabb);
			AABB getBoudingBox();

			bool rayCollision(const Vector3D rayOrigin, const Vector3D rayDirection);

			//apply transformation to the bounding box
			AABB updateBoundingBox();

			void setName(const char *name);
			char* getName();

			void addBone(Bone *newBone);
			std::vector<Bone*> getBones();

		private:
			char m_name[150];

			Matrix m_world;
			IBuffer *m_vertexBuffer;
			IBuffer *m_indexBuffer;

			SamplerState m_samplerState;

			bool m_frontCounterClockwise; 
			int m_numVertices;
			int m_numElementsVertex;
			Vertex* m_vertexData;

			int m_numIndices;
			DWORD* m_indexData;

			PRIMITIVE_TOPOLOGY m_primitiveTopology;

			int m_numMaterials;

			bool m_visible;

			std::vector<Material*> m_materials;
			std::vector<ITexture2D*> m_textures;
			std::vector<ITexture2D*> m_specularTextures;
			std::vector<ITexture2D*> m_heightTextures;
			std::string m_textureBasePath;
			std::vector<Bone*> m_bones;

			AABB m_aabb;
	};
}

#endif