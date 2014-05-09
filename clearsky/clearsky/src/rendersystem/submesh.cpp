/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/mesh.h"

namespace clearsky
{
	MOFSubmesh::MOFSubmesh():m_vertexBuffer(NULL),
				m_indexBuffer(NULL),
				m_vertexData(NULL),
				m_indexData(NULL),
				m_numVertices(0),
				m_numElementsVertex(4),
				m_numIndices(0),
				m_visible(true),
				m_primitiveTopology(PT_TRIANGLELIST),
				m_frontCounterClockwise(false)
	{
		D3DXMatrixIdentity(&this->m_world);
		strncpy_s(this->m_name,"Mesh",149);

		this->m_samplerState.addressU = TEXTURE_ADDRESS_WRAP;
		this->m_samplerState.addressV = TEXTURE_ADDRESS_WRAP;
	}

	MOFSubmesh::~MOFSubmesh()
	{
		//SAFE_ARRAY_DELETE(this->m_vertexData);
		//SAFE_ARRAY_DELETE(this->m_indexData);
		SAFE_DELETE(this->m_vertexBuffer);
		SAFE_DELETE(this->m_indexBuffer);

		for(unsigned int i=0; i<this->m_materials.size(); ++i)
			SAFE_DELETE(this->m_materials[i]);
			
		for(unsigned int i=0; i<this->m_textures.size(); ++i)
			SAFE_DELETE(this->m_textures[i]);

		printf("submesh deleted\n");
	}

	void MOFSubmesh::draw()
	{
		if(!this->m_visible)
			return;

		IRenderer* renderer = GLOBALS->getRenderer();

		PRIMITIVE_TOPOLOGY topology = renderer->setPrimitive(this->m_primitiveTopology);

		renderer->saveWorld();

		//renderer->multWorld(this->m_world);
		renderer->setWorld(this->m_world);
		//renderer->apply(0,0);

		if(this->m_vertexBuffer)
				this->m_vertexBuffer->activate();

		if(this->m_indexBuffer)
				this->m_indexBuffer->activate();
			
		SamplerState *oldSamplerState = renderer->getSamplerState();
		renderer->setSamplerState(&this->m_samplerState);

		bool texturing = renderer->isTexturing();
		bool specTexturing = renderer->isSpecTexturing();
		bool heightTexturing = renderer->isHeightTexturing();

		if(this->m_textures.size()>0)
			renderer->setTexturing(true);
		else
			renderer->setTexturing(false);

		if(this->m_specularTextures.size()>0)
			renderer->setSpecTexturing(true);
		else
			renderer->setSpecTexturing(false);

		if(this->m_heightTextures.size()>0)
			renderer->setHeightTexturing(true);
		else
			renderer->setHeightTexturing(false);
		
		renderer->setLighting(true);

		//bool lastFaceOrder = renderer->setFrontOrder(this->m_frontCounterClockwise);
		CULL_MODE oldCullMode = renderer->getCullMode();
		//if(this->m_frontCounterClockwise)
		//TODO: set proper backface culling
		
		renderer->setCullMode(CULL_MODE_NONE);

		if(this->m_materials.size()<=0)
		{
			renderer->apply(0,0);
			if(this->m_indexBuffer)
			{
				renderer->drawIndexed(this->m_numIndices*3,0,0);
			}
			else
				renderer->draw(this->m_numVertices,0);
		}
		else
		{
			//bool lastOrder = renderer->setFrontOrder(false);
			Material oldMaterial;
			oldMaterial = renderer->getMaterial();

			
			for(unsigned int i=0; i<this->m_materials.size(); ++i)
			{		
				if((this->m_materials[i]->texture!=NULL) && (!this->m_textures.empty()))
				{
					renderer->setTexture(this->m_materials[i]->texture); 
				}

				if((this->m_materials[i]->specularTexture!=NULL) && (!this->m_specularTextures.empty()))
				{
					renderer->setSpecTexture(this->m_materials[i]->specularTexture); 
				}

				if((this->m_materials[i]->heightmapTexture!=NULL) && (!this->m_heightTextures.empty()))
				{
					renderer->setHeightTexture(this->m_materials[i]->heightmapTexture); 
				}

				renderer->setMaterial(*this->m_materials[i]);
				renderer->apply(0,0);

				if(this->m_indexBuffer)
				{
					renderer->drawIndexed(this->m_materials[i]->numFaces*3,this->m_materials[i]->startFace*3,0);
				}
				else
					renderer->draw(this->m_numVertices,0);	
			}

				
			renderer->setMaterial(oldMaterial);
			//renderer->setFrontOrder(lastOrder);
		}

		//renderer->setFrontOrder(lastFaceOrder);
		renderer->setCullMode(oldCullMode);
		renderer->setTexturing(texturing);
		renderer->setSpecTexturing(specTexturing);
		renderer->setHeightTexturing(heightTexturing);
		renderer->restoreWorld();	
		renderer->setPrimitive(topology);	
		renderer->setSamplerState(oldSamplerState);
	}

	void MOFSubmesh::rotateY(float deg)
	{
		Matrix yRotation;
		D3DXMatrixRotationY(&yRotation, DEG_TO_RAD(deg));
		this->m_world *= yRotation;

		//updateBoundingBox();
	}

	void MOFSubmesh::rotateX(float deg)
	{
		Matrix xRotation;
		D3DXMatrixRotationX(&xRotation, DEG_TO_RAD(deg));
		this->m_world *= xRotation;

		//updateBoundingBox();
	}

	void MOFSubmesh::rotateZ(float deg)
	{
		Matrix zRotation;
		D3DXMatrixRotationZ(&zRotation, DEG_TO_RAD(deg));
		this->m_world *= zRotation;

		//updateBoundingBox();
	}

	void MOFSubmesh::translate(float x, float y, float z)
	{
		Matrix translation;
		D3DXMatrixTranslation(&translation, x, y, z);
		this->m_world *= translation;

		//updateBoundingBox();
	}

	void MOFSubmesh::scale(float factorX, float factorY, float factorZ)
	{
		Matrix scaling;
		D3DXMatrixScaling(&scaling, factorX, factorY, factorZ);
		this->m_world *= scaling;

		//updateBoundingBox();
	}

	void MOFSubmesh::transform(Matrix *transformMat)
	{
		this->m_world *= *transformMat;

		//updateBoundingBox();
	}

	void MOFSubmesh::resetTransform()
	{
		D3DXMatrixIdentity(&this->m_world);

		//updateBoundingBox();
	}

	Vector3D MOFSubmesh::getPosition()
	{
		Vector3D position = Vector3D(this->m_world._41, this->m_world._42, this->m_world._43);
		return position;
	}

	void MOFSubmesh::setRotation(float degX, float degY, float degZ)
	{
		Matrix yRotation;
		D3DXMatrixRotationY(&yRotation, DEG_TO_RAD(degY));

		Matrix xRotation;
		D3DXMatrixRotationX(&xRotation, DEG_TO_RAD(degX));

		Matrix zRotation;
		D3DXMatrixRotationZ(&zRotation, DEG_TO_RAD(degZ));
		
		this->m_world *= (zRotation*yRotation*xRotation);

		//updateBoundingBox();
	}

	void MOFSubmesh::setPosition(float x, float y, float z)
	{
		Matrix translation;
		D3DXMatrixTranslation(&translation, x, y, z);
		this->m_world *= translation;

		//updateBoundingBox();
	}

	void MOFSubmesh::setSize(float factorX, float factorY, float factorZ)
	{
		Matrix scaling;
		D3DXMatrixScaling(&scaling, factorX, factorY, factorZ);
		this->m_world *= scaling;

		//updateBoundingBox();
	}

	void MOFSubmesh::addTexture2D(ITexture2D *texture)
	{
		this->m_textures.push_back(texture);
	}

	std::vector<ITexture2D*> MOFSubmesh::getTextures()
	{
		return this->m_textures;
	}

	void MOFSubmesh::addSpecTexture2D(ITexture2D *texture)
	{
		this->m_specularTextures.push_back(texture);
	}

	std::vector<ITexture2D*> MOFSubmesh::getSpecTextures()
	{
		return this->m_specularTextures;
	}

	void MOFSubmesh::addHeightTexture2D(ITexture2D *texture)
	{
		this->m_heightTextures.push_back(texture);
	}

	std::vector<ITexture2D*> MOFSubmesh::getHeightTextures()
	{
		return this->m_heightTextures;
	}

	void MOFSubmesh::setTextureBase(std::string textureBasePath)
	{
		this->m_textureBasePath = textureBasePath;
	}

	void MOFSubmesh::setVertexBuffer(IBuffer *vertexBuffer)
	{
		if(this->m_vertexBuffer)
		{
			this->m_vertexBuffer->release();
			this->m_vertexBuffer = NULL;
		}

		this->m_vertexBuffer = vertexBuffer;
	}

	IBuffer* MOFSubmesh::getVertexBuffer()
	{
		return this->m_vertexBuffer;
	}

	void MOFSubmesh::setIndexBuffer(IBuffer *indexBuffer)
	{
		if(this->m_indexBuffer)
		{
			this->m_indexBuffer->release();
			this->m_indexBuffer = NULL;
		}

		this->m_indexBuffer = indexBuffer;
	}

	IBuffer* MOFSubmesh::getIndexBuffer()
	{
		return this->m_indexBuffer;
	}


	void MOFSubmesh::setVisible(bool visible)
	{
		this->m_visible = visible;
	}

	bool MOFSubmesh::isVisible()
	{
		return this->m_visible;
	}

	int MOFSubmesh::getNumVertices()
	{
		return this->m_numVertices;
	}

	void MOFSubmesh::setNumVertices(int num)
	{
		this->m_numVertices = num;
	}

	int MOFSubmesh::getNumElementsVertex()
	{
		return this->m_numElementsVertex;
	}

	void MOFSubmesh::setNumElementsVertex(int num)
	{
		this->m_numElementsVertex = num;
	}

	int MOFSubmesh::getNumIndices()
	{
		return this->m_numIndices;
	}

	void MOFSubmesh::setNumIndices(int num)
	{
		this->m_numIndices = num;
	}

	int MOFSubmesh::getNumMaterials()
	{
		return this->m_numMaterials;
	}

	void MOFSubmesh::setNumMaterials(int num)
	{
		this->m_numMaterials = num;
	}

	void MOFSubmesh::addMaterial(Material *material)
	{
		this->m_materials.push_back(material);
	}

	void MOFSubmesh::addVertexData(Vertex *vertexData, int numVerts)
	{
		SAFE_ARRAY_DELETE(this->m_vertexData);
		this->m_vertexData = vertexData;
		this->m_numVertices = numVerts;

		this->buildVBuffer();
	}

	void MOFSubmesh::addIndexData(DWORD *indexData, int numIndices)
	{
		SAFE_ARRAY_DELETE(this->m_indexData);
		this->m_indexData = indexData;
		this->m_numIndices = numIndices;

		this->buildIBuffer();
	}

	RETURN_VALUE MOFSubmesh::buildVBuffer()
	{
		if(!this->m_vertexData)
			return RETURN_ERROR;

		SAFE_DELETE(this->m_vertexBuffer);

		this->m_vertexBuffer = GLOBALS->getRenderer()->createBuffer();
		if(!this->m_vertexBuffer)
			return RETURN_ERROR;

		if(this->m_vertexBuffer->create(this->m_vertexData,sizeof(Vertex),this->m_numVertices)!=RETURN_OK)
			return RETURN_ERROR;

		return RETURN_OK;
	}

	RETURN_VALUE MOFSubmesh::buildIBuffer()
	{
		if(!this->m_indexData)
			return RETURN_ERROR;

		SAFE_DELETE(this->m_indexBuffer);

		this->m_indexBuffer = GLOBALS->getRenderer()->createBuffer();
		if(!this->m_indexBuffer)
			return RETURN_ERROR;

		if(this->m_indexBuffer->create(this->m_indexData,4,this->m_numIndices,BT_INDEX)!=RETURN_OK)
			return RETURN_ERROR;

		return RETURN_OK;
	}

	PRIMITIVE_TOPOLOGY MOFSubmesh::setPrimitiveTopology(PRIMITIVE_TOPOLOGY newTopology)
	{
		PRIMITIVE_TOPOLOGY oldTopology = this->m_primitiveTopology;

		this->m_primitiveTopology = newTopology;

		return oldTopology;
	}

	PRIMITIVE_TOPOLOGY MOFSubmesh::getPrimitiveTopology()
	{
		return this->m_primitiveTopology;
	}

	void MOFSubmesh::setFrontOrder(bool ccw)
	{
		this->m_frontCounterClockwise = ccw;
	}

	bool MOFSubmesh::frontOrderCCW()
	{
		return this->m_frontCounterClockwise;
	}

	void MOFSubmesh::setBoundingBox(AABB &aabb)
	{
		this->m_aabb = aabb;
	}

	AABB MOFSubmesh::getBoudingBox()
	{
		AABB aabb = updateBoundingBox();
		aabb.recompute();
		//transform bounding box
		return aabb;
		//return &this->m_aabb;
	}

	AABB MOFSubmesh::updateBoundingBox()
	{
		AABB newAABB;

		Vector4D minVec = (Vector4D)this->m_aabb.min;//Vector4D(this->m_aabb.min.x, this->m_aabb.min.y, this->m_aabb.min.z, 1.0f);
		Vector4D maxVec = (Vector4D)this->m_aabb.max;//Vector4D(this->m_aabb.max.x, this->m_aabb.max.y, this->m_aabb.max.z, 1.0f);

		D3DXVec3Transform(&minVec, &this->m_aabb.min, &this->m_world);
		D3DXVec3Transform(&maxVec, &this->m_aabb.max, &this->m_world);

		newAABB.min = (Vector3D)minVec;
		newAABB.max = (Vector3D)maxVec;

		return newAABB;
	}

	bool MOFSubmesh::rayCollision(const Vector3D rayOrigin, const Vector3D rayDirection)
	{
		return this->m_aabb.rayCollision(rayOrigin, rayDirection);
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

	std::vector<Bone*> MOFSubmesh::getBones()
	{
		return this->m_bones;
	}
}

