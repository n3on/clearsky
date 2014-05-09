/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/box.h"

namespace clearsky
{
	Box::Box():m_vertexBuffer(NULL),
			   m_indexBuffer(NULL),
			   m_renderer(NULL),
			   m_texture(NULL)
	{
		

	}

	RETURN_VALUE Box::create()
	{
		m_renderer = GLOBALS->getRenderer();
		this->m_texture = this->m_renderer->createTexture2D();

		if(!this->m_renderer)
			return RETURN_ERROR;

		this->m_vertexBuffer = this->m_renderer->createBuffer();
		this->m_indexBuffer  =  this->m_renderer->createBuffer();

		Vertex vertices[8];

		vertices[0].position = Vector3D(-1.0f, -1.0f, -1.0f);
		vertices[0].texCoord0 = Vector2D(0,1);
		vertices[0].normal = Vector3D(-1.0f, -1.0f, -1.0f);
		//vertices[0].color    = Color(1.0f,1.0f,1.0f,1.0f);
		//vertices[0].diffuse    = Color(1,1,1,1);
		vertices[1].position = Vector3D(-1.0f, +1.0f, -1.0f);
		vertices[1].texCoord0 = Vector2D(0,0);
		vertices[1].normal = Vector3D(-1.0f, 1.0f, -1.0f);
		//vertices[1].color    = Color(1.0f,1.0f,1.0f,1.0f);
		//vertices[1].diffuse    = Color(1,1,1,1);
		vertices[2].position = Vector3D(+1.0f, +1.0f, -1.0f);
		vertices[2].texCoord0 = Vector2D(1,0);
		vertices[2].normal = Vector3D(1.0f, 1.0f, -1.0f);
		//vertices[2].color    = Color(1.0f,1.0f,1.0f,1.0f);
		//vertices[2].diffuse    = Color(1,1,1,1);
		vertices[3].position = Vector3D(+1.0f, -1.0f, -1.0f);
		vertices[3].texCoord0 = Vector2D(1,1);
		vertices[3].normal = Vector3D(1.0f, -1.0f, -1.0f);
		//vertices[3].color    = Color(1.0f,1.0f,1.0f,1.0f);
		//vertices[3].diffuse    = Color(1,1,1,1);
		vertices[4].position = Vector3D(-1.0f, -1.0f, +1.0f);
		vertices[4].texCoord0 = Vector2D(1,1);
		vertices[4].normal = Vector3D(-1.0f, -1.0f, 1.0f);
		//vertices[4].color    = Color(1.0f,1.0f,1.0f,1.0f);
		//vertices[4].diffuse    = Color(1,1,1,1);
		vertices[5].position = Vector3D(-1.0f, +1.0f, +1.0f);
		vertices[5].texCoord0 = Vector2D(1,0);
		vertices[5].normal = Vector3D(-1.0f, 1.0f, 1.0f);
		//vertices[5].color    = Color(1.0f,1.0f,1.0f,1.0f);
		//vertices[5].diffuse    = Color(1,1,1,1);
		vertices[6].position = Vector3D(+1.0f, +1.0f, +1.0f);
		vertices[6].texCoord0 = Vector2D(0,0);
		vertices[6].normal = Vector3D(1.0f, 1.0f, 1.0f);
		//vertices[6].color    = Color(1.0f,1.0f,1.0f,1.0f);
		//vertices[6].diffuse    = Color(1,1,1,1);
		vertices[7].position = Vector3D(+1.0f, -1.0f, +1.0f);
		vertices[7].texCoord0 = Vector2D(0,1);
		vertices[7].normal = Vector3D(1.0f, -1.0f, 1.0f);
		//vertices[7].color    = Color(1.0f,1.0f,1.0f,1.0f);
		//vertices[7].diffuse    = Color(1,1,1,1);

		RETURN_VALUE result = this->m_vertexBuffer->create(vertices, sizeof(Vertex),8);
		if(result!=RETURN_OK)
			return result;

		DWORD indices[] = 
		{
			// front face
			0, 1, 2,
			0, 2, 3,

			// back face
			4, 6, 5,
			4, 7, 6,

			// left face
			4, 5, 1,
			4, 1, 0,

			// right face
			3, 2, 6,
			3, 6, 7,

			// top face
			1, 5, 6,
			1, 6, 2,

			// bottom face
			4, 0, 3,
			4, 3, 7
		};

		result = this->m_indexBuffer->create(indices, sizeof(DWORD), 12*3, clearsky::BT_INDEX);
		if(result!=RETURN_OK)
			return result;

		return RETURN_OK;
	}

	Box::~Box()
	{
		this->release();
	}

	void Box::setTexture(ITexture2D *texture)
	{
		this->m_texture = texture;
	}

	ITexture2D* Box::getTexture()
	{
		return this->m_texture;
	}

	void Box::draw()
	{
		this->m_vertexBuffer->activate();
		this->m_indexBuffer->activate();


		this->m_renderer->drawIndexed(12*3,0,0);
	}

	void Box::release()
	{
		SAFE_DELETE(this->m_vertexBuffer);
		SAFE_DELETE(this->m_indexBuffer);
		SAFE_DELETE(this->m_texture);
	}

	IBuffer* Box::getVertexBuffer()
	{
		return this->m_vertexBuffer;
	}

	IBuffer* Box::getIndexBuffer()
	{
		return this->m_indexBuffer;
	}

}