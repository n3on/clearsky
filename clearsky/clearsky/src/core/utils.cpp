/*
*
*/

#include "core/precompiled.h"
#include "core/utils.h"

namespace clearsky
{
	//TODO: move to std::map
	ErrorText ReturnMessages[] =
	{
		{RETURN_OK, "no error"},
		{RETURN_ERROR, "error occured"},
		{RETURN_INVALIDE_DEVICE, "failed create directx device"},
		{RETURN_INVALIDE_RENDERER, "failed create renderer"},
		{RETURN_EFFECT_ERROR, "effect file was not loaded"},
		{RETURN_INPUT_FORMAT_ERROR, "vertex input format"},
		{RETURN_FREETYPE_ERROR, "freetype error"},
		{RETURN_COMPILE_EFFECT_ERROR, "compiling directx effect file failed"},
		{RETURN_RENDER_TARGET_VIEW_ERROR, "could not create render target view"},
		{RETURN_GET_BUFFER_ERROR, "could not return a buffer from ressource"},
		{RETURN_CREATE_TEXTURE_ERROR, "creating texture failed"},
		{RETURN_DEPTH_STENCIL_VIEW_ERROR, "failed creating depthstencil view"},
		{RETURN_CREATE_RASTERIZER_STATE_ERROR, "failed creating rasterizer state"},
		{RETURN_CREATE_BLEND_STATE_ERROR, "failed creating blend state"},
		{RETURN_CREATE_SHADER_RESOURCE_ERROR, "failed creating shader resource"},
		{RETURN_INVALID_WINDOW_HANDLE, "failed creating window handle"}
	};

	const char* getReturnMessage(RETURN_VALUE value)
	{
		for(int i=0; i<NUM_RETURN_VALUES; i++)
		{
			if(ReturnMessages[i].returnValue == value)
				return ReturnMessages[i].msg;
		}

		return "";
	}

}