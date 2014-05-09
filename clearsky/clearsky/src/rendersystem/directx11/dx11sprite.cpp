/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/directx11/DX11Sprite.h"

namespace clearsky
{
	DX11Sprite::DX11Sprite(ID3D11DeviceContext *immediateContext)://m_sprite(NULL),
					 m_curFrame(0),
					 m_numFrames(0),
					 m_width(100),
					 m_height(100),
					 m_texture(NULL),
					 m_cameraMatrix(NULL),
					 m_visible(true)
	{
		this->m_immediateContext = immediateContext;
		immediateContext->GetDevice(&this->m_dxdevice);

		D3DXMatrixIdentity(&this->m_world);

		this->m_renderer = GLOBALS->getRenderer();

		this->m_depthState = new DepthStencilState;
		this->m_depthState->depthEnable = false;
	}

	DX11Sprite::DX11Sprite(const char *filename, int spriteWidth, int spriteHeight, int num,int maxFramesX, int maxFramesY)
	{
		this->init(filename, spriteWidth, spriteHeight, num, maxFramesX, maxFramesY);
	}

	DX11Sprite::~DX11Sprite()
	{
		this->release();
	}

	RETURN_VALUE DX11Sprite::init(const char *filename, int spriteWidth, int spriteHeight, int num, int maxFramesX, int maxFramesY)
	{
		this->m_texture = this->m_renderer->createTexture2D();
		this->m_texture->loadFile(filename);
		if(!this->m_texture)
			return RETURN_ERROR;

		this->m_vertexBuffer = this->m_renderer->createBuffer();
		this->m_vertexBuffer->setCPUUsage(BCU_WRITE);
		this->m_vertexBuffer->setBufferUsage(BU_DYNAMIC);

		this->m_numFrames = num;

		this->m_deltaUV.x = (float)spriteWidth/this->m_texture->getWidth();
		this->m_deltaUV.y = (float)spriteHeight/this->m_texture->getHeight();

		this->m_startUV = Vector2D(0,0);
		this->m_endUV   = Vector2D(0, 0);

		Vertex rect[6];
		rect[0].position = Vector3D(0,0,0);
		rect[0].texCoord0 = Vector2D(0,this->m_deltaUV.y);
		rect[1].position = Vector3D(0,0,0);
		rect[1].texCoord0 = Vector2D(this->m_deltaUV.x,this->m_deltaUV.y);
		rect[2].position = Vector3D(0,0,0);
		rect[2].texCoord0 = Vector2D(0,0);
		rect[3].position = Vector3D(0,0,0);
		rect[3].texCoord0 = Vector2D(0,0);
		rect[4].position = Vector3D(0,0,0);
		rect[4].texCoord0 = Vector2D(this->m_deltaUV.x,this->m_deltaUV.y);
		rect[5].position = Vector3D(0,0,0);
		rect[5].texCoord0 = Vector2D(this->m_deltaUV.x,0);

		this->m_vertexBuffer->create(&rect,sizeof(Vertex),6);

		this->m_width  = spriteWidth;
		this->m_height = spriteHeight;
		this->m_maxFramesX = maxFramesX;
		this->m_maxFramesY = maxFramesY;
		this->m_curFrame = -1;

		return RETURN_OK;
	}

	void DX11Sprite::show()
	{
		if(!this->m_visible)
			return;

		if(this->m_texture)
			this->m_renderer->setTexture(this->m_texture);
		
		this->m_renderer->saveWorld();

		Matrix world;
		D3DXMatrixIdentity(&world);
		if(this->m_cameraMatrix)
			world *= *this->m_cameraMatrix;
		world *= this->m_world;

		this->m_renderer->setWorld(world);

		this->m_renderer->setFrontOrder(true);
		this->m_renderer->setCullMode(CULL_MODE_NONE);

		this->m_renderer->pushDepthStencilState();
		//this->m_renderer->setDepthStencilState(this->m_depthState);

		this->m_renderer->apply(0,0);
		this->m_vertexBuffer->activate();
		this->m_renderer->draw(6,0);
		//this->m_renderer->end();

		this->m_renderer->restoreWorld();
		this->m_renderer->popDepthStencilState();

	}

	void DX11Sprite::drawAll()
	{
		/*for(int i=0; i<this->m_numFrames; ++i)
		{
			this->nextFrame();
			this->show();
		}*/
	}

	void DX11Sprite::setVisible(bool visible)
	{
		this->m_visible = visible;
	}

	bool DX11Sprite::getVisible()
	{
		return this->m_visible;
	}


	
	void DX11Sprite::billboard(Matrix *camMatrix)
	{
		this->m_cameraMatrix = camMatrix;
	}

	void DX11Sprite::unbillboard()
	{
		this->m_cameraMatrix = NULL;
	}

	void DX11Sprite::release()
	{
		
		SAFE_DELETE(m_vertexBuffer);
		this->m_texture->release();
		SAFE_DELETE(this->m_texture);
	}

	void DX11Sprite::setNumFrames(int numFrames)
	{
		this->m_numFrames = numFrames;
	}

	void DX11Sprite::setFrameType(FRAME_TYPE frameType)
	{
		this->m_frameType = frameType;
	}

	void DX11Sprite::setCurrentFrame(int frame)
	{
		this->m_curFrame = frame;
	}

	int DX11Sprite::getCurrentFrame()
	{
		return this->m_curFrame;
	}

	int DX11Sprite::getNumFrames()
	{
		return this->m_numFrames;
	}

	void DX11Sprite::nextFrame()
	{
		//++this->m_curFrame;
		this->m_curFrame = (++this->m_curFrame)%(this->m_numFrames);

		this->m_startUV.x = this->m_deltaUV.x*(this->m_curFrame%this->m_maxFramesX);
		this->m_endUV.x   = this->m_startUV.x + this->m_deltaUV.x;

		this->m_startUV.y = this->m_deltaUV.y*(floorf((float)this->m_curFrame/(float)this->m_maxFramesX));
		this->m_endUV.y   = this->m_startUV.y + this->m_deltaUV.y;

		//float newUVy = this->m_deltaUV.y*this->m_curFrame;

		//printf("frame: %d numFrames: %d startX: %f endX: %f startY: %f endY: %f deltaX: %f deltaY: %f\n",this->m_curFrame,this->m_numFrames, this->m_startUV.x, this->m_endUV.x,this->m_startUV.y, 
		//																					this->m_endUV.y,this->m_deltaUV.x,this->m_deltaUV.y);

		Vertex rect[6];
		rect[0].position = Vector3D(0,0,0);
		rect[0].texCoord0 = Vector2D(this->m_startUV.x,this->m_startUV.y);
		rect[1].position = Vector3D(1,0,0);
		rect[1].texCoord0 = Vector2D(this->m_endUV.x,this->m_startUV.y);
		rect[2].position = Vector3D(0,1,0);
		rect[2].texCoord0 = Vector2D(this->m_startUV.x,this->m_endUV.y);
		rect[3].position = Vector3D(0,1,0);
		rect[3].texCoord0 = Vector2D(this->m_startUV.x,this->m_endUV.y);
		rect[4].position = Vector3D(1,0,0);
		rect[4].texCoord0 = Vector2D(this->m_endUV.x,this->m_startUV.y);
		rect[5].position = Vector3D(1,1,0);
		rect[5].texCoord0 = Vector2D(this->m_endUV.x,this->m_endUV.y);

		Vertex *verts = NULL;
		verts = (Vertex*)this->m_vertexBuffer->map(MT_WRITE_DISCARD);
		if(verts!=NULL)
			memcpy(verts,rect,sizeof(Vertex)*6);
		this->m_vertexBuffer->unmap();

		//TODO: update texture coordinates
	}

	void DX11Sprite::resetTransform()
	{
		D3DXMatrixIdentity(&this->m_world);
	}

	void DX11Sprite::transform(Matrix *mat)
	{
		this->m_world *= *mat;
	}

	void DX11Sprite::translate(float x, float y, float z)
	{
		Matrix translation;
		D3DXMatrixTranslation(&translation, x, y, z);
		this->m_world *= translation;
	}

	void DX11Sprite::scale(float x, float y, float z)
	{
		Matrix scaling;
		D3DXMatrixScaling(&scaling, x, y, z);
		this->m_world *= scaling;
	}

	void DX11Sprite::prevFrame()
	{
		if(this->m_curFrame>0)
			this->m_curFrame = (this->m_curFrame-1)%this->m_numFrames;
		else
			this->m_curFrame = this->m_numFrames;
	}
}