/*
*
*/
#pragma once
#ifndef _CLEARSKY_DX11_INPUT_LAYOUT_H_
#define _CLEARSKY_DX11_INPUT_LAYOUT_H_

//#include <d3d11.h>
#include "core/utils.h"
#include "rendersystem/iinputlayout.h"
#include "rendersystem/directx11/dx11effect.h"

namespace clearsky
{

	class CLEARSKY_API DX11InputLayout : public IInputLayout
	{
		public:
			DX11InputLayout(ID3D11DeviceContext *immediateContext);
			~DX11InputLayout();
			void release();

			RETURN_VALUE create(VERTEX_FORMAT vertexFormat, IEffect *effect, int techNr=0);
			int  getNumElements();
			D3D11_INPUT_ELEMENT_DESC* getElementDesc();
			//void setEffect(IEffect *effect);
			//void setEffect(DX11Effect *effect);
			RETURN_VALUE activate();
			
		private:
			D3D11_INPUT_ELEMENT_DESC *m_elementDesc;
			VERTEX_FORMAT m_vertexFormat;
			DX11Effect *m_effect;
			ID3D11InputLayout *m_inputLayout;
			int m_numElements;
	
			void initDXStruct(VERTEX_FORMAT vertexFormat);
			void setVertexFormat(VERTEX_FORMAT vf);
			int getVertexFormat();
			RETURN_VALUE setNumElements(int numElements);

			ID3D11DeviceContext *m_immediateContext;
			ID3D11Device *m_dxdevice;
			
	};
}

#endif