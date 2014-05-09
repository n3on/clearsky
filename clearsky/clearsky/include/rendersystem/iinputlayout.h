/*
*
*/
#pragma once
#ifndef _CLEARSKY_I_INPUT_LAYOUT_H_
#define _CLEARSKY_I_INPUT_LAYOUT_H_

#include "rendersystem/ieffect.h"
#include "rendersystem/vertexformats.h"

namespace clearsky
{
	#define NUM_VERTEX_FORMATS 3


	class CLEARSKY_API IInputLayout
	{
		public:
			virtual void release()=0;

			virtual RETURN_VALUE create(VERTEX_FORMAT vertexFormat, IEffect *effect, int techNr=0)=0;
			virtual int  getNumElements()=0;
			//virtual D3D10_INPUT_ELEMENT_DESC* getElementDesc()=0;
			//virtual void setEffect(IEffect *effect)=0;
			//bind the effect
			virtual RETURN_VALUE activate()=0;
			
	};
}

#endif