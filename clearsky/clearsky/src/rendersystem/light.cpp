/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/light.h"

namespace clearsky
{
	Light::Light()
	{
		this->m_property = new LightProperty;

		this->m_property->pos		= Vector3D(0,0,0);
		this->m_property->dir		= Vector3D(0,0,0);
		this->m_property->ambient	= Color(0,0,0,1);
		this->m_property->diffuse	= Color(0,0,0,1);
		this->m_property->specular	= Color(0,0,0,1);
		//this->m_property->att		= Vector3D(0,0,0);
		this->m_property->att0 = 0;
		this->m_property->att1 = 0;
		this->m_property->att2 = 0;
		this->m_property->spotPower = 0;
		this->m_property->range		= 0;
		this->m_property->falloff	= 0;
		this->m_property->active    = true;
	}

	Light::~Light()
	{
		SAFE_DELETE(this->m_property);
	}

	LightProperty* Light::getProperty()
	{
		return this->m_property;
	}

	void Light::setProperty(LightProperty *newProperty)
	{
		this->m_property = newProperty;
	}

	LIGHT_TYPE Light::getType()
	{
		return (LIGHT_TYPE)this->m_property->lightType;
	}

	bool Light::isActive() const
	{
		return this->m_property->active;
	}

	void Light::setActive(bool active)
	{
		this->m_property->active = active;
	}

	void Light::setDirection(Vector3D direction)
	{
		this->m_property->dir = direction;
	}

	Vector3D Light::getDirection()
	{
		return this->m_property->dir;
	}

	void Light::setAtt(float att0, float att1, float att2)
	{
		this->m_property->att0 = att0;
		this->m_property->att1 = att1;
		this->m_property->att2 = att2;
	}

	PointLight::PointLight()
	{
		Light();
		this->m_property->lightType = (int)LIGHT_POINT;
	}

	PointLight::~PointLight()
	{

	}

	void PointLight::create(Vector3D pos, Vector3D dir, Color ambient, Color diffuse, Color specular,  float range)
	{
		this->m_property->pos		= pos;
		this->m_property->dir		= dir;
		this->m_property->ambient	= ambient;
		this->m_property->diffuse	= diffuse;
		this->m_property->specular	= specular;
		this->m_property->range		= range;
	}

	void PointLight::setPosition(Vector3D newPos)
	{
		this->m_property->pos = newPos;
	}

	DirLight::DirLight()
	{
		Light();
		this->m_property->lightType = (int)LIGHT_DIR;
	}

	DirLight::~DirLight()
	{

	}

	void DirLight::create(Vector3D dir, Color ambient, Color diffuse, Color specular)
	{
		this->m_property->dir		= dir;
		this->m_property->ambient	= ambient;
		this->m_property->diffuse	= diffuse;
		this->m_property->specular	= specular;
	}

	SpotLight::SpotLight()
	{
		Light();
		this->m_property->lightType = (int)LIGHT_SPOT;
	}

	SpotLight::~SpotLight()
	{

	}

	void SpotLight::create(Vector3D pos, Vector3D dir, Color ambient, Color diffuse, Color specular, float power, float range)
	{
		this->m_property->pos		= pos;
		this->m_property->dir       = dir;
		this->m_property->ambient	= ambient;
		this->m_property->diffuse	= diffuse;
		this->m_property->specular	= specular;
		this->m_property->spotPower = power;
		this->m_property->range		= range;
	}

	void SpotLight::setPosition(Vector3D newPos)
	{
		this->m_property->pos = newPos;
	}

	void SpotLight::setAtt(float att0, float att1, float att2)
	{
		this->m_property->att0 = att0;
		this->m_property->att1 = att1;
		this->m_property->att2 = att2;
	}

}
