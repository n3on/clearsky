/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/terrain.h"

namespace clearsky
{
	TerrainQuadtreeNode::TerrainQuadtreeNode():vertexBuffer(NULL),
												vertexData(NULL),
												posX(0),
												posY(0),
												width(0),
												height(0),
												isLeave(false)
	{
		this->children[0] = NULL;
		this->children[1] = NULL;
		this->children[2] = NULL;
		this->children[3] = NULL; 

		this->bboxMax = Vector3D(-1000000,-1000000,-1000000);
		this->bboxMin = Vector3D(100000,1000000,1000000);

	}

	TerrainQuadtreeNode::~TerrainQuadtreeNode()
	{
		SAFE_DELETE(this->vertexBuffer);
		SAFE_ARRAY_DELETE(this->vertexData);

		SAFE_DELETE(this->children[0]);
		SAFE_DELETE(this->children[1]);
		SAFE_DELETE(this->children[2]);
		SAFE_DELETE(this->children[3]);
	}

	TerrainQuadtree::TerrainQuadtree():m_indexBuffer(NULL),
										m_root(NULL),
										m_viewFrustum(NULL)
	{
		this->m_maxChunkHeight = 64;
		this->m_maxChunkWidth  = 64;

		this->m_renderer = GLOBALS->getRenderer();
	}

	TerrainQuadtree::~TerrainQuadtree()
	{
		SAFE_DELETE(this->m_root);
		SAFE_DELETE(this->m_indexBuffer);
		SAFE_DELETE(this->m_viewFrustum);
	}

	RETURN_VALUE TerrainQuadtree::build(Vertex *data, int x, int y, int width, int height)
	{
		this->m_terrainWidth  = width;
		this->m_terrainHeight = height;
		this->m_root = new TerrainQuadtreeNode();
		this->build(this->m_root, data, x, y, width, height);

		return RETURN_OK;
	}

	RETURN_VALUE TerrainQuadtree::build(TerrainQuadtreeNode *node, Vertex *data, int x, int y, int width, int height)
	{
		if(!node)
			return RETURN_ERROR;

		node->posX		= x;
		node->posY		= y;
		node->width		= width;
		node->height	= height;

		//debug output
		//printf("regionX = %d, regionY = %d, regionWidth = %d, regionHeight = %d terrainWidth = %d terrainHeight = %d\n",x,y,width,height, TerrainQuadtree::terrainWidth, TerrainQuadtree::terrainHeight);

		if((width<=this->m_maxChunkWidth) && (height<=this->m_maxChunkHeight))
		{
			node->isLeave = true;

			++width;
			++height;

			//ensure that our coordinatinates are in memory limits
			if((width+x)>=(this->m_terrainWidth))
				x -= (width+x)-(this->m_terrainWidth)+1;

			if((height+y)>=(this->m_terrainHeight))
				y -= (height+y)-(this->m_terrainHeight)+1;
				
			//printf("regionX = %d, regionY = %d, regionWidth = %d, regionHeight = %d twidth = %d theight = %d index = %d\n",
			//		x,y,width,height, this->m_terrainWidth, this->m_terrainHeight, y*this->m_terrainWidth+x);

			//extract vertices for the region
			node->vertexData = new Vertex[width*height];
			memset(node->vertexData,0,width*height*sizeof(Vertex));
			Vertex *vertexPointer = node->vertexData;
			Vertex *dataPointer = data+(y*(this->m_terrainWidth+1)+x);

			//copy the current vertices block for the chunk
			for(int i=0; i<height; ++i)
			{
				memcpy(vertexPointer,dataPointer,width*sizeof(Vertex));
				dataPointer += (this->m_terrainWidth+1);
				vertexPointer += width;
			}

			this->computeBoundingSphere(node->vertexData,width*height,node->bsphereCenter, node->bsphereRadius, node->bboxMin, node->bboxMax);
			//now build the vertexbuffer width the data
			node->vertexBuffer = GLOBALS->getRenderer()->createBuffer();
			node->vertexBuffer->create(node->vertexData,sizeof(Vertex),width*height);

			SAFE_ARRAY_DELETE(node->vertexData);

			return RETURN_OK;
		}

		//extract the 4 subregions
		//x,y |-------| w,y
		//    | 1 | 2 |
		//    |---|---|
		//    | 3 | 4 |
		//x,h |-------| w,h

		int halfWidth = (int)ceil((float)width/2);
		int halfHeight = (int)ceil((float)height/2);

		node->children[0] = new TerrainQuadtreeNode();
		node->children[1] = new TerrainQuadtreeNode();
		node->children[2] = new TerrainQuadtreeNode();
		node->children[3] = new TerrainQuadtreeNode();

		this->build(node->children[0], data,x, y, halfWidth, halfHeight);
		this->build(node->children[1], data, x+halfWidth, y, halfWidth, halfHeight);
		this->build(node->children[2], data, x, y+halfHeight, halfWidth, halfHeight);
		this->build(node->children[3], data, x+halfWidth, y+halfHeight, halfWidth, halfHeight);
		
		//build bounding box for non leaves
		node->bboxMax = node->children[0]->bboxMax;
		node->bboxMin = node->children[0]->bboxMin;
		for(int i=0; i<4; ++i)
		{
			if(node->children[i]->bboxMax.x>node->bboxMax.x)
				node->bboxMax.x = node->children[i]->bboxMax.x;

			if(node->children[i]->bboxMax.y>node->bboxMax.y)
				node->bboxMax.y = node->children[i]->bboxMax.y;
			
			if(node->children[i]->bboxMax.z>node->bboxMax.z)
				node->bboxMax.z =node->children[i]->bboxMax.z;
			
			if(node->children[i]->bboxMin.x<node->bboxMin.x)
				node->bboxMin.x = node->children[i]->bboxMin.x;

			if(node->children[i]->bboxMax.y<node->bboxMin.y)
				node->bboxMin.y = node->children[i]->bboxMin.y;

			if(node->children[i]->bboxMax.z<node->bboxMin.z)
				node->bboxMin.z =node->children[i]->bboxMin.z;
		}

		//build bounding sphere for non leaves
		node->bsphereCenter = node->children[0]->bsphereCenter+node->children[1]->bsphereCenter+
							  node->children[2]->bsphereCenter+node->children[3]->bsphereCenter;
		node->bsphereCenter /= 4;

		Vector3D line = node->bboxMax - node->bboxMin;
		node->bsphereRadius =  (int)D3DXVec3Length(&line)/2;

		return RETURN_OK;
	}

	RETURN_VALUE TerrainQuadtree::computeBoundingSphere(Vertex *data, int size, Vector3D &center, int &radius, Vector3D &bboxMin, Vector3D &bboxMax)
	{
		if((!data) || (size<=0))
			return RETURN_ERROR;
		
		//compute the center and the radius of the sphere
		center = Vector3D(0,0,0);
		Vector3D minPos = data[0].position; //minimal position of a vertex
		Vector3D maxPos = data[0].position; //maximal position of a vertex 
		for(int i=0; i<size; ++i)
		{
			center += data[i].position;
			
			if(data[i].position.x>maxPos.x)
				maxPos.x = data[i].position.x;

			if(data[i].position.y>maxPos.y)
				maxPos.y = data[i].position.y;
			
			if(data[i].position.z>maxPos.z)
				maxPos.z = data[i].position.z;
			
			if(data[i].position.x<minPos.x)
				minPos.x = data[i].position.x;

			if(data[i].position.y<minPos.y)
				minPos.y = data[i].position.y;

			if(data[i].position.z<minPos.z)
				minPos.z = data[i].position.z;
			//printf("position: %f %f %f\n", center.x, center.y, center.z);
		}
		center /= (float)size;
		
		Vector3D line = maxPos-minPos;
		radius = (int)D3DXVec3Length(&line)/2;

		//bounding box stuff
		bboxMin = minPos;
		bboxMax = maxPos;
		return RETURN_OK;
	}

	RETURN_VALUE computeBoundingBox(Vertex *data, int size, Vector3D &bboxMin, Vector3D &bboxMax)
	{

		return RETURN_OK;
	}

	void TerrainQuadtree::setIndexBuffer(IBuffer *newIndexBuffer)
	{
		this->m_indexBuffer = newIndexBuffer;
	}

	void TerrainQuadtree::setChunkSize(const int width, const int height)
	{
		this->m_maxChunkWidth = width;
		this->m_maxChunkHeight = height;
	}

	void TerrainQuadtree::setPosition(Vector3D position)
	{
		this->m_position = position;
	}

	void TerrainQuadtree::setEffect(IEffect *effect)
	{
		this->m_effect = effect;
	}

	RETURN_VALUE TerrainQuadtree::show()
	{
		if(this->m_viewFrustum==NULL)
			return RETURN_ERROR;

		if(!this->m_root)
			return RETURN_ERROR;

		this->m_renderer->saveWorld();
		this->m_renderer->translate(this->m_position.x, this->m_position.y, this->m_position.z);
		this->m_renderer->apply(0,0);
		this->show(this->m_root);
		this->m_renderer->restoreWorld();
        
		return RETURN_OK;
	}

	void TerrainQuadtree::setViewFrustum(ViewFrustum *viewFrustum)
	{
		this->m_viewFrustum = viewFrustum;
	}

	RETURN_VALUE TerrainQuadtree::show(TerrainQuadtreeNode *node)
	{
		//if(!this->m_viewFrustum->sphereInside(node->bsphereCenter+this->m_position,node->bsphereRadius))
		//	return RETURN_OK;

		if(!this->m_viewFrustum->boxInside(node->bboxMin+this->m_position, node->bboxMax+this->m_position))
			return RETURN_OK;

		if(node->isLeave)
		{
			//printf("x=%f y=%f z=%f x = %f y = %f z = %f\n",node->bboxMin.x,node->bboxMin.y,node->bboxMin.z,
			//											node->bboxMax.x,node->bboxMax.y,node->bboxMax.z);
			if(node->vertexBuffer!=NULL /*&& this->indexBuffer!=NULL*/)
			{
				//printf("x = %d y = %d width = %d height = %d\n", this->regionX,this->regionY, this->regionWidth, this->regionHeight);

				node->vertexBuffer->activate();
				//GLOBALS->getRenderer()->apply(0,0);
				//this->m_effect->apply(0,0);
				this->m_renderer->apply(0,0);
				GLOBALS->getRenderer()->drawIndexed(6*(node->width)*(node->height),0,0);
			}

			return RETURN_OK;
		}

		if(node->children[0])
			this->show(node->children[0]);
		if(node->children[1])
			this->show(node->children[1]);
		if(node->children[2])
			this->show(node->children[2]);
		if(node->children[3])
			this->show(node->children[3]);
        
		return RETURN_OK;
	}

	Terrain::Terrain():   m_indexBuffer(NULL),
						   m_lod(1),
						   m_vertices(NULL),
						   m_heightmap(NULL),
						   m_quadtree(NULL),
						   m_chunkWidth(256),
						   m_chunkHeight(256),
						   m_effect(NULL),
						   m_renderer(NULL),
						   m_texture1(NULL),
						   m_texture2(NULL)
	{
		this->m_scaleFactors = Vector3D(1,1,1);


	}

	Terrain::~Terrain()
	{
		SAFE_DELETE(this->m_indexBuffer);
		SAFE_ARRAY_DELETE(this->m_heightmap);

	}

	void Terrain::setScaleFactors(float x, float y, float z)
	{
		this->setScaleFactors(Vector3D(x,y,z));
	}

	void Terrain::setScaleFactors(Vector3D newFactors)
	{
		this->m_scaleFactors = newFactors;
	}

	void Terrain::setLOD(int lod)
	{
		this->m_lod = lod;
	}

	int Terrain::getLOD()
	{
		return this->m_lod;
	}

	void Terrain::setChunkSize(const unsigned int width, const unsigned int height)
	{
		this->m_chunkWidth  = width;
		this->m_chunkHeight = height;
	}

	void Terrain::setEffect(IEffect *effect)
	{
		this->m_effect = effect;
		this->m_quadtree->setEffect(this->m_effect);
	}

	RETURN_VALUE Terrain::loadFromFile(const char *filename, int width, int depth)
	{
		this->m_width = width;
		this->m_depth = depth;

		//load heightdata, into the heightbuffer
		return this->loadRawHeightmap(filename);
	}

	RETURN_VALUE Terrain::saveToFile(const char *filename)
	{
		FILE *file;
		fopen_s(&file, filename, "wb");
		if(!file)
			return RETURN_ERROR;

		if(!this->m_heightmap)
			return RETURN_ERROR;

		fwrite(this->m_heightmap,1,this->m_width*this->m_depth,file);

		fclose(file);

		return RETURN_OK;
	}

	RETURN_VALUE Terrain::generateFlat(int width, int depth, int height)
	{
		this->m_width = width;
		this->m_depth = depth;

		this->m_heightmap = new unsigned char[width*depth];
		memset(this->m_heightmap, height, width*depth);

		return RETURN_OK;
	}

	RETURN_VALUE Terrain::create()
	{
		if(!this->m_heightmap)
			return RETURN_ERROR;

		this->m_renderer = GLOBALS->getRenderer();

		m_vertices = new Vertex[(this->m_width*this->m_depth)/this->m_lod];


		float scaleX = this->m_scaleFactors.x;
		float scaleY = this->m_scaleFactors.y;
		float scaleZ = this->m_scaleFactors.z;

		unsigned char *softHeightmap = new unsigned char[this->m_width*this->m_depth];

		//soften terrain
		for(int i=0; i<this->m_depth; ++i)
		{
			for(int j=0; j<this->m_width; ++j)
			{	
				int index = i*(this->m_width)+j; 

				double newHeight;
				this->getHeight(i,j,newHeight);
				
				softHeightmap[index] = (unsigned char)newHeight;
			}
		}

		//compute height
		for(int i=0; i<this->m_depth; ++i)
		{
			for(int j=0; j<this->m_width; ++j)
			{	
				//index into the highmap, specified by current position
				int index = i*this->m_width+j; 

				//printf("HEIGHT[%d]: %d\n", i*width+j, this->m_heightmap[i*width+j]);
				m_vertices[index].position = Vector3D(j*scaleX, this->m_heightmap[index]*scaleY,i*scaleZ);
				this->m_vertices[index].texCoord0 = Vector2D((float)j/(this->m_width/50), (float)i/(this->m_depth/50));//Vector2D((float)j, (float)i);
				this->m_vertices[index].texCoord1 = Vector2D((float)j/(this->m_width/1), (float)i/(this->m_depth/1));
				//this->m_vertices[index].texCoord2 = Vector2D((float)j/(this->m_width/100), (float)i/(this->m_depth/100));
				//TODO remove this stuff
				//make color addition more flexible
				/*if(this->m_heightmap[index]>=0 && this->m_heightmap[index]<=10)
					m_vertices[index].color = Color(0.5f,0.55f,0.5f,1.0f);
				else if(this->m_heightmap[index]>=20 && this->m_heightmap[index]<=40)
					m_vertices[index].color = Color(0.55f,0.57f,0.55f,1.0f);
				else if(this->m_heightmap[index]>=40 && this->m_heightmap[index]<=80)
					m_vertices[index].color = Color(0.6f,0.6f,0.6f,1.0f);
				else	
					m_vertices[index].color = Color(0.65f,0.65f,0.65f,1.0f);*/
				
				float value = 0.005f*this->m_heightmap[index] + 0.6f;
				//m_vertices[index].color = Color(value,value,value,1.0f);

				this->m_vertices[index].normal = this->getNormal(j,i);
				//printf("normal: x = %f, y = %f, z = %f\n", this->m_vertices[index].normal.x, this->m_vertices[index].normal.y, this->m_vertices[index].normal.z);
			}
		}


		int chunkWidth  = this->m_chunkWidth+1;
		int chunkDepth = this->m_chunkHeight+1;

		DWORD *indices = new DWORD[6*(chunkWidth-1)*(chunkDepth-1)];
		memset(indices,0,sizeof(DWORD)*(chunkWidth-1)*(chunkDepth-1)*6);
		int index = 0;
		for(int i=0; i<chunkDepth-1; i+=this->m_lod)
		{
			for(int j=0; j<chunkWidth-1; j+=this->m_lod)
			{
				indices[index]   = i*chunkWidth+j;
				indices[index+1] = indices[index+4] = i*chunkWidth+(j+1);
				indices[index+2] = indices[index+3] = (i+1)*chunkWidth+j;
				indices[index+5] = (i+1)*chunkWidth+(j+1);

				index += 6;
			}
		}
		
		
		this->m_indexBuffer = GLOBALS->getRenderer()->createBuffer();
		this->m_indexBuffer->create(indices,4,6*(chunkWidth-1)*(chunkDepth-1),BT_INDEX);
		//position.y = -80*scaleY;

		this->position = Vector3D(-(this->m_width*scaleX)/2, 0,-(this->m_depth*scaleZ)/2);

		this->m_quadtree = new TerrainQuadtree;
		this->m_quadtree->setPosition(Vector3D(-(this->m_width*scaleX)/2, 0,-(this->m_depth*scaleZ)/2));
		this->m_quadtree->setChunkSize(this->m_chunkWidth,this->m_chunkHeight);
		this->m_quadtree->build(this->m_vertices,0,0,this->m_width-1, this->m_depth-1);
		this->m_quadtree->setIndexBuffer(this->m_indexBuffer);

		this->m_effect = GLOBALS->getRenderer()->createEffect();
		if(!this->m_effect)
			return RETURN_EFFECT_ERROR;

		RETURN_VALUE result = this->m_effect->create("..\\data\\shaders\\terrain.fx");
		if(result!=RETURN_OK)
			return result;

		SAFE_ARRAY_DELETE(this->m_heightmap);
		return RETURN_OK;
	}

	void Terrain::setColorMap(ITexture2D *texture)
	{
		this->m_texture2 = texture;
	}

	void Terrain::setDetailMap(ITexture2D *texture)
	{
		this->m_texture1 = texture;
	}

	RETURN_VALUE Terrain::loadRawHeightmap(const char *filename)
	{
		FILE *file;
		fopen_s(&file, filename, "rb");
		if(!file)
			return RETURN_ERROR;

		unsigned int size = this->m_width*this->m_depth;

		//mirrow the hightmap for correction
		//unsigned char* hightmapMirroredData = new unsigned char[size];

		//corrected heightmap
		SAFE_DELETE(this->m_heightmap);
		this->m_heightmap = new unsigned char[size];

		unsigned char header[100];
		fread(header,1,100,file);
		
		fread(this->m_heightmap,1,size,file);

		fclose(file);
		
		
		//mirror
		/*for(int i=0; i<this->m_width*this->m_depth; ++i)
		{
			this->m_heightmap[i] = hightmapMirroredData[this->m_width*(this->m_depth)-(i+1)];
		}
		SAFE_ARRAY_DELETE(hightmapMirroredData);
		*/

		//for height swapping
		//unsigned char temp;

		/*//mirror on y axis
		for(int y=0; y<this->m_depth; ++y)
		{
			for(int x=0; x<this->m_width/2; ++x)
			{
				
				temp = this->m_heightmap[y*this->m_width+x];
				this->m_heightmap[y*this->m_width+x] = this->m_heightmap[y*this->m_width+(this->m_width-x)];
			    this->m_heightmap[y*this->m_width+(this->m_width-x)] = temp;
			}
		}*/

		/*//mirror on x axis
		for(int y=0; y<this->m_depth; ++y)
		{
			for(int x=0; x<this->m_width/2; ++x)
			{
				
				temp = this->m_heightmap[y*this->m_width+x];
				this->m_heightmap[y*this->m_width+x] = this->m_heightmap[(y+(this->m_depth-y))*this->m_width+(this->m_width-x)];
			    this->m_heightmap[(y+(this->m_depth-y))*this->m_width+(this->m_width-x)] = temp;
			}
		}*/

		return RETURN_OK;
		//printf("NUM heights %d\n",numHeight);
	}

	unsigned char* Terrain::getHeightmap()
	{

		return this->m_heightmap;
	}

	Vector3D Terrain::getNormal(int x, int z)
	{
		//calculate normals through finding triangle endges of current position 
		//and compute the cross product

		if(x>(this->m_width-1) || z>(this->m_depth-1))
			return Vector3D(0,0,0);

		//final vertex normal
		Vector3D normal;

		//consider the maximum width/height position
		int faktorX = 1;//x>=this->m_width?-1:1;
		int faktorZ = 1;//z>=this->m_depth?-1:1;

		//get the heights
		float y0 = this->m_heightmap[z*faktorZ*this->m_width+x*faktorX]*this->m_scaleFactors.y;
		float y1 = this->m_heightmap[z*faktorZ*this->m_width+(x+1)*faktorX]*this->m_scaleFactors.y;
		float y2 = this->m_heightmap[(z+1)*faktorZ*this->m_width+x*faktorX]*this->m_scaleFactors.y;

		Vector3D Edge1 = Vector3D(1,y1-y0,0);
		Vector3D Edge2 = Vector3D(0,y2-y0,1);

		//compute the normal of Edge1 and Edge2
		D3DXVec3Cross(&normal, &Edge2, &Edge1);
		D3DXVec3Normalize(&normal, &normal);

		return normal;
	}

	RETURN_VALUE Terrain::getHeight(double x, double z, double &height)
	{
		//x = (float)floor(x+this->m_width/2);
		//z = (float)floor(z+this->m_depth/2);
		x -= this->position.x;
		z -= this->position.z;

		if(x>=0 && x<(this->m_width-1) && z>=0 && z<(this->m_depth-1))
		{
			//get 8 points around the x,z coordinates
			int index1=(int)((int)(z+1)*(int)this->m_width+(int)(x-1));
			int index2=(int)((int)(z+1)*(int)this->m_width+(int)(x));
			int index3=(int)((int)(z+1)*(int)this->m_width+(int)(x+1));
			int index4=(int)((int)(z)*(int)this->m_width+(int)(x-1));
			int index5=(int)((int)(z)*(int)this->m_width+(int)(x+1));
			int index6=(int)((int)(z-1)*(int)this->m_width+(int)(x+1));
			int index7=(int)((int)(z-1)*(int)this->m_width+(int)(x));
			int index8=(int)((int)(z-1)*(int)this->m_width+(int)(x+1));
			int index9=(int)((int)(z)*(int)this->m_width+(int)(x));

			//printf("x = %d y = %d index = %d\n",(int)x,(int)z,index);
			float height1 = this->m_vertices[index1].position.y;
			float height2 = this->m_vertices[index2].position.y;
			float height3 = this->m_vertices[index3].position.y;
			float height4 = this->m_vertices[index4].position.y;
			float height5 = this->m_vertices[index5].position.y;
			float height6 = this->m_vertices[index6].position.y;
			float height7 = this->m_vertices[index7].position.y;
			float height8 = this->m_vertices[index8].position.y;
			float height9 = this->m_vertices[index9].position.y;

			//out height is the average over the 4 corners
			height = (height1+height2+height3+height4+height5+height6+height7+height8+height9)/9;

			return RETURN_OK;
		}
		
		return RETURN_ERROR;
	}

	void Terrain::draw(ViewFrustum *viewFrustum)
	{
		IEffect *oldEffect = GLOBALS->getRenderer()->getEffect();
		this->m_renderer->pushEffect();
		if(this->m_renderer->setEffect(this->m_effect)!=RETURN_OK)
			return;
		this->m_quadtree->setEffect(this->m_effect);
		this->m_renderer->setTexturing(true);

		if(this->m_texture1)
			this->m_renderer->setTexture(this->m_texture1);
			//this->m_effect->setTexture("gTexture2D0", this->m_texture1);
		if(this->m_texture2)
			this->m_renderer->setTexture(this->m_texture2,1);
			//this->m_effect->setTexture("gTexture2D1", this->m_texture2);

		//this->m_renderer->apply(0,0);
		
		this->m_effect->setInt("time",(int)Timer::getTickCount());
		//this->m_effect->apply(0,0);
		this->m_renderer->apply(0,0);
		GLOBALS->getRenderer()->setCullMode(CULL_MODE_BACK);
		//GLOBALS->getRenderer()->setLighting(false);
		this->m_renderer->setFrontOrder(false);

		this->m_renderer->setPrimitive(PT_TRIANGLELIST);
		this->m_indexBuffer->activate();
		this->m_quadtree->setViewFrustum(viewFrustum);
		this->m_quadtree->show();

		//GLOBALS->getRenderer()->setEffect(oldEffect);
		this->m_renderer->popEffect();
		this->m_renderer->setFrontOrder(true);
		//GLOBALS->getRenderer()->setLighting(true);
	}
}
