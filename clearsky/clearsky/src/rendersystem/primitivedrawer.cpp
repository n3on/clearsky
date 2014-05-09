/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/primitivedrawer.h"

namespace clearsky
{
	PrimitiveDrawer::PrimitiveDrawer():m_renderer(NULL),
									   m_lineVertexBuffer(NULL),
									   m_maxLineVerts(50),
									   m_filledRVerts(NULL),
									   m_maxFilledRVerts(50)

	{
	}

	RETURN_VALUE PrimitiveDrawer::init(const unsigned int initSize)
	{
		this->m_renderer = clearsky::GLOBALS->getRenderer();

		if(!this->m_renderer)
			return RETURN_ERROR;
		
		this->m_maxLineVerts = initSize;

		this->m_lineVertexBuffer = this->m_renderer->createBuffer();
		this->m_lineVertexBuffer->setCPUUsage(BCU_WRITE);
		this->m_lineVertexBuffer->setBufferUsage(BU_DYNAMIC);
		clearsky::Vertex *initData = new clearsky::Vertex[this->m_maxLineVerts];
		if(this->m_lineVertexBuffer->create(initData,sizeof(Vertex),50)!=RETURN_OK)
			RETURN_ERROR;


		this->m_maxFilledRVerts = initSize;

		this->m_filledRVertexBuffer = this->m_renderer->createBuffer();
		this->m_filledRVertexBuffer->setCPUUsage(BCU_WRITE);
		this->m_filledRVertexBuffer->setBufferUsage(BU_DYNAMIC);
		if(this->m_filledRVertexBuffer->create(initData,sizeof(Vertex),50)!=RETURN_OK)
			RETURN_ERROR;

		SAFE_ARRAY_DELETE(initData);
		//TODO: delete initData

		return RETURN_OK;
	}

	PrimitiveDrawer::~PrimitiveDrawer()
	{
		//TODO: delete vertex buffer
	}

	void PrimitiveDrawer::addLine(const Vector3D start, const Vector3D end, const Color color)
	{
		if(this->m_lineVerts.size()>=this->m_maxLineVerts)
			return;


		VertexPC newLineStart;
		newLineStart.position = start;
		newLineStart.color    = color;
		this->m_lineVerts.push_back(newLineStart);

		VertexPC newLineEnd;
		newLineEnd.position = end;
		newLineEnd.color    = color;
		this->m_lineVerts.push_back(newLineEnd);
	}

	void PrimitiveDrawer::commitLineBuffer()
	{
		VertexPC *verts = (VertexPC*)this->m_lineVertexBuffer->map(MT_WRITE_DISCARD);

		for(unsigned int i=0; i<this->m_lineVerts.size(); ++i)
		{
			verts[i].position = this->m_lineVerts[i].position;
			verts[i].color = this->m_lineVerts[i].color;
		}
		
		this->m_lineVertexBuffer->unmap();
	}

	void PrimitiveDrawer::addFilledRect(const Vector3D position, const int width, const int height, const Color color)
	{
		if(this->m_filledRVerts.size()>=this->m_maxFilledRVerts)
			return;

		VertexPC rectVertex1;
		VertexPC rectVertex2;
		VertexPC rectVertex3;
		VertexPC rectVertex4;

		rectVertex1.position = position;
		rectVertex1.color    = color;
		this->m_filledRVerts.push_back(rectVertex1);

		rectVertex2.position = position;
		rectVertex2.color    = color;
		this->m_filledRVerts.push_back(rectVertex2);

		rectVertex3.position = position;
		rectVertex3.color    = color;
		this->m_filledRVerts.push_back(rectVertex3);

		rectVertex4.position = position;
		rectVertex4.color    = color;
		this->m_filledRVerts.push_back(rectVertex4);

	}

	void PrimitiveDrawer::commitFilledRectBuffer()
	{
		VertexPC *verts = (VertexPC*)this->m_filledRVertexBuffer->map(MT_WRITE_DISCARD);

		for(unsigned int i=0; i<this->m_filledRVerts.size(); ++i)
		{
			verts[i].position = this->m_filledRVerts[i].position;
			verts[i].color = this->m_filledRVerts[i].color;
		}
		
		this->m_filledRVertexBuffer->unmap();
	}

	void PrimitiveDrawer::show()
	{
		this->m_lineVertexBuffer->activate();

		this->m_renderer->setTexturing(false);
		this->m_renderer->apply(0,0);

		//draw lines
		this->m_renderer->setPrimitive(PT_LINELIST);
		this->m_renderer->draw(this->m_lineVerts.size(),0);

		//draw filled rects
		this->m_renderer->setPrimitive(PT_TRIANGLESTRIP);
		this->m_renderer->draw(this->m_filledRVerts.size(),0);
	}
}