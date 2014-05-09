/*
*
*/

#pragma once
#ifndef _CLEARSKY_Terrain_H_
#define _CLEARSKY_Terrain_H_

//#include "core/precompiled.h"
#include "core/utils.h"
#include "core/timer.h"
#include "rendersystem/mesh.h"
#include "rendersystem/ibuffer.h"

namespace clearsky
{
	class TerrainQuadtreeNode
	{
		public:
			TerrainQuadtreeNode();
			~TerrainQuadtreeNode();

			TerrainQuadtreeNode *children[4];
			bool isLeave;
			Vertex *vertexData;
			IBuffer *vertexBuffer;

			int posX;
			int posY;
			int width;
			int height;

			Vector3D bsphereCenter; //bounding sphere
			int bsphereRadius; //bounding sphere

			Vector3D bboxMin; //bounding box minimum coord
			Vector3D bboxMax; //bounding box maximum coord
	};

	//quadtree for the terrain
	class TerrainQuadtree
	{
		public:
			void setIndexBuffer(IBuffer *newIndexBuffer);
			void setChunkSize(const int width, const int height);
			void setPosition(Vector3D position);
			void setEffect(IEffect *effect);
			RETURN_VALUE build(Vertex *data, int x, int y, int width,int height);
			RETURN_VALUE show();
			void setViewFrustum(ViewFrustum *viewFrustum);

			TerrainQuadtree();
			~TerrainQuadtree();
		
		private:
			RETURN_VALUE build(TerrainQuadtreeNode *node, Vertex *data, int x, int y, int width, int height);
			RETURN_VALUE show(TerrainQuadtreeNode *node);
			//compute bounding box for leaves
			RETURN_VALUE computeBoundingSphere(Vertex *data, int size, Vector3D &center, int &radius, Vector3D &bboxMin, Vector3D &bboxMax);
			RETURN_VALUE computeBoundingBox(Vertex *data, int size, Vector3D &bboxMin, Vector3D &bboxMax);

			int m_maxChunkWidth; //width of the leave data
			int m_maxChunkHeight; //height of the leave data

			TerrainQuadtreeNode *m_root; //top node of the quadtree

			IRenderer *m_renderer;
			IBuffer *m_indexBuffer;
			ViewFrustum *m_viewFrustum;

			IEffect *m_effect;

			int m_terrainWidth;
			int m_terrainHeight;

			Vector3D m_position;
	};

	class CLEARSKY_API Terrain
	{
		public:
			Terrain();
			~Terrain();

			void setScaleFactors(float x, float y, float z);
			void setScaleFactors(Vector3D newFactors);
			void setLOD(int lod);
			int getLOD();

			void setChunkSize(const unsigned int width, const unsigned int height);

			void setEffect(IEffect *effect);

			//support raw image format
			RETURN_VALUE loadFromFile(const char *filename, int width, int depth);
			RETURN_VALUE saveToFile(const char *filename);
			RETURN_VALUE generateFlat(int width, int depth, int height);

			unsigned char* getHeightmap();

			RETURN_VALUE create();
			void setColorMap(ITexture2D *texture);
			void setDetailMap(ITexture2D *texture);

			void draw(ViewFrustum *viewFrustum);

			//get height at specific map position
			RETURN_VALUE getHeight(double x, double z, double &height);

			Vector3D position;
		private:
			IBuffer *m_indexBuffer;
			//height values
			//std::vector<unsigned char> m_heightmap;
			unsigned char *m_heightmap;

			TerrainQuadtree *m_quadtree;  
			unsigned int m_chunkWidth; //width of quadtree smallest region
			unsigned int m_chunkHeight; //height of quadtree smallest region
			//std::vector<Mesh*> m_meshes;
			Vertex *m_vertices;

			//load det height data
			RETURN_VALUE loadRawHeightmap(const char *filename);
			//get the normal of a vertex at specific highmap position
			Vector3D getNormal(int x, int z);

			Vector3D m_scaleFactors; //scale factor for x,y,z direction
			int m_width; //map width
			int m_depth; //map height
			int m_lod; //level of detail

			IEffect *m_effect;

			ITexture2D *m_texture1;
			ITexture2D *m_texture2;

			IRenderer *m_renderer;
	};
};

#endif
