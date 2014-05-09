/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/statedesc.h"

namespace clearsky
{
		RasterizerState::RasterizerState()
		{
			this->fillMode  = FILL_MODE_SOLID;
			this->cullMode  = CULL_MODE_BACK;
			this->frontCCW  = false;
			this->depthBias = 0;
			this->slopeScaledDepthBias = 0.0f;
			this->depthBiasClamp = 0.0f;
			this->depthClip = true;
			this->scissor = false;
			this->multisampling = false;
			this->antialisedLine = false;
		}

		RasterizerState::~RasterizerState()
		{

		}

		BlendState::BlendState()
		{

		}

		BlendState::~BlendState()
		{

		}

		DepthStencilFuncDesc::DepthStencilFuncDesc()
		{
			this->stencilFunc = COMPARISON_FUNC_ALWAYS;
			this->stencilDepthFailFunc = STENCIL_FUNC_KEEP;
			this->stencilPassFunc = STENCIL_FUNC_KEEP;
			this->stencilFailFunc = STENCIL_FUNC_KEEP;
		}

		DepthStencilFuncDesc::~DepthStencilFuncDesc()
		{
		}

		DepthStencilState::DepthStencilState()
		{
			this->depthEnable     = true;
			this->depthWriteMask  = DEPTH_WRITE_MASK_ALL;
			this->depthFunc       = COMPARISON_FUNC_LESS;
			this->stencilEnable   = false;
			this->stencilReadMask  = 0xff; 
			this->stencilWriteMask = 0xff;
			

		}

		DepthStencilState::~DepthStencilState()
		{

		}

		SamplerState::SamplerState()
		{

		}

		SamplerState::~SamplerState()
		{

		}

		BlendRenderTarget::BlendRenderTarget()
		{

		}

		BlendRenderTarget::~BlendRenderTarget()
		{

		}

}
