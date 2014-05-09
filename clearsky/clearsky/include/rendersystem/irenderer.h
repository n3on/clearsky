/*
*
*/


#pragma once
#ifndef _CLEARSKY_I_RENDERER_H_
#define _CLEARSKY_I_RENDERER_H_


#include "rendersystem/ibuffer.h"
#include "rendersystem/ieffect.h"
#include "rendersystem/iinputlayout.h"
#include "rendersystem/itexture2D.h"
#include "rendersystem/isprite.h"
#include "rendersystem/statedesc.h"

namespace clearsky
{
	class Light;
	class Fog;
	class Camera;
	class KeyboardHandler;
	class MouseHandler;
	class StartParams;
	class DefaultEffect;

	//maximum of lights enabled
	const unsigned int MAX_NUM_LIGHTS = 6;
	const unsigned int MAX_NUM_TEXTURE_LAYERS = 7;

	//supported draw layouts for primitives
	enum CLEARSKY_API PRIMITIVE_TOPOLOGY
	{
		PT_TRIANGLELIST,
		PT_TRIANGLESTRIP,
		PT_POINTLIST,
		PT_LINELIST,
		PT_LINESTRIP,
		PT_LINELIST_ADJ,
		PT_LINESTRIP_ADJ,
		PT_TRIANGLELIST_ADJ,
		PT_TRIANGLESTRIP_ADJ
	};

	//stored material infomation for faces, objects ...
	//TODO: move in own class
	struct CLEARSKY_API Material
	{
		Material()
		{
			//set default color values, all to white
			diffuse  = Color(1,1,1,1);
			specular = Color(1,1,1,1);
			ambient  = Color(1,1,1,1);
			emissive = Color(1,1,1,1);
			
			id=0;  //id of material, used when many materials are used
			startFace=0; //start face index into the indexbuffer
			numFaces=0; //number of faces using this material

			this->texture = NULL;
			this->specularTexture = NULL;
			this->heightmapTexture = NULL;
			this->normalsTexture   = NULL;
		}

		Material(Material &mat)
		{
			this->diffuse  = mat.diffuse;
			this->ambient  = mat.ambient;
			this->specular = mat.specular; 

			this->id = mat.id;
			this->startFace = mat.startFace;
			this->numFaces  = mat.numFaces;

			this->texture = NULL;
			this->specularTexture  = NULL;
			this->heightmapTexture = NULL;
			this->normalsTexture   = NULL;
		}


		Color diffuse; //global scattered light
		Color specular;
		Color ambient;	
		Color emissive; //color that emits the object itself

		char id;
		int startFace;
		int numFaces;
		ITexture2D *texture; //diffuse texture map
		ITexture2D *specularTexture; 
		ITexture2D *heightmapTexture;
		ITexture2D *normalsTexture; 
		//float power;
		

	};

	struct EffectMaterial
	{
		Color diffuse;
		Color specular;
		Color ambient;
	};

	enum RESOURCE_USAGE
	{
		USAGE_DEFAULT = 0,
		USAGE_IMMUTABLE = 1,
		USAGE_DYNAMIC = 2,
		USAGE_STAGING = 3
	};

	//rendering window properties
	struct WindowDesc
	{
		WindowDesc()
		{
			this->fullscreen = false;
			this->width = 1024;
			this->height = 768;
			this->multiSamplingCount = 1;
			this->multiSamplingLevel = 0;

			hwnd = NULL;
		}

		bool fullscreen;
		int width;
		int height;
		int multiSamplingCount;
		int multiSamplingLevel;

		WindowHandler hwnd;
	};

	//screen dimensions
	struct Viewport
	{
		int x;
		int y;
		int width;
		int height;
		float minDepth;
		float maxDepth;
	};

	struct RendererStates
	{
		bool lighting;
		bool texturing;
		bool specTexturing;
		bool heightTexturing;
		bool alphamapping; //use alpha texture for transparancy
		bool blending;
		bool fullscreen;
		bool defaultEffect;
		bool normalsPosition;
		bool frontOrdnerCw;
		bool wireframe;
		bool fogging;
		int sampleCount;
		int sampleLevel;
		PRIMITIVE_TOPOLOGY primitiveTopology;
		CULL_MODE cullMode;
		DATA_FORMAT backbufferFormat;
		DATA_FORMAT depthStencilFormat;
		Color clearColor;
		Viewport viewport;
		float fov;
		float aspectRation;
		float nearPlane; 
		float farPlane;
	};

	//TODO move to isprite.h
	enum CLEARSKY_API FRAME_TYPE
	{
		FRAME_STRIP,
		FRAME_RECT
	};


	/**
	* Defines common interfaces for all the renderes
	*/
	class CLEARSKY_API IRenderer
	{
		public:
			virtual RETURN_VALUE create(const StartParams *startparams) = 0;
			virtual RETURN_VALUE create(const char *title, int width, int height) = 0;
			virtual void release()=0;

			virtual void setFullScreen(bool fullscreen) = 0;
			virtual bool getFullScreen() = 0;

			//begin drawing
			virtual void begin(bool clearRenderTarget=false, bool clearDepthStencil=false) = 0;
			virtual void end() = 0;

			virtual void update()=0;

			//clear background
			virtual void clear(float red, float green, float blue) = 0;

			virtual void present() = 0;
			
			virtual void draw(int numVertexes, int startVertex) = 0;
			virtual void drawIndexed(int num, int startIndex, int baseIndex) = 0;

			virtual void apply(int tech, int pass)=0;

			virtual ITexture2D* createTexture2D()=0;
			virtual IBuffer* createBuffer()=0;
			virtual IEffect* createEffect()=0;
			virtual IInputLayout* createInputLayout()=0;
			virtual ISprite* createSprite()=0;

			virtual void saveWorld()=0;
			virtual void restoreWorld()=0;
			virtual void setWorld(Matrix world)=0;
			virtual void multWorld(Matrix world)=0;
			virtual Matrix getWorld()=0;

			virtual float setFOV(float newFOV)=0;
			virtual float getFOV()=0;

			virtual float getAspectRation()=0;

			virtual void rotateY(float deg)=0;
			virtual void rotateX(float deg)=0;
			virtual void rotateZ(float deg)=0;
			virtual void translate(float x, float y, float z)=0;
			virtual void scale(float factorX, float factorY, float factorZ)=0;

			virtual RETURN_VALUE setCurrentOutputWindow(int id)=0;

			virtual RETURN_VALUE setViewport(int x, int y, int width, int height)=0;
			virtual Viewport getViewport()=0;
			virtual RETURN_VALUE setProjection(int width, int height, float nearPlane, float farPlane, float fov)=0;
			virtual RETURN_VALUE setOrthoProjection(int width, int height, float nearPlane, float farPlane)=0;
			virtual Matrix getProjection()=0;

			virtual Matrix getViewTransformation()=0;

			virtual RETURN_VALUE setWireframe(bool wireframe)=0;
			virtual bool isWireframe()=0;
			virtual PRIMITIVE_TOPOLOGY setPrimitive(PRIMITIVE_TOPOLOGY topology)=0;
			virtual PRIMITIVE_TOPOLOGY getPrimitive()=0;
			virtual RETURN_VALUE setTexturing(bool enableTex)=0;
			virtual bool isTexturing()=0;
			virtual RETURN_VALUE setSpecTexturing(bool enableTex)=0;
			virtual bool isSpecTexturing()=0;
			virtual RETURN_VALUE setHeightTexturing(bool enableTex)=0;
			virtual bool isHeightTexturing()=0;
			virtual RETURN_VALUE setAlphaMapping(bool enable)=0;
			virtual bool isAlphaMapping()=0;
			virtual RETURN_VALUE setLighting(bool enable)=0;
			virtual bool isLighting()=0;
			virtual RETURN_VALUE setBlending(bool enable)=0;
			virtual bool isBlending()=0;
			//fixed function pipeline fog
			virtual RETURN_VALUE setFogging(bool enable)=0;
			virtual bool isFogging()=0;
			virtual CULL_MODE setCullMode(CULL_MODE newMode)=0;
			virtual CULL_MODE getCullMode()=0;
			virtual bool setFrontOrder(bool cw)=0;
			virtual Color setClearColor(float r, float g, float b, float a)=0;
			virtual Color getClearColor()=0;
			virtual RETURN_VALUE setEffect(IEffect *newEffect)=0;
			virtual IEffect* getEffect()=0;

			virtual void setDefaultEffect()=0;

			virtual RETURN_VALUE setInputLayout(IInputLayout *newInputLayout)=0;
			virtual IInputLayout* getInputLayout()=0;

			virtual void SetScissorRect(Rect rect)=0;
			virtual void SetScissorRect(int left, int top, int right, int bottom)=0;
			virtual Rect getScissorRect()=0;

			virtual RETURN_VALUE setMultisampling(int sampleCount, int qualityLevel)=0;
			virtual RETURN_VALUE setBackbufferFormat(DATA_FORMAT format)=0;
			virtual RETURN_VALUE setDepthStencilFormat(DATA_FORMAT format)=0;

			virtual RETURN_VALUE setBlendState(BlendState *newBlendState)=0;
			virtual BlendState* getBlendState()=0;
			virtual RETURN_VALUE setRasterizerState(RasterizerState *newRasterizerState)=0;
			virtual RasterizerState* getRasterizerState()=0;
			virtual RETURN_VALUE setDepthStencilState(DepthStencilState *newDepthStencilState)=0;
			virtual DepthStencilState* getDepthStencilState()=0;
			virtual RETURN_VALUE setSamplerState(SamplerState *newSamplerState)=0;
			virtual SamplerState* getSamplerState()=0;

			virtual void pushBlendState()=0;
			virtual void popBlendState()=0;
			virtual void pushRasterizerState()=0;
			virtual void popRasterizerState()=0;
			virtual void pushDepthStencilState()=0;
			virtual void popDepthStencilState()=0;
			virtual void pushSamplerState()=0;
			virtual void popSamplerState()=0;
			virtual void pushEffect()=0;
			virtual void popEffect()=0;

			virtual RETURN_VALUE setLight(int index, Light *light)=0;
			virtual RETURN_VALUE setFog(Fog *fog)=0;
			virtual RETURN_VALUE setTexture(ITexture2D *texture, const unsigned int layer = 0)=0;
			virtual RETURN_VALUE setSpecTexture(ITexture2D *texture)=0;
			virtual RETURN_VALUE setHeightTexture(ITexture2D *texture)=0;
			virtual void setAlphaTexture(ITexture2D *texture)=0;

			virtual void addOutputWindow(WindowDesc *window, int id)=0;

			virtual Material setMaterial(Material newMaterial)=0;
			virtual Material getMaterial()=0;

			virtual const char* getFeatureLevel()=0;
			virtual const char* getDriverType()=0;

			virtual void rayFromScreen(const int x, const int y, Vector3D &rayOrigin, Vector3D &rayDir)=0;
			virtual void unproject(const int x, const int y, Vector3D &worldPoint)=0;

			//TODO move camera to e.g Engine class
			virtual Camera* getCamera()=0;
			virtual void setKeyboardHandler(KeyboardHandler *handlerHandler)=0;
			virtual void setMouseHandler(MouseHandler *mouseHandler)=0;

			virtual void begin2D()=0;
			virtual void drawRect(float x, float y, float width, float height, Color color=Color(1.0f,1.0f,1.0f,1.0f))=0;
			virtual void drawRect(float x, float y, float width, float height, float depth, Color color=Color(1.0f,1.0f,1.0f,1.0f))=0;
			virtual void drawFilledRect(float x, float y, float width, float height, Color color = Color(1.0f,1.0f,1.0f,1.0f))=0;
			virtual void drawFilledRect(float x, float y, float width, float height, float depth, Color color = Color(1.0f,1.0f,1.0f,1.0f))=0;
			virtual void drawFilledRect(float x, float y, float width, float height, float depth, Gradient gradient)=0;
			virtual void drawLine(float xStart, float yStart, float xEnd, float yEnd, Color color = Color(1.0f,1.0f,1.0f,1.0f))=0;
			virtual void drawLine(Vector3D start, Vector3D end, Color color = Color(1.0f,1.0f,1.0f,1.0f))=0;
			virtual void drawLine(float xStart, float yStart, float xEnd, float yEnd, float depth, Color color = Color(1.0f,1.0f,1.0f,1.0f))=0;
			virtual void drawCircle(float xCenter, float yCenter, float radius)=0;
			virtual void drawCircle(float xCenter, float yCenter, float depth, float radius)=0;
			virtual void end2D()=0;
			 
			virtual bool isRunning()=0;

			virtual void setDefaultRenderTarget()=0;

			virtual void resize(int width, int height)=0;

			virtual void exit(bool exit)=0;
	};
}

#endif