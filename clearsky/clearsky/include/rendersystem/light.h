/*
*
*/
#pragma once
#ifndef _CLEARSKY_LIGHT_H_
#define _CLEARSKY_LIGHT_H_

#include "core/utils.h"
#include "math/mathutils.h"

namespace clearsky
{
	enum LIGHT_TYPE
	{
		LIGHT_DIR  = 0,
		LIGHT_POINT = 1,
		LIGHT_SPOT  = 2
	};

	struct CLEARSKY_API LightProperty
	{
		//Light();
		//~Light();

		Vector3D pos;
		float pad;
		Vector3D dir;
		float pad2;
		Color	 ambient;
		Color	 diffuse;
		Color	 specular;
		float att0;
		float att1;
		float att2;
		float	 spotPower;
		float	 range;
		float	falloff;
		float	theta;
		float	phi;
		int lightType;
		Vector3D pad3;
		bool active;
		Vector3D pad4;
		//pdding variables: make sure the size is a multiple of float4 = 16 bytes ... need for HLSL

	};

	class  CLEARSKY_API Light
	{
		public:
			Light();
			virtual ~Light();

			LightProperty *getProperty();
			void setProperty(LightProperty *newProperty);
			LIGHT_TYPE getType();
			bool isActive() const;
			void setActive(bool active);
			void setDirection(Vector3D direction);
			Vector3D getDirection();

			void setAtt(float att0, float att1, float att2);

		protected:
			LightProperty *m_property;
	};

	class  CLEARSKY_API PointLight : public Light
	{
		public:
			PointLight();
			virtual ~PointLight();

			void create(Vector3D pos, Vector3D dir, Color	ambient, Color diffuse, Color specular,  float range);
			void setPosition(Vector3D newPos);

		private:
	};

	class  CLEARSKY_API DirLight : public Light
	{
		public:
			DirLight();
			virtual ~DirLight();

		void create(Vector3D dir, Color	ambient, Color diffuse, Color specular);

		private:
	};

	class  CLEARSKY_API SpotLight : public Light
	{
		public:
			SpotLight();
			virtual ~SpotLight();
			
			void create(Vector3D pos, Vector3D dir, Color	ambient, Color diffuse, Color specular,  float power, float range);
			void setPosition(Vector3D newPos);

			void setAtt(float att0, float att1, float att2);

		private:
	};
}

#endif