/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/directX11/dX11effect.h"

namespace clearsky
{
	DX11Effect::DX11Effect(ID3D11Device *device, ID3D11DeviceContext *immediateContext):m_effect(NULL),
												m_effectBuffer(NULL),
												m_dxdevice(NULL)
	{
		this->m_immediateContext = immediateContext;
		this->m_dxdevice = device;
	}

	DX11Effect::~DX11Effect()
	{
		this->release();
	}

	void DX11Effect::release()
	{
		SAFE_RELEASE(this->m_effect);
		SAFE_RELEASE(this->m_effectBuffer);
	}

	RETURN_VALUE DX11Effect::create(const char* filename)
	{
		if(!this->m_dxdevice)
			return RETURN_INVALIDE_DEVICE;


		//try the highest fx version first
		if(this->buildEffect(filename, "fx_5_0")!=RETURN_OK)
		{
			if(this->buildEffect(filename, "fx_4_1")!=RETURN_OK)
			{
				if(this->buildEffect(filename, "fx_4_0")!=RETURN_OK)
					return RETURN_EFFECT_ERROR;
			}
		}

		//if(this->buildEffect(filename, "fx_2_0")!=RETURN_OK)
		//			return RETURN_EFFECT_ERROR;

		return RETURN_OK;
	}

	RETURN_VALUE DX11Effect::buildEffect(const char* filename, const char* fxVersion)
	{
		HRESULT result;

		UINT  shaderFlags = 0;//D3D10_SHADER_ENABLE_STRICTNESS;

		#ifdef _DEBUG
			shaderFlags = shaderFlags | D3D10_SHADER_DEBUG;
		#endif


		//first compile the effect file
		ID3D10Blob *errorBuffer = NULL;
		result = D3DX11CompileFromFile(toMultibyteStr(filename), NULL,
										NULL, NULL,
										fxVersion,
										shaderFlags,
										NULL,
										NULL,
										&this->m_effectBuffer,
										&errorBuffer,
										NULL);
		
		#ifdef _DEBUG
		if(errorBuffer!=NULL)
			printf("effect output: %s\n",errorBuffer->GetBufferPointer());
		#endif

		if(errorBuffer!=NULL)
			errorBuffer->Release();

		if(FAILED(result))
		{
			return RETURN_COMPILE_EFFECT_ERROR;
		}


		//now load it
		result = D3DX11CreateEffectFromMemory(this->m_effectBuffer->GetBufferPointer(),
												this->m_effectBuffer->GetBufferSize(),
												0,
												this->m_dxdevice,
												&this->m_effect);

		if(FAILED(result))
			return RETURN_EFFECT_ERROR;

		return RETURN_OK;
	}

	ID3DX11Effect* DX11Effect::getEffect()
	{
		return this->m_effect;
	}

	ID3DX11EffectTechnique* DX11Effect::getTech(const char* name)
	{
		if(!this->m_effect)
			return NULL;

		return this->m_effect->GetTechniqueByName(name);
	}

	ID3D10Blob* DX11Effect::getEffectBuffer()
	{
		return this->m_effectBuffer;
	}

	ID3DX11EffectTechnique* DX11Effect::getTech(int nr)
	{
		if(!this->m_effect)
			return NULL;

		return this->m_effect->GetTechniqueByIndex(nr);
	}

	void DX11Effect::apply(int technr, int pass)
	{
		
		this->m_effect->GetTechniqueByIndex(technr)->GetPassByIndex(pass)->Apply(0, this->m_immediateContext);
	}

	void DX11Effect::apply(const char* name, int pass)
	{
		this->m_effect->GetTechniqueByName(name)->GetPassByIndex(pass)->Apply(0, this->m_immediateContext);
	}

	int DX11Effect::getNumPasses(const char* effectName)
	{
		if(!this->m_effect)
			return -1;

		D3DX11_TECHNIQUE_DESC desc;
		this->getTech(effectName)->GetDesc(&desc);

		return desc.Passes;
	}

	int DX11Effect::getNumPasses(int nr)
	{
		if(!this->m_effect)
			return -1;

		D3DX11_TECHNIQUE_DESC desc;
		this->getTech(nr)->GetDesc(&desc);

		return desc.Passes;
	}

	void DX11Effect::setInt(const char* name, int value)
	{
		ID3DX11EffectScalarVariable* intVar = this->m_effect->GetVariableByName(name)->AsScalar();
		intVar->SetInt(value);
	}

	int DX11Effect::getInt(const char* name)
	{
		int value;
		ID3DX11EffectScalarVariable* intVar = this->m_effect->GetVariableByName(name)->AsScalar();
		intVar->GetInt(&value);

		return value;
	}

	void DX11Effect::setFloat(const char* name, float value)
	{
		ID3DX11EffectScalarVariable* intVar = this->m_effect->GetVariableByName(name)->AsScalar();
		intVar->SetFloat(value);
	}

	float DX11Effect::getFloat(const char* name)
	{
		float value;
		ID3DX11EffectScalarVariable* intVar = this->m_effect->GetVariableByName(name)->AsScalar();
		intVar->GetFloat(&value);

		return value;
	}


	void DX11Effect::setBool(const char* name, bool value)
	{
		ID3DX11EffectScalarVariable* boolVar = this->m_effect->GetVariableByName(name)->AsScalar();
		boolVar->SetBool(value);
	}

	bool DX11Effect::getBool(const char* name)
	{
		BOOL value = FALSE;

		ID3DX11EffectScalarVariable* boolVar = this->m_effect->GetVariableByName(name)->AsScalar();
		boolVar->GetBool(&value);

		return value!=0;
	}

	void DX11Effect::setMatrix(const char* name, Matrix value)
	{
		ID3DX11EffectMatrixVariable* matrix = this->m_effect->GetVariableByName(name)->AsMatrix();
		matrix->SetMatrix((float*)&value);
	}

	Matrix DX11Effect::getMatrix(const char* name)
	{
		Matrix value;
		ID3DX11EffectMatrixVariable* matrix = this->m_effect->GetVariableByName(name)->AsMatrix();

		matrix->GetMatrix((float*)&value);

		return value;
	}

	void DX11Effect::setData(const char* name, void *data, int size,  int offset)
	{
		ID3DX11EffectVariable* dataVar = this->m_effect->GetVariableByName(name);
		dataVar->SetRawValue(data,offset,size);

	}

	void DX11Effect::setTexture(const char* name, ITexture2D *texture)
	{
		DX11Texture2D *dxTexture = NULL;
		dxTexture = (DX11Texture2D*)texture;
		if(!dxTexture)
			return;

		ID3DX11EffectShaderResourceVariable *effectTex = this->m_effect->GetVariableByName(name)->AsShaderResource();
		if(!effectTex)
			return;

		ID3D11ShaderResourceView *shaderView = dxTexture->getShaderView();
		if(!shaderView)
			return;

		effectTex->SetResource(shaderView);
	}
}