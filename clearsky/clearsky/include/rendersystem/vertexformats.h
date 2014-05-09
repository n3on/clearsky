/*
*
*/

#pragma once
#ifndef _CLEARSKY_VERTEX_FORMAT_H_
#define _CLEARSKY_VERTEX_FORMAT_H_

namespace clearsky
{
	enum CLEARSKY_API VERTEX_FORMAT
	{
		VF_DEFAULT=0,
		VF_P,
		VF_PC,
		VF_PCUV,
		VF_PN2UV
	};

	struct CLEARSKY_API Vertex
	{
		Vertex()
		{
			position	= Vector3D(0,0,0);
			normal		= Vector3D(0,0,0);
			tangent     = Vector3D(0,0,0);
			texCoord0 = Vector2D(0,0);
			texCoord1 = Vector2D(0,0);
		}

		static VERTEX_FORMAT getFormat()
		{
			return VF_DEFAULT;
		}
		
		Vector3D position;
		Vector3D normal;
		Vector3D tangent;
		Vector2D texCoord0;
		Vector2D texCoord1;
	};

	struct CLEARSKY_API VertexPN2UV
	{
		VertexPN2UV()
		{
			position	= Vector3D(0,0,0);
			normal		= Vector3D(0,0,0);
			texCoord0 = Vector2D(0,0);
			texCoord1 = Vector2D(0,0);
		}

		static VERTEX_FORMAT getFormat()
		{
			return VF_PN2UV;
		}
		
		Vector3D position;
		Vector3D normal;
		Vector2D texCoord0;
		Vector2D texCoord1;
	};

	struct CLEARSKY_API VertexP
	{
		VertexP()
		{
			position	= Vector3D(0,0,0);
		}

		static VERTEX_FORMAT getFormat()
		{
			return VF_P;
		}
		
		Vector3D position;
	};

	struct CLEARSKY_API VertexPC
	{
		VertexPC()
		{
			position	= Vector3D(0,0,0);
			color		= Color(1,1,1,1);
		}

		static VERTEX_FORMAT getFormat()
		{
			return VF_PC;
		}
		
		Vector3D position;
		Color color;
	};

	struct CLEARSKY_API VertexPCUV
	{
		VertexPCUV()
		{
			position	= Vector3D(0,0,0);
			color		= Color(1,1,1,1);
			texCoord0 = Vector2D(0,0);
		}

		static VERTEX_FORMAT getFormat()
		{
			return VF_PCUV;
		}
		
		Vector3D position;
		Color color;
		Vector2D texCoord0;
	};

}

#endif