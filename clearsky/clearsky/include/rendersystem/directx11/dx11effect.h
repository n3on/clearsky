/*
*
*/
#pragma once
#ifndef _CLEARSKY_DX11_EFFECT_H_
#define _CLEARSKY_DX11_EFFECT_H_

//#include <d3d10.h>
//#include <d3dX11.h>
#include "d3dx11effect.h"
#include "core/utils.h"
#include "core/globals.h"
#include "math/mathutils.h"
#include "rendersystem/ieffect.h"
#include "rendersystem/directx11/dx11texture2D.h"

namespace clearsky
{
	class CLEARSKY_API DX11Effect : public IEffect
	{
		public:
			DX11Effect(ID3D11Device *device, ID3D11DeviceContext *immediateContext);
			~DX11Effect();
			void release();

			RETURN_VALUE create(const char* filename);
			ID3DX11EffectTechnique* getTech(const char* name);
			ID3DX11EffectTechnique* getTech(int nr);

			void apply(int technr, int pass);
			void apply(const char* name, int pass);
			
			int getNumPasses(const char* effectName);
			int getNumPasses(int nr);

			void setInt(const char* name, int value);
			int getInt(const char* name);
			void setFloat(const char* name, float value);
			float getFloat(const char* name);
			void setBool(const char* name, bool value);
			bool getBool(const char* name);
			void setMatrix(const char* name, Matrix value);
			Matrix getMatrix(const char* name);
			void setData(const char* name, void *data, int size, int offset=0);

			void setTexture(const char* name, ITexture2D *texture);

			ID3DX11Effect* getEffect();
			ID3D10Blob* getEffectBuffer();

		private:
			RETURN_VALUE buildEffect(const char* filename, const char* fxVersion);
			ID3DX11Effect *m_effect;
			ID3D10Blob *m_effectBuffer;
			ID3D11Device *m_dxdevice;
			ID3D11DeviceContext *m_immediateContext;

	};
}

#endif