/*
*
*/
#pragma once
#ifndef _CLEARSKY_DX11_RENDERER_H_
#define _CLEARSKY_DX11_RENDERER_H_


#include "core/core.h"
#include "core/utils.h"
#include "core/globals.h"
#include "core/timer.h"
#include "core/logger.h"

#include "math/mathutils.h"

#include "rendersystem/irenderer.h"
#include "inputsystem/keyboardHandler.h"
#include "inputsystem/mousehandler.h"
#include "rendersystem/camera.h"
#include "rendersystem/light.h"
#include "rendersystem/fog.h"
#include "rendersystem/startparams.h"

#include "rendersystem/directx11/dx11inputlayout.h"
#include "rendersystem/directx11/dx11effect.h"
#include "rendersystem/directx11/dx11texture2D.h"
#include "rendersystem/directx11/dx11buffer.h"
#include "rendersystem/directx11/dx11sprite.h"

namespace clearsky
{

	struct SwapChain
	{
		SwapChain():windowDesc(NULL),
					dxSwapChain(NULL),
					renderTarget(NULL),
					depthStencilView(NULL),
					id(0)
		{
		}

		WindowDesc *windowDesc;
		IDXGISwapChain *dxSwapChain;
		ID3D11RenderTargetView *renderTarget;
		ID3D11DepthStencilView *depthStencilView;

		int id;
	};

	//main class for rendering
	class CLEARSKY_API DX11Renderer : public IRenderer
	{
		public:
			DX11Renderer();
			~DX11Renderer();

			RETURN_VALUE create(const StartParams *startparams);
			RETURN_VALUE create(const char *title, int width, int height);

			void setFullScreen(bool fullscreen);
			bool getFullScreen();

			void begin(bool clearRenderTarget=false, bool clearDepthStencil=false);
			void end();
			void clear(float red, float green, float blue);

			void update();

			void present();
			void draw(int numVertexes, int startVertex);
			void drawIndexed(int num, int startIndex, int baseIndex);

			void setKeyboardHandler(KeyboardHandler *handlerHandler);
			void setMouseHandler(MouseHandler *mouseHandler);
			RETURN_VALUE setEffect(IEffect *newEffect);
			IEffect* getEffect();

			void setDefaultEffect();

			RETURN_VALUE setCurrentOutputWindow(int id);

			RETURN_VALUE setInputLayout(IInputLayout *newInputLayout);
			IInputLayout* getInputLayout();

			Camera *getCamera();

			RETURN_VALUE setViewport(int x, int y, int width, int height);
			Viewport getViewport();
			RETURN_VALUE setViewTransformation(Vector3D position, Vector3D direction, Vector3D up);
			Matrix getViewTransformation();
			RETURN_VALUE setProjection(int width, int height, float near, float far, float fov);
			Matrix getProjection();
			RETURN_VALUE setOrthoProjection(int width, int height, float nearPlane, float farPlane);

			void addOutputWindow(WindowDesc *window, int id);

			RETURN_VALUE setWireframe(bool wireframe);
			bool isWireframe();
			bool setFrontOrder(bool cw);
			RETURN_VALUE setTexturing(bool enableTex);
			bool isTexturing();
			RETURN_VALUE setSpecTexturing(bool enableTex);
			bool isSpecTexturing();
			RETURN_VALUE setHeightTexturing(bool enableTex);
			bool isHeightTexturing();
			RETURN_VALUE setAlphaMapping(bool enable);
			bool isAlphaMapping();
			RETURN_VALUE setLighting(bool enable);
			bool isLighting();
			RETURN_VALUE setBlending(bool enable);
			bool isBlending();
			RETURN_VALUE setFogging(bool enable);
			bool isFogging();
			Color setClearColor(float r, float g, float b, float a);
			Color getClearColor();

			void depthEnable(bool enable);
			bool isDepthEnabled();

			RETURN_VALUE setMultisampling(int sampleCount, int qualityLevel);
			RETURN_VALUE setBackbufferFormat(DATA_FORMAT format);
			RETURN_VALUE setDepthStencilFormat(DATA_FORMAT format);

			PRIMITIVE_TOPOLOGY setPrimitive(PRIMITIVE_TOPOLOGY topology);
			PRIMITIVE_TOPOLOGY getPrimitive();

			CULL_MODE setCullMode(CULL_MODE newMode);
			CULL_MODE getCullMode();
			ID3D11Device *getDevice();
			RETURN_VALUE setNormalsPosition(bool enable);
			bool isNormalsPosition();

			RETURN_VALUE setBlendState(BlendState *newBlendState);
			BlendState* getBlendState();
			RETURN_VALUE setRasterizerState(RasterizerState *newRasterizerState);
			RasterizerState* getRasterizerState();
			RETURN_VALUE setDepthStencilState(DepthStencilState *newDepthStencilState);
			DepthStencilState* getDepthStencilState();
			RETURN_VALUE setSamplerState(SamplerState *newSamplerState);
			SamplerState* getSamplerState();

			void pushBlendState();
			void popBlendState();
			void pushRasterizerState();
			void popRasterizerState();
			void pushDepthStencilState();
			void popDepthStencilState();
			void pushSamplerState();
			void popSamplerState();
			void pushEffect();
			void popEffect();

			void SetScissorRect(Rect rect);
			void SetScissorRect(int left, int top, int right, int bottom);
			Rect getScissorRect();

			void updateView();

			void apply(int tech, int pass);

			void rotateY(float deg);
			void rotateX(float deg);
			void rotateZ(float deg);
			void translate(float x, float y, float z);
			void scale(float factorX, float factorY, float factorZ);

			void setWorld(Matrix world);
			void multWorld(Matrix world);
			Matrix getWorld();

			float setFOV(float newFOV);
			float getFOV();

			float getAspectRation();

			void rayFromScreen(const int x, const int y, Vector3D &rayOrigin, Vector3D &rayDir);
			void unproject(const int x, const int y, Vector3D &worldPoint);

			RETURN_VALUE setLight(int index, Light *light);
			RETURN_VALUE setFog(Fog *fog);
			RETURN_VALUE setTexture(ITexture2D *texture, const unsigned int layer=0);
			RETURN_VALUE setSpecTexture(ITexture2D *texture);
			RETURN_VALUE setHeightTexture(ITexture2D *texture);
			void setAlphaTexture(ITexture2D *texture);
			Material setMaterial(Material newMaterial);
			Material getMaterial();

			const char* getFeatureLevel();
			const char* getDriverType();

			void saveWorld();
			void restoreWorld();

			void release();

			DX11Texture2D* createTexture2D();
			DX11Buffer* createBuffer();
			IEffect* createEffect();
			IInputLayout* createInputLayout();
			ISprite* createSprite();

			void begin2D();
			void drawRect(float x, float y, float width, float height, Color color);
			void drawRect(float x, float y, float width, float height, float depth, Color color);
			void drawFilledRect(float x, float y, float width, float height, Color color);
			void drawFilledRect(float x, float y, float width, float height, float depth, Color color);
			void drawFilledRect(float x, float y, float width, float height, float depth, Gradient gradient);
			void drawLine(float xStart, float yStart, float xEnd, float yEnd, Color color);
			void drawLine(float xStart, float yStart, float xEnd, float yEnd, float depth, Color color);
			void drawLine(Vector3D start, Vector3D end, Color color = Color(1.0f,1.0f,1.0f,1.0f));
			void drawCircle(float xCenter, float yCenter, float radius);
			void drawCircle(float xCenter, float yCenter, float depth, float radius);
			void end2D();

			bool isRunning();

			void setDefaultRenderTarget();

			void resize(int width, int height);

			void exit(bool exit);
		private:
			HWND createWindow(TCHAR *title, int width, int height);
			static LRESULT CALLBACK staticWndProc(HWND, UINT, WPARAM, LPARAM);
			LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
			RETURN_VALUE createDevice();

			D3D_DRIVER_TYPE stringToDriverType(const char *driverType);
			D3D_FEATURE_LEVEL stringToFeatureLevel(const char *featureLevel);

			D3D11_PRIMITIVE_TOPOLOGY convertTopology(PRIMITIVE_TOPOLOGY);
			D3D11_DEPTH_STENCIL_DESC convertDepthStencilDesc(DepthStencilState *depthStencilState);
			D3D11_DEPTH_STENCILOP_DESC convertDepthStencilFuncDesc(DepthStencilFuncDesc funcDesc);

			D3D11_RASTERIZER_DESC convertRasterizerDesc(RasterizerState *newRasterizerState);
			D3D11_SAMPLER_DESC convertSamplerDesc(SamplerState *newSamplerState);

			//read default setting for render states(lighting, texturing etc.) from the shader
			void readDefaultShaderValues();

			RendererStates m_states;

			PRIMITIVE_TOPOLOGY m_primitiveTopology;

			ID3D11DeviceContext *m_immediateContext;
			ID3D11Device *m_device;

			D3D11_RASTERIZER_DESC m_rasterizerDesc;

			D3D11_SAMPLER_DESC m_samplerDesc;
			ID3D11SamplerState *m_samplerState;

			D3D11_DEPTH_STENCIL_DESC m_depthStencilDesc;
			ID3D11DepthStencilState *m_depthStencilState;

			D3D11_BLEND_DESC m_blendDesc;
			
			//ID3D11DepthStencilState *m_depthStencilState;
			ID3D11BlendState *m_blendState;
			ID3D11RasterizerState *m_rasterizerState;

			RasterizerState *m_rasterizerStateDesc;
			DepthStencilState *m_depthStencilStateDesc;
			SamplerState *m_samplerStateDesc;
			BlendState *m_blendStateDesc;

			std::vector<SwapChain *> m_outputWindows;
			SwapChain *m_currentChain;

			std::vector<RasterizerState*> m_savedRasterizerStates;
			std::vector<DepthStencilState*> m_savedDepthStencilStates;
			std::vector<SamplerState*> m_savedSamplerStates;
			std::vector<BlendState*> m_savedBlendStates;
			std::vector<IEffect*> m_savedEffects;

			bool m_savedLighting;

			Rect m_scissorRect;

			Matrix m_view;
			Matrix m_proj;
			Matrix m_world;
			Matrix m_mvp;

			Matrix m_worldSaved;

			IEffect *m_curEffect; //current effect

			DX11InputLayout *m_inputLayout;

			DX11Effect *m_fixedFuncEffect; //effect emulates fixedfunctionpipeline
			DX11InputLayout *m_fixedFuncInputLayout;

			DX11InputLayout *m_inputLayout2D;

			D3D_FEATURE_LEVEL m_featureLevel;
			D3D_DRIVER_TYPE   m_driverType;

			KeyboardHandler *m_keyboardHandler;
			MouseHandler *m_mouseHandler;
			Camera *m_camera;
			Timer  *m_timer;
			Material m_material;
			D3D11_VIEWPORT m_viewport;

			bool m_run;

			//--------2D datas
			DX11Buffer *m_buffer2DPrimitive;		//Vertexbuffer for 2d drawing

	};
}

#endif