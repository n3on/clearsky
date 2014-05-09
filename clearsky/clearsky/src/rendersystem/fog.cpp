/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/fog.h"

namespace clearsky
{
	Fog::Fog()
	{
		this->m_properties = new FogProperty;

		this->m_properties->fogType = FT_LINEAR;
		this->m_properties->color = Color(1,1,1,1);
		this->m_properties->start = 0;
		this->m_properties->end   = 1000;
		this->m_properties->density = 1;
	}

	Fog::~Fog()
	{
		SAFE_DELETE(this->m_properties);
	}

	FOG_TYPE Fog::setType(FOG_TYPE newType)
	{
		FOG_TYPE oldType = (FOG_TYPE)this->m_properties->fogType;
		if(newType!=oldType)
		{
			this->m_properties->fogType = newType;
		}

		return oldType;
	}

	FOG_TYPE Fog::getType()
	{
		return (FOG_TYPE)this->m_properties->fogType;
	}

	FogProperty *Fog::getProperty()
	{
		return this->m_properties;
	}

	void Fog::setProperty(FogProperty *newProperty)
	{
		SAFE_DELETE(this->m_properties);
		this->m_properties = newProperty;
	}

	Color Fog::setColor(Color newColor)
	{
		Color oldColor = this->m_properties->color;
		this->m_properties->color = newColor;
		
		return oldColor;
	}

	Color Fog::getColor()
	{
		return this->m_properties->color;
	}

	void Fog::setDimensions(float start, float end)
	{
		this->m_properties->start = start;
		this->m_properties->end   = end;
	}

	float Fog::getStart()
	{
		return this->m_properties->start;
	}

	float Fog::getEnd()
	{
		return this->m_properties->end;
	}

	float Fog::setDensity(float newDensity)
	{
		float oldDensity = this->m_properties->density;
		this->m_properties->density = newDensity;

		return oldDensity;
	}

	float Fog::getDensity()
	{
		return this->m_properties->density;
	}

}