/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/skybox.h"

namespace clearsky
{
	Skybox::Skybox():m_indexBuffer(NULL),
					 m_vertexBuffer(NULL),
					 m_scaleFactor(1500)
	{
		this->m_renderer=NULL;

		this->m_textureNorth = NULL;
		this->m_textureSouth = NULL;
		this->m_textureEast  = NULL;
		this->m_textureWest  = NULL;
		this->m_textureUp    = NULL;
		this->m_textureDown  = NULL;

		this->m_samplerState.addressU = TEXTURE_ADDRESS_WRAP;
		this->m_samplerState.addressV = TEXTURE_ADDRESS_WRAP;
	}

	Skybox::~Skybox()
	{
		SAFE_DELETE(this->m_indexBuffer);
		SAFE_DELETE(this->m_vertexBuffer);

		SAFE_DELETE(this->m_textureNorth);
		SAFE_DELETE(this->m_textureSouth);
		SAFE_DELETE(this->m_textureEast);
		SAFE_DELETE(this->m_textureWest);
		SAFE_DELETE(this->m_textureUp);
		SAFE_DELETE(this->m_textureDown);
	}

	RETURN_VALUE Skybox::create(Camera *camera)
	{
		this->m_renderer = GLOBALS->getRenderer();

		if(!camera || !this->m_renderer)
			return RETURN_ERROR;

		this->m_camera = camera;


		this->m_indexBuffer  = this->m_renderer->createBuffer();
		this->m_vertexBuffer = this->m_renderer->createBuffer();

		Vertex vertices[8];

		vertices[0].position = Vector3D(-1.0f, -1.0f, 0.0f);
		vertices[0].texCoord0 = Vector2D(1,1);
		vertices[0].normal = Vector3D(-1.0f, -1.0f, -1.0f);
		vertices[1].position = Vector3D(-1.0f, +1.0f, 0.0f);
		vertices[1].texCoord0 = Vector2D(1,0);
		vertices[1].normal = Vector3D(-1.0f, 1.0f, -1.0f);
		vertices[2].position = Vector3D(+1.0f, +1.0f, 0.0f);
		vertices[2].texCoord0  = Vector2D(0,0);
		vertices[2].normal = Vector3D(1.0f, 1.0f, -1.0f);
		vertices[3].position = Vector3D(+1.0f, -1.0f, 0.0f);
		vertices[3].texCoord0 = Vector2D(0,1);
		vertices[3].normal = Vector3D(1.0f, -1.0f, -1.0f);
		vertices[4].position = Vector3D(-1.0f, -1.0f, +1.0f);
		vertices[4].texCoord0  = Vector2D(1,1);
		vertices[4].normal = Vector3D(-1.0f, -1.0f, 1.0f);
		vertices[5].position = Vector3D(-1.0f, +1.0f, +1.0f);
		vertices[5].texCoord0 = Vector2D(1,0);
		vertices[5].normal = Vector3D(-1.0f, 1.0f, 1.0f);
		vertices[6].position = Vector3D(+1.0f, +1.0f, +1.0f);
		vertices[6].texCoord0 = Vector2D(0,0);
		vertices[6].normal = Vector3D(1.0f, 1.0f, 1.0f);
		vertices[7].position = Vector3D(+1.0f, -1.0f, +1.0f);
		vertices[7].texCoord0 = Vector2D(0,1);
		vertices[7].normal = Vector3D(1.0f, -1.0f, 1.0f);

		RETURN_VALUE result = this->m_vertexBuffer->create(vertices, sizeof(Vertex),8);
		if(result!=RETURN_OK)
			return result;

		DWORD indices[] = 
		{
			// front face south
			0, 1, 2,
			0, 2, 3,

			// back face north
			4, 6, 5,
			4, 7, 6,

			// left face west
			4, 5, 1,
			4, 1, 0,

			// right face east
			3, 2, 6,
			3, 6, 7,

			// top face top
			1, 5, 6,
			1, 6, 2,

			// bottom face down
			4, 0, 3,
			4, 3, 7
		};

		result = this->m_indexBuffer->create(indices, sizeof(DWORD), 12*3, clearsky::BT_INDEX);
		if(result!=RETURN_OK)
			return result;

		return RETURN_OK;
	}

	void Skybox::draw()
	{
		this->m_vertexBuffer->activate();
		this->m_indexBuffer->activate();

		SamplerState *oldSamplerState = this->m_renderer->getSamplerState();
		this->m_renderer->setSamplerState(&this->m_samplerState);

		bool lighting = this->m_renderer->isLighting();
		CULL_MODE cullMode = this->m_renderer->setCullMode(CULL_MODE_FRONT);
		 this->m_renderer->setLighting(false);
		 this->m_renderer->setTexturing(true);

		//GLOBALS->getDXDevice()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		this->m_renderer->setPrimitive(PT_TRIANGLELIST);
		//this->m_renderer->scale(this->m_scaleFactor,this->m_scaleFactor,this->m_scaleFactor);
		//this->m_renderer->translate(this->m_camera->position.x,this->m_camera->position.y,this->m_camera->position.z);
		//this->m_renderer->apply(0,0);

		this->m_renderer->setCullMode(CULL_MODE_NONE);

		//north side
		this->m_renderer->saveWorld();
			
		this->m_renderer->setTexture(this->m_textureNorth);
		//this->m_renderer->setTexture(this->m_textureNorth);
		this->m_renderer->scale(this->m_scaleFactor,this->m_scaleFactor,this->m_scaleFactor);
		this->m_renderer->rotateY(180);
		this->m_renderer->translate(0,0,this->m_scaleFactor);
		this->m_renderer->translate(this->m_camera->position.x,this->m_camera->position.y,this->m_camera->position.z);		
		
		this->m_renderer->apply(0,0);
		this->m_renderer->drawIndexed(6,6*0,0);
		this->m_renderer->restoreWorld();

		//south side
		this->m_renderer->saveWorld();
		this->m_renderer->setTexture(this->m_textureSouth);	
		this->m_renderer->scale(this->m_scaleFactor,this->m_scaleFactor,this->m_scaleFactor);
		//this->m_renderer->rotateY(-180);
		this->m_renderer->translate(0,0,-this->m_scaleFactor);
		this->m_renderer->translate(this->m_camera->position.x,this->m_camera->position.y,this->m_camera->position.z);
		this->m_renderer->apply(0,0);
		this->m_renderer->drawIndexed(6,6*0,0);
		this->m_renderer->restoreWorld();

		//west side
		this->m_renderer->saveWorld();
		this->m_renderer->setTexture(this->m_textureWest);	
		this->m_renderer->scale(this->m_scaleFactor,this->m_scaleFactor,this->m_scaleFactor);
		this->m_renderer->rotateY(90);
		this->m_renderer->translate(-this->m_scaleFactor,0,0);
		this->m_renderer->translate(this->m_camera->position.x,this->m_camera->position.y,this->m_camera->position.z);		
		this->m_renderer->apply(0,0);
		this->m_renderer->drawIndexed(6,6*0,0);
		this->m_renderer->restoreWorld();
		
		//east side
		this->m_renderer->saveWorld();
		this->m_renderer->setTexture(this->m_textureEast);	
		this->m_renderer->scale(this->m_scaleFactor,this->m_scaleFactor,this->m_scaleFactor);
		this->m_renderer->rotateY(-90);
		this->m_renderer->translate(this->m_scaleFactor,0,0);
		this->m_renderer->translate(this->m_camera->position.x,this->m_camera->position.y,this->m_camera->position.z);		
		this->m_renderer->apply(0,0);
		this->m_renderer->drawIndexed(6,6*0,0);
		this->m_renderer->restoreWorld();

		//up side
		this->m_renderer->saveWorld();
		this->m_renderer->setTexture(this->m_textureUp);	
		this->m_renderer->scale(this->m_scaleFactor,this->m_scaleFactor,this->m_scaleFactor);
		this->m_renderer->rotateX(90);
		//this->m_renderer->rotateY(180);
		this->m_renderer->translate(0,this->m_scaleFactor,0);
		this->m_renderer->translate(this->m_camera->position.x,this->m_camera->position.y,this->m_camera->position.z);		
		this->m_renderer->apply(0,0);
		this->m_renderer->drawIndexed(6,6*0,0);
		this->m_renderer->restoreWorld();

		//down side
		this->m_renderer->saveWorld();
		this->m_renderer->setTexture(this->m_textureDown);	
		this->m_renderer->scale(this->m_scaleFactor,this->m_scaleFactor,this->m_scaleFactor);
		this->m_renderer->rotateX(-90);
		//this->m_renderer->rotateY(90);
		this->m_renderer->translate(0,-this->m_scaleFactor,0);
		this->m_renderer->translate(this->m_camera->position.x,this->m_camera->position.y,this->m_camera->position.z);		
		this->m_renderer->apply(0,0);
		this->m_renderer->drawIndexed(6,6*0,0);
		this->m_renderer->restoreWorld();

		this->m_renderer->setSamplerState(oldSamplerState);
		this->m_renderer->setLighting(lighting);
		this->m_renderer->setCullMode(cullMode);	
	}

	RETURN_VALUE Skybox::setTextures(ITexture2D *north, ITexture2D *south,
								ITexture2D *west, ITexture2D *east,
								ITexture2D *up, ITexture2D *down)
	{

		this->m_textureNorth = north;
		this->m_textureSouth = south;
		this->m_textureEast  = east;
		this->m_textureWest  = west;
		this->m_textureUp    = up;
		this->m_textureDown  = down;

		return RETURN_OK;
	}

	RETURN_VALUE Skybox::setTextures(const char *north, const char *south,
									const char *west, const char *east,
									const char *up, const char *down)
	{
		IRenderer *renderer = GLOBALS->getRenderer();

		if(!renderer)
			return RETURN_ERROR;

		//release previeous textures
		SAFE_DELETE(this->m_textureNorth);
		SAFE_DELETE(this->m_textureSouth);
		SAFE_DELETE(this->m_textureEast);
		SAFE_DELETE(this->m_textureWest);
		SAFE_DELETE(this->m_textureUp);
		SAFE_DELETE(this->m_textureDown);


		//load textures
		//this->m_textureNorth = renderer->createTexture2D();
		this->m_textureNorth = Resources->loadTexture2D(north);
		if(!this->m_textureNorth)
			return RETURN_ERROR;
		//if(this->m_textureNorth->loadFile(north)!=RETURN_OK)
		//	return RETURN_ERROR;

		//this->m_textureSouth = renderer->createTexture2D();
		this->m_textureSouth = Resources->loadTexture2D(south);
		if(!this->m_textureSouth)
			return RETURN_ERROR;
		/*if(this->m_textureSouth->loadFile(south)!=RETURN_OK)
			return RETURN_ERROR;*/
		
		
		//this->m_textureWest = renderer->createTexture2D();
		this->m_textureWest = Resources->loadTexture2D(west);
		if(!this->m_textureWest)
			return RETURN_ERROR;
		/*if(this->m_textureWest->loadFile(west)!=RETURN_OK)
			return RETURN_ERROR;*/

		//this->m_textureEast = renderer->createTexture2D();
		this->m_textureEast = Resources->loadTexture2D(east);
		if(!this->m_textureEast)
			return RETURN_ERROR;
		/*if(this->m_textureEast->loadFile(east)!=RETURN_OK)
			return RETURN_ERROR;*/

		//this->m_textureUp = renderer->createTexture2D();
		this->m_textureUp = Resources->loadTexture2D(up);
		if(!this->m_textureUp)
			return RETURN_ERROR;
		/*if(this->m_textureUp->loadFile(up)!=RETURN_OK)
			return RETURN_ERROR;*/

		//this->m_textureDown = renderer->createTexture2D();
		this->m_textureDown = Resources->loadTexture2D(down);
		if(!this->m_textureDown)
			return RETURN_ERROR;
		/*if(this->m_textureDown->loadFile(down)!=RETURN_OK)
			return RETURN_ERROR;*/

		return RETURN_OK;
	}

	void Skybox::setScaleFactor(float scale)
	{
		this->m_scaleFactor = scale;
	}

}
