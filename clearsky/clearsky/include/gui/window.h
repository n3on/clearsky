/*
*
*/
#pragma once
#ifndef _CLEARSKY_GUI_WINDOW_H_
#define _CLEARSKY_GUI_WINDOW_H_

//#include "core/precompiled.h"
//#include <vector>
#include "core/utils.h"
#include "core/globals.h"
#include "inputsystem/mousehandler.h"
#include "math/mathutils.h"
#include "gui/event.h"
#include "gui/imouselistener.h"
#include "gui/iwindowlistener.h"
#include "gui/guiskin.h"

namespace clearsky
{
	//return values for event and virtual functions
	const int PASS_MSG=0; //the default msg handling will also be performed
	const int SKIP_MSG=1; //no default msg handling

	enum CLEARSKY_API GUI_ELEMENT_TYPE
	{
		GUI_TYPE_WINDOW,
		GUI_TYPE_TOPWINDOW,
		GUI_TYPE_BUTTON,
		GUI_TYPE_PUSHBUTTON
	};

	class CLEARSKY_API Window
	{
		public:
			Window();
			Window(GUISkin *guiSkin);
			virtual ~Window();

			void init();
			virtual void reloadSkin();

			void setVisible(bool visible);
			bool isVisible();
			void showTitle(bool show);
			bool isTitleVisible();
			void setActive(bool active);
			bool isActive();
			void setMovable(bool movable);
			bool isMovable();

			void addChild(Window *child);
			void removeChild(Window *child);
			void removeChildren();

			void setPosition(float x, float y);
			Vector3D getPosition();
			void setPositionAbs(float x, float y);
			Vector3D getPositionAbs();

			void setDepth(float depth);
			float getDepth();

			void setSize(float width, float height);
			float getWidth();
			float getHeight();

			void setParent(Window *parent);
			Window* getParent();

			void addMouseListener(IMouseListener *mouseListener);
			void removeMouseListener(IMouseListener *mouseListener);
			void addWindowListener(IWindowListener *windowListener);
			void removeWindowListener(IWindowListener *windowListener);

			void setColor(Color windowColor);
			Color getColor();
			bool isTexturing();
			void setTexturing(bool enableTex);
			ITexture2D* getTexture();
			void setTexture(ITexture2D *windowTexture);

			bool pointInside(int x, int y); //check wheter a point is inside window

			bool processEvents();

			virtual int paint();
			virtual int mouseMove(int x, int y, double deltaX, double deltaY);
			virtual int mouseKey(int x, int y, int keyCode, bool down);
			//virtual int mouseDown(int x, int y, int mouseKey);
			virtual int windowMove(double x, double y);
			virtual int mouseOver(int x, int y);
			virtual int mouseClicked(int x, int y, int keyCode);
			virtual int mouseEnter();
			virtual int mouseLeave();

			GUI_ELEMENT_TYPE getType();
			void setType(GUI_ELEMENT_TYPE type);

			int getId();
			void setId(unsigned int newId);

			std::string getIdString();
			void setIdString(std::string idString);

			void setCaption(const char *caption);
			const char* getCaption();

			void show();

		protected:
			Window *m_parent;
			bool m_isVisible;
			bool m_showTitle;
			bool m_active;
			bool m_isMovable;

			GUISkin *m_skin;

			Vector3D m_position;
			Vector3D m_positionAbs; //absolute position
			float m_width;
			float m_height;
			char m_caption[50];

			
			Gradient m_windowColor;
			Gradient m_topwindowColor;
			ITexture2D *m_topwindowTexture;
			bool	 m_topwindowTexturing;

			Gradient m_titleColor;
			int		 m_titleSize;
			Font	 *m_titleFont;
			Gradient m_titleFontColor;
			ITexture2D *m_titleTexture;
			bool	 m_titleTexturing;

			ITexture2D  *m_windowTexture;
			bool		 m_windowTexturing;
			bool		m_useSkin; //use skin class instead of property variables
			Font	*m_windowFont;
			Gradient m_windowFontColor;
			
			GUI_ELEMENT_TYPE m_type;
			int m_id;
			std::string m_idString;

			std::vector<Window*> m_children;
			std::vector<IMouseListener*> m_mouseListener;
			std::vector<IWindowListener*> m_windowListener;

			//event states
			bool m_mouseMove;
			bool m_mouseKeyDown;
			bool m_mouseOver;
			int m_lastMouseButton; //last mouse pressed key
			//bool m_mouseKeyUp;
			bool m_mouseIn; //mouse in window?

			IRenderer *m_renderer;
	};

}

#endif