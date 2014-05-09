/*
*
*/
#pragma once
#ifndef _CLEARSKY_CLEARSKY_STATEDESC_H_
#define _CLEARSKY_CLEARSKY_STATEDESC_H_

#include "core/utils.h"

namespace clearsky
{
	enum FILL_MODE
	{
		FILL_MODE_WIREFRAME=2,
		FILL_MODE_SOLID=3
	};

	enum CULL_MODE
	{
		CULL_MODE_NONE=1,
		CULL_MODE_FRONT=2,
		CULL_MODE_BACK=3
	};

	enum CLEARSKY_API BLEND_FAC
	{
		BLEND_FAC_ZERO = 1,
		BLEND_FAC_ONE,
		BLEND_FAC_SRC_COLOR,
		BLEND_FAC_INV_SRC_COLOR,
		BLEND_FAC_SRC_ALPHA,
		BLEND_FAC_INV_SRC_ALPHA,
		BLEND_FAC_DEST_ALPHA,
		BLEND_FAC_INV_DEST_ALPHA,
		BLEND_FAC_DEST_COLOR,
		BLEND_FAC_INV_DEST_COLOR,
		BLEND_FAC_SRC_ALPHA_SAT,
		BLEND_FAC_BLEND_FACTOR,
		BLEND_FAC_INV_BLEND_FACTOR,
		BLEND_FAC_SRC1_COLOR,
		BLEND_FAC_INV_SRC1_COLOR,
		BLEND_FAC_SRC1_ALPHA,
		BLEND_FAC_INV_SRC1_ALPHA

	};

	enum CLEARSKY_API BLEND_FUNC
	{
		BLEND_FUNC_ADD = 1,
		BLEND_FUNC_SUBTRACT,
		BLEND_FUNC_REV_SUBTRACT,
		BLEND_FUNC_MIN,
		BLEND_FUNC_MAX
	};

	class CLEARSKY_API BlendRenderTarget
	{
		public:
			BlendRenderTarget();
			~BlendRenderTarget();

		private:
			bool blendEnable;
			BLEND_FAC   srcColFac;
			BLEND_FAC   dstColFac;
			BLEND_FAC   srcAlphaFac;
			BLEND_FAC   dstAlphaFac;
			BLEND_FUNC	colFunc;
			BLEND_FUNC	alphaFunc;
			unsigned int  writeMask;
	};

	class CLEARSKY_API RasterizerState
	{
		public:
			RasterizerState();
			~RasterizerState();

			FILL_MODE	fillMode;
			CULL_MODE	cullMode;
			int		depthBias;
			float	depthBiasClamp;
			float	slopeScaledDepthBias;
			bool	frontCCW;
			bool	depthClip;
			bool	scissor;
			bool	multisampling;
			bool	antialisedLine;

		private:
	};

	class CLEARSKY_API BlendState
	{
		public:
			BlendState();
			~BlendState();

			bool   alphaToCoverage;
            bool   independentBlend;
			BlendRenderTarget RenderTarget[8];
		private:
	};

	enum DEPTH_WRITE_MASK
	{
		DEPTH_WRITE_MASK_ZERO   = 0,
		DEPTH_WRITE_MASK_ALL 
	};

	enum COMPARISON_FUNC
	{
		COMPARISON_FUNC_NEVER = 1,
		COMPARISON_FUNC_LESS,
		COMPARISON_FUNC_EQUAL,
		COMPARISON_FUNC_LESS_EQUAL,
		COMPARISON_FUNC_GREATER,
		COMPARISON_FUNC_NOT_EQUAL,
		COMPARISON_FUNC_GREATER_EQUAL,
		COMPARISON_FUNC_ALWAYS 
	};

	enum STENCIL_FUNC 
	{
		STENCIL_FUNC_KEEP = 1,
		STENCIL_FUNC_ZERO,
		STENCIL_FUNC_REPLACE,
		STENCIL_FUNC_INCR_SAT,
		STENCIL_FUNC_DECR_SAT,
		STENCIL_FUNC_INVERT,
		STENCIL_FUNC_INCR,
		STENCIL_FUNC_DECR 
	};

	class CLEARSKY_API DepthStencilFuncDesc
	{
		public:
			DepthStencilFuncDesc();
			~DepthStencilFuncDesc();
			
			STENCIL_FUNC		stencilFailFunc;
			STENCIL_FUNC		stencilDepthFailFunc;
			STENCIL_FUNC		stencilPassFunc;
			COMPARISON_FUNC		stencilFunc;
		
		private:
	};

	class CLEARSKY_API DepthStencilState
	{
		public:
			DepthStencilState();
			~DepthStencilState();

			 bool                       depthEnable;
			 bool                       stencilEnable;
			 unsigned int               stencilReadMask;
			 unsigned int               stencilWriteMask;
			 DEPTH_WRITE_MASK			depthWriteMask;
			 COMPARISON_FUNC			depthFunc;
			 DepthStencilFuncDesc	frontFace;
			 DepthStencilFuncDesc	backFace;

		private:
	};

	enum FILTER_METHOD
	{
		FM__MIN_MAG_MIP_POINT                            = 0,
		FM__MIN_MAG_POINT_MIP_LINEAR                     = 0x1,
		FM__MIN_POINT_MAG_LINEAR_MIP_POINT               = 0x4,
		FM__MIN_POINT_MAG_MIP_LINEAR                     = 0x5,
		FM__MIN_LINEAR_MAG_MIP_POINT                     = 0x10,
		FM__MIN_LINEAR_MAG_POINT_MIP_LINEAR              = 0x11,
		FM__MIN_MAG_LINEAR_MIP_POINT                     = 0x14,
		FM__MIN_MAG_MIP_LINEAR                           = 0x15,
		FM__ANISOTROPIC                                  = 0x55,
		FM__COMPARISON_MIN_MAG_MIP_POINT                 = 0x80,
		FM__COMPARISON_MIN_MAG_POINT_MIP_LINEAR          = 0x81,
		FM__COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT    = 0x84,
		FM__COMPARISON_MIN_POINT_MAG_MIP_LINEAR          = 0x85,
		FM__COMPARISON_MIN_LINEAR_MAG_MIP_POINT          = 0x90,
		FM__COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR   = 0x91,
		FM__COMPARISON_MIN_MAG_LINEAR_MIP_POINT          = 0x94,
		FM__COMPARISON_MIN_MAG_MIP_LINEAR                = 0x95,
		FM__COMPARISON_ANISOTROPIC                       = 0xd5,
		FM__TEXT_1BIT                                    = 0x80000000 
	};

	enum TEXTURE_ADDRESS_MODE
	{
		TEXTURE_ADDRESS_WRAP = 1,
		TEXTURE_ADDRESS_MIRROR,
		TEXTURE_ADDRESS_CLAMP,
		TEXTURE_ADDRESS_BORDER,
		TEXTURE_ADDRESS_MIRROR_ONCE
	};

	class CLEARSKY_API SamplerState
	{
		public:
			SamplerState();
			~SamplerState();

			FILTER_METHOD        filter;
			TEXTURE_ADDRESS_MODE addressU;
			TEXTURE_ADDRESS_MODE addressV;
			TEXTURE_ADDRESS_MODE addressW;
			float                mipLODBias;
			unsigned int         maxAnisotropy;
			COMPARISON_FUNC		 comparisonFunc;
			float                borderColor[4];
			float                minLOD;
			float                maxLOD;

		private:
	};
}

#endif
