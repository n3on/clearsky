/*
*
*/

#pragma once
#ifndef _CLEARSKY_UTILS_H_
#define _CLEARSKY_UTILS_H_

#ifdef CLEARSKY_EXPORTS
 #define CLEARSKY_API __declspec(dllexport)
#else
 #define CLEARSKY_API __declspec(dllimport)
#endif

#pragma warning(disable : 4251)
#pragma warning(disable : 4100)
#pragma warning(disable : 4820)

#include "core/precompiled.h"

namespace clearsky
{
	//define types
	#define WindowHandler HWND
	#define WindowInstance HINSTANCE

	//renderer macros
	//CLEARSKY_RENDER_DX10 for directx10
	//CLEARSKY_RENDER_DX11 for directx11

	#define ERR_INDEX_OUT_OF_RANGE 10325000

	#define DEFAULT_EFFECT_FILE "..\\data\\shaders\\default.fx"

	#define NULL 0

	enum CLEARSKY_API DATA_FORMAT
	{
		FORMAT_UNKNOWN = 0,
		FORMAT_R32G32B32A32_TYPELESS = 1,
		FORMAT_R32G32B32A32_FLOAT = 2,
		FORMAT_R32G32B32A32_UINT = 3,
		FORMAT_R32G32B32A32_SINT = 4,
		FORMAT_R32G32B32_TYPELESS = 5,
		FORMAT_R32G32B32_FLOAT = 6,
		FORMAT_R32G32B32_UINT = 7,
		FORMAT_R32G32B32_SINT = 8,
		FORMAT_R16G16B16A16_TYPELESS = 9,
		FORMAT_R16G16B16A16_FLOAT = 10,
		FORMAT_R16G16B16A16_UNORM = 11,
		FORMAT_R16G16B16A16_UINT = 12,
		FORMAT_R16G16B16A16_SNORM = 13,
		FORMAT_R16G16B16A16_SINT = 14,
		FORMAT_R32G32_TYPELESS = 15,
		FORMAT_R32G32_FLOAT = 16,
		FORMAT_R32G32_UINT = 17,
		FORMAT_R32G32_SINT = 18,
		FORMAT_R32G8X24_TYPELESS = 19,
		FORMAT_D32_FLOAT_S8X24_UINT = 20,
		FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
		FORMAT_X32_TYPELESS_G8X24_UINT = 22,
		FORMAT_R10G10B10A2_TYPELESS = 23,
		FORMAT_R10G10B10A2_UNORM = 24,
		FORMAT_R10G10B10A2_UINT = 25,
		FORMAT_R11G11B10_FLOAT = 26,
		FORMAT_R8G8B8A8_TYPELESS = 27,
		FORMAT_R8G8B8A8_UNORM = 28,
		FORMAT_R8G8B8A8_UNORM_SRGB = 29,
		FORMAT_R8G8B8A8_UINT = 30,
		FORMAT_R8G8B8A8_SNORM = 31,
		FORMAT_R8G8B8A8_SINT = 32,
		FORMAT_R16G16_TYPELESS = 33,
		FORMAT_R16G16_FLOAT = 34,
		FORMAT_R16G16_UNORM = 35,
		FORMAT_R16G16_UINT = 36,
		FORMAT_R16G16_SNORM = 37,
		FORMAT_R16G16_SINT = 38,
		FORMAT_R32_TYPELESS = 39,
		FORMAT_D32_FLOAT = 40,
		FORMAT_R32_FLOAT = 41,
		FORMAT_R32_UINT = 42,
		FORMAT_R32_SINT = 43,
		FORMAT_R24G8_TYPELESS = 44,
		FORMAT_D24_UNORM_S8_UINT = 45,
		FORMAT_R24_UNORM_X8_TYPELESS = 46,
		FORMAT_X24_TYPELESS_G8_UINT = 47,
		FORMAT_R8G8_TYPELESS = 48,
		FORMAT_R8G8_UNORM = 49,
		FORMAT_R8G8_UINT = 50,
		FORMAT_R8G8_SNORM = 51,
		FORMAT_R8G8_SINT = 52,
		FORMAT_R16_TYPELESS = 53,
		FORMAT_R16_FLOAT = 54,
		FORMAT_D16_UNORM = 55,
		FORMAT_R16_UNORM = 56,
		FORMAT_R16_UINT = 57,
		FORMAT_R16_SNORM = 58,
		FORMAT_R16_SINT = 59,
		FORMAT_R8_TYPELESS = 60,
		FORMAT_R8_UNORM = 61,
		FORMAT_R8_UINT = 62,
		FORMAT_R8_SNORM = 63,
		FORMAT_R8_SINT = 64,
		FORMAT_A8_UNORM = 65,
		FORMAT_R1_UNORM = 66,
		FORMAT_R9G9B9E5_SHAREDEXP = 67,
		FORMAT_R8G8_B8G8_UNORM = 68,
		FORMAT_G8R8_G8B8_UNORM = 69,
		FORMAT_BC1_TYPELESS = 70,
		FORMAT_BC1_UNORM = 71,
		FORMAT_BC1_UNORM_SRGB = 72,
		FORMAT_BC2_TYPELESS = 73,
		FORMAT_BC2_UNORM = 74,
		FORMAT_BC2_UNORM_SRGB = 75,
		FORMAT_BC3_TYPELESS = 76,
		FORMAT_BC3_UNORM = 77,
		FORMAT_BC3_UNORM_SRGB = 78,
		FORMAT_BC4_TYPELESS = 79,
		FORMAT_BC4_UNORM = 80,
		FORMAT_BC4_SNORM = 81,
		FORMAT_BC5_TYPELESS = 82,
		FORMAT_BC5_UNORM = 83,
		FORMAT_BC5_SNORM = 84,
		FORMAT_B5G6R5_UNORM = 85,
		FORMAT_B5G5R5A1_UNORM = 86,
		FORMAT_B8G8R8A8_UNORM = 87,
		FORMAT_B8G8R8X8_UNORM = 88,
		FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
		FORMAT_B8G8R8A8_TYPELESS = 90,
		FORMAT_B8G8R8A8_UNORM_SRGB = 91,
		FORMAT_B8G8R8X8_TYPELESS = 92,
		FORMAT_B8G8R8X8_UNORM_SRGB = 93,
		FORMAT_BC6H_TYPELESS = 94,
		FORMAT_BC6H_UF16 = 95,
		FORMAT_BC6H_SF16 = 96,
		FORMAT_BC7_TYPELESS = 97,
		FORMAT_BC7_UNORM = 98,
		FORMAT_BC7_UNORM_SRGB = 99,
		FORMAT_FORCE_UINT = 0xffffffffUL
	};

	enum CLEARSKY_API BUFFER_TYPE
	{
		BT_VERTEX,
		BT_INDEX
	};

	enum CLEARSKY_API BUFFER_USAGE
	{
		BU_DEFAULT=0,
		BU_IMMUTABLE,
		BU_DYNAMIC,
		BU_STAGING
	};

	enum CLEARSKY_API BUFFER_CPU_USAGE
	{
		BCU_READ = 0x10000L,
		BCU_WRITE = 0x20000L 
	};

	enum CLEARSKY_API MAP_TYPE
	{
		MT_READ                 = 1,
		MT_WRITE                = 2,
		MT_READ_WRITE           = 3,
		MT_WRITE_DISCARD        = 4,
		MT_WRITE_NO_OVERWRITE   = 5 
	};

	//TODO: create struct with description
	enum CLEARSKY_API RETURN_VALUE
	{
		RETURN_OK,
		RETURN_ERROR,
		RETURN_INVALIDE_DEVICE,
		RETURN_INVALIDE_RENDERER,
		RETURN_INVALID_WINDOW_HANDLE,
		RETURN_EFFECT_ERROR,
		RETURN_COMPILE_EFFECT_ERROR,
		RETURN_INPUT_FORMAT_ERROR,
		RETURN_FREETYPE_ERROR,
		RETURN_RENDER_TARGET_VIEW_ERROR,
		RETURN_GET_BUFFER_ERROR,
		RETURN_CREATE_TEXTURE_ERROR,
		RETURN_CREATE_SHADER_RESOURCE_ERROR,
		RETURN_DEPTH_STENCIL_VIEW_ERROR,
		RETURN_CREATE_RASTERIZER_STATE_ERROR,
		RETURN_CREATE_BLEND_STATE_ERROR,
		NUM_RETURN_VALUES
	};

	struct ErrorText
	{
		RETURN_VALUE returnValue;
		const char* msg;
	};

	CLEARSKY_API const char* getReturnMessage(RETURN_VALUE);
	
	template <typename T>
	inline void SAFE_DELETE(T& var)
	{
		if(var)
		{
			delete var;
			var = NULL;
		}
	};

	template <typename T>
	inline void SAFE_ARRAY_DELETE(T& var)
	{
		if(var)
		{
			delete[] var;
			var = NULL;	
		}
	};

	template <typename T>
	inline void SAFE_RELEASE(T& var)
	{
		if(var)
		{
			var->Release();
			var = NULL;
		}
	};



	 template <typename C> class Singleton
	 {
		 public:
			static C* getInstance()
			{
			   if (!m_instance)
				  m_instance = new C();
			   return m_instance;
			}
			virtual ~Singleton ()
			{
			   m_instance = 0;
			}
		 private:
			static C* m_instance;
		 protected:
			Singleton () { }
	 };

	 template <typename C> C* Singleton<C>::m_instance = 0;

	 //converts normal char* string into multibyte one
	 inline wchar_t* toMultibyteStr(const char *string)
	 {
		 if(!string)
			 return NULL;

		 int strSize = MultiByteToWideChar(CP_ACP,0,string ,strlen(string)+1,NULL,0);
		 wchar_t *wcharString = new wchar_t[strSize];
		 MultiByteToWideChar(CP_ACP,0,string,strlen(string)+1,wcharString,strSize);

		 return wcharString;
	 }

	 inline char* toStr(wchar_t *string)
	 {
		 if(!string)
			 return NULL;

		 char *newFilename = new char[wcslen(string) + 1];
		 newFilename[wcslen(string)] = '\0';
		 WideCharToMultiByte(CP_ACP, 0, string, -1, newFilename, (int)wcslen(string), NULL, NULL);

		 return newFilename;
	 }

	 inline unsigned long long rand64()
	 {
        return (static_cast<unsigned long long>(rand())<<32 ) | rand();
	 } 

}

#endif