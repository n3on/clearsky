/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/directx11/DX11InputLayout.h"

namespace clearsky
{
	DX11InputLayout::DX11InputLayout(ID3D11DeviceContext *immediateContext):m_numElements(3),
								m_immediateContext(NULL),
							   m_elementDesc(NULL),
							   m_effect(NULL)
	{
		this->m_immediateContext = immediateContext;
		this->m_immediateContext->GetDevice(&this->m_dxdevice);
	}

	DX11InputLayout::~DX11InputLayout()
	{
		this->release();
	}

	void DX11InputLayout::release()
	{
		SAFE_DELETE(this->m_elementDesc);
		SAFE_RELEASE(this->m_inputLayout);
		if(this->m_effect)
			this->m_effect->release();
	}

	RETURN_VALUE DX11InputLayout::setNumElements(int numElements)
	{
		if(numElements<0)
			return RETURN_ERROR;

		this->m_numElements = numElements;
		return RETURN_OK;
	}

	int DX11InputLayout::getNumElements()
	{
		return this->m_numElements;
	}

	void DX11InputLayout::setVertexFormat(VERTEX_FORMAT vf)
	{
		this->m_vertexFormat = vf;
	}

	/*void DX11InputLayout::setEffect(IEffect *effect)
	{
		if(effect)
			this->m_effect = (DX11Effect*)effect;
	}*/

	/*void DX11InputLayout::setEffect(DX11Effect *effect)
	{
		this->m_effect = effect;
	}*/

	int DX11InputLayout::getVertexFormat()
	{
		return this->m_vertexFormat;
	}

	RETURN_VALUE DX11InputLayout::create(VERTEX_FORMAT vertexFormat, IEffect *effect, int techNr)
	{
		if(!this->m_dxdevice)
		{
			clearsky::LOG->logMsg(clearsky::LT_ERROR, "InputLayout: Invalid DX device");
			return RETURN_INVALIDE_DEVICE;
		}

		if(!effect)
		{
			clearsky::LOG->logMsg(clearsky::LT_ERROR, "InputLayout: Invalid Effect");
			return RETURN_EFFECT_ERROR;
		}

		this->m_effect = (DX11Effect*)effect;

		this->initDXStruct(vertexFormat);

		ID3DX11EffectTechnique *tech = this->m_effect->getTech(techNr);
		D3DX11_PASS_DESC passDesc;
		HRESULT result = tech->GetPassByIndex(0)->GetDesc(&passDesc);

		if(FAILED(result))
		{
			clearsky::LOG->logMsg(clearsky::LT_ERROR, "InputLayout: Can't get effect pass descriptor");
			return RETURN_EFFECT_ERROR;
		}

		ID3DX11EffectPass *effectPass = tech->GetPassByIndex(0);//this->m_effect->getTech(0)->GetPassByIndex(0);
		if(!effectPass)
		{
			clearsky::LOG->logMsg(clearsky::LT_ERROR, "InputLayout: Can't get effect pass");
			return RETURN_EFFECT_ERROR;
		}

		D3DX11_PASS_SHADER_DESC vsDesc;
		memset(&vsDesc,0,sizeof(D3DX11_PASS_SHADER_DESC));
		result = effectPass->GetVertexShaderDesc(&vsDesc);

		if(!vsDesc.pShaderVariable || FAILED(result))
		{
			clearsky::LOG->logMsg(clearsky::LT_ERROR, "InputLayout: Can't get effect shader descriptor");
			return RETURN_EFFECT_ERROR;
		}

		D3DX11_EFFECT_SHADER_DESC vsDesc2;
		memset(&vsDesc2,0,sizeof(D3DX11_EFFECT_SHADER_DESC));
		result = vsDesc.pShaderVariable->GetShaderDesc(vsDesc.ShaderIndex, &vsDesc2);

		if(FAILED(result))
		{
			clearsky::LOG->logMsg(clearsky::LT_ERROR, "InputLayout: Can't get effect pass descriptor");
			return RETURN_EFFECT_ERROR;
		}

		const void *vsCode = vsDesc2.pBytecode;
		unsigned vsLEngth = vsDesc2.BytecodeLength;

		result = this->m_dxdevice->CreateInputLayout(this->m_elementDesc, this->m_numElements,
												vsCode, vsLEngth, &this->m_inputLayout);
		if(result!=S_OK)
		{
				clearsky::LOG->logMsg(clearsky::LT_ERROR, "InputLayout: Can't create inputlayout. DX ErrorCode: %x",result);
				return RETURN_INPUT_FORMAT_ERROR;
		}

		return RETURN_OK;
	}

	void DX11InputLayout::initDXStruct(VERTEX_FORMAT vertexFormat)
	{
		//now create the default vertexFormats
		//TODO: use Reflection interface of the shader to compute this information at the runtime
		switch(vertexFormat)
		{
			case VF_DEFAULT:
			{
				this->m_numElements = 5;
				this->m_elementDesc = new D3D11_INPUT_ELEMENT_DESC[this->m_numElements];

				this->m_elementDesc[0].SemanticName  = "POSITION";
				this->m_elementDesc[0].SemanticIndex = 0;
				this->m_elementDesc[0].Format        = DXGI_FORMAT_R32G32B32_FLOAT;
				this->m_elementDesc[0].InputSlot	 = 0;
				this->m_elementDesc[0].AlignedByteOffset	= 0;
				this->m_elementDesc[0].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
				this->m_elementDesc[0].InstanceDataStepRate	= 0;

				this->m_elementDesc[1].SemanticName  = "NORMAL";
				this->m_elementDesc[1].SemanticIndex = 0;
				this->m_elementDesc[1].Format        = DXGI_FORMAT_R32G32B32_FLOAT;
				this->m_elementDesc[1].InputSlot	 = 0;
				this->m_elementDesc[1].AlignedByteOffset	= 12;
				this->m_elementDesc[1].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
				this->m_elementDesc[1].InstanceDataStepRate	= 0;

				this->m_elementDesc[2].SemanticName  = "TANGENT";
				this->m_elementDesc[2].SemanticIndex = 0;
				this->m_elementDesc[2].Format        = DXGI_FORMAT_R32G32B32_FLOAT;
				this->m_elementDesc[2].InputSlot	 = 0;
				this->m_elementDesc[2].AlignedByteOffset	= 24;
				this->m_elementDesc[2].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
				this->m_elementDesc[2].InstanceDataStepRate	= 0;
				
				this->m_elementDesc[3].SemanticName  = "texCoord";
				this->m_elementDesc[3].SemanticIndex = 0;
				this->m_elementDesc[3].Format        = DXGI_FORMAT_R32G32_FLOAT;
				this->m_elementDesc[3].InputSlot	 = 0;
				this->m_elementDesc[3].AlignedByteOffset	= 36;
				this->m_elementDesc[3].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
				this->m_elementDesc[3].InstanceDataStepRate	= 0;

				this->m_elementDesc[4].SemanticName  = "texCoord";
				this->m_elementDesc[4].SemanticIndex = 1;
				this->m_elementDesc[4].Format        = DXGI_FORMAT_R32G32_FLOAT;
				this->m_elementDesc[4].InputSlot	 = 0;
				this->m_elementDesc[4].AlignedByteOffset	= 44;
				this->m_elementDesc[4].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
				this->m_elementDesc[4].InstanceDataStepRate	= 0;

				/*this->m_elementDesc[6].SemanticName  = "texCoord";
				this->m_elementDesc[6].SemanticIndex = 2;
				this->m_elementDesc[6].Format        = DXGI_FORMAT_R32G32_FLOAT;
				this->m_elementDesc[6].InputSlot	 = 0;
				this->m_elementDesc[6].AlignedByteOffset	= 68;
				this->m_elementDesc[6].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
				this->m_elementDesc[6].InstanceDataStepRate	= 0;*/

			}break;
			case VF_P:
			{
				this->m_numElements = 1;
				this->m_elementDesc = new D3D11_INPUT_ELEMENT_DESC[this->m_numElements];

				this->m_elementDesc[0].SemanticName  = "POSITION";
				this->m_elementDesc[0].SemanticIndex = 0;
				this->m_elementDesc[0].Format        = DXGI_FORMAT_R32G32B32_FLOAT;
				this->m_elementDesc[0].InputSlot	 = 0;
				this->m_elementDesc[0].AlignedByteOffset	= 0;
				this->m_elementDesc[0].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
				this->m_elementDesc[0].InstanceDataStepRate	= 0;
			}break;
			case VF_PC:
			{
				this->m_numElements = 2;
				this->m_elementDesc = new D3D11_INPUT_ELEMENT_DESC[this->m_numElements];

				this->m_elementDesc[0].SemanticName  = "POSITION";
				this->m_elementDesc[0].SemanticIndex = 0;
				this->m_elementDesc[0].Format        = DXGI_FORMAT_R32G32B32_FLOAT;
				this->m_elementDesc[0].InputSlot	 = 0;
				this->m_elementDesc[0].AlignedByteOffset	= 0;
				this->m_elementDesc[0].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
				this->m_elementDesc[0].InstanceDataStepRate	= 0;

				this->m_elementDesc[1].SemanticName  = "COLOR";
				this->m_elementDesc[1].SemanticIndex = 0;
				this->m_elementDesc[1].Format        = DXGI_FORMAT_R32G32B32A32_FLOAT;
				this->m_elementDesc[1].InputSlot	 = 0;
				this->m_elementDesc[1].AlignedByteOffset	= 12;
				this->m_elementDesc[1].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
				this->m_elementDesc[1].InstanceDataStepRate	= 0;

			}break;
			case VF_PCUV:
			{
				this->m_numElements = 3;
				this->m_elementDesc = new D3D11_INPUT_ELEMENT_DESC[this->m_numElements];

				this->m_elementDesc[0].SemanticName  = "POSITION";
				this->m_elementDesc[0].SemanticIndex = 0;
				this->m_elementDesc[0].Format        = DXGI_FORMAT_R32G32B32_FLOAT;
				this->m_elementDesc[0].InputSlot	 = 0;
				this->m_elementDesc[0].AlignedByteOffset	= 0;
				this->m_elementDesc[0].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
				this->m_elementDesc[0].InstanceDataStepRate	= 0;

				this->m_elementDesc[1].SemanticName  = "COLOR";
				this->m_elementDesc[1].SemanticIndex = 0;
				this->m_elementDesc[1].Format        = DXGI_FORMAT_R32G32B32A32_FLOAT;
				this->m_elementDesc[1].InputSlot	 = 0;
				this->m_elementDesc[1].AlignedByteOffset	= 12;
				this->m_elementDesc[1].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
				this->m_elementDesc[1].InstanceDataStepRate	= 0;

				this->m_elementDesc[2].SemanticName  = "texCoord";
				this->m_elementDesc[2].SemanticIndex = 0;
				this->m_elementDesc[2].Format        = DXGI_FORMAT_R32G32_FLOAT;
				this->m_elementDesc[2].InputSlot	 = 0;
				this->m_elementDesc[2].AlignedByteOffset	= 28;
				this->m_elementDesc[2].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
				this->m_elementDesc[2].InstanceDataStepRate	= 0;

			}break;
		}
	}

	RETURN_VALUE DX11InputLayout::activate()
	{
		if(!this->m_dxdevice || !this->m_immediateContext)
			return RETURN_ERROR;

		if(!this->m_inputLayout)
			return RETURN_ERROR;


		this->m_immediateContext->IASetInputLayout(this->m_inputLayout);
		

		return RETURN_OK;
	}

	D3D11_INPUT_ELEMENT_DESC* DX11InputLayout::getElementDesc()
	{
		return this->m_elementDesc;
	}

}