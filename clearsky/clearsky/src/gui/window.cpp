/*
*
*/

#include "core/precompiled.h"
#include "gui/window.h"

namespace clearsky
{
	Window::Window()
	{
		this->init();
	}

	Window::Window(GUISkin *guiSkin)
	{
		this->m_skin = guiSkin;
		this->init();
	}

	Window::~Window()
	{
		for(unsigned int i=0; i<this->m_children.size(); ++i)
			SAFE_DELETE(this->m_children[i]);

	}

	void Window::init()
	{
		m_parent		= NULL;
		m_isVisible		= true;
		m_showTitle		= true;
		m_active		= true;
		m_width			= 100;
		m_height		= 100;
		m_type			= GUI_TYPE_WINDOW;
		m_mouseMove		= false;
		m_mouseKeyDown	= false;
						//m_mouseKeyUp(false),
		m_lastMouseButton = -1;
		m_useSkin		  = true;
		m_isMovable		  = true;
		m_mouseIn		  = false;
		m_mouseOver		  = false;
		memset(this->m_caption,0,49);

		this->m_renderer	= GLOBALS->getRenderer();
		this->m_position.x	= m_positionAbs.x = 0.0f;
		this->m_position.y	= m_positionAbs.y = 0.0f;
		this->m_position.z	= m_positionAbs.z = START_DEPTH;

		this->reloadSkin();
		
	}

	void Window::reloadSkin()
	{
		if(!this->m_skin)
			return;

		SkinData* skin = this->m_skin->getCurrentSkin();

		this->m_windowColor		   = skin->windowColor;
		this->m_topwindowColor	   = skin->topwindowColor;
		this->m_topwindowTexture   = skin->topwindowTexture;
		this->m_topwindowTexturing = skin->topwindowTexturing; 

		this->m_windowTexture	   = skin->windowTexture;
		this->m_windowTexturing    = skin->windowTexturing;
		this->m_titleColor         = skin->windowTitleColor;
		this->m_titleSize          = skin->windowTitleHeight;
		this->m_windowFont         = skin->windowFont;
		this->m_windowFontColor    = skin->windowFontColor;

		this->m_titleFont		   = skin->windowTitleFont;
		this->m_titleColor		   = skin->windowTitleColor;
		this->m_titleFontColor	   = skin->windowTitleFontColor;
		this->m_titleTexture	   = skin->windowTitleTexture;
		this->m_titleTexturing     = skin->windowTitleTexturing;

		for(unsigned int i=0; i<this->m_children.size(); ++i)
			this->m_children[i]->reloadSkin();
	}

	void Window::show()
	{
		if(!this->m_isVisible)
			return;

		if(this->paint()==SKIP_MSG)
			return;

	
		//bool lastTexturing = this->m_renderer->isTexturing();
		//this->m_renderer->setTexturing(this->m_windowTexturing);
			
		bool lastTexturing = this->m_renderer->isTexturing();

		//clip children on parents boundary
		//RasterizerState *rasterizerState = this->m_renderer->getRasterizerState();
		//bool lastScissor = rasterizerState->scissor;
		//this->m_renderer->pushRasterizerState();
		//this->m_renderer->begin2D();
		if(this->m_type!=GUI_TYPE_TOPWINDOW)
		{
			float titleHeight = 0.0f;

			if(this->m_showTitle)
			{
				lastTexturing = this->m_renderer->isTexturing();
				if(this->m_titleTexturing)
				{
					this->m_renderer->setTexturing(true);
					this->m_renderer->setTexture(this->m_titleTexture);
					this->m_renderer->apply(1,0);
				}
				else
					this->m_renderer->setTexturing(false);

				//draw titlebar
				this->m_renderer->drawFilledRect(this->m_positionAbs.x,
													this->m_positionAbs.y,
													this->m_width,
													(float)this->m_titleSize,
													this->m_position.z,
													this->m_titleColor);

				//if(this->m_titleTexturing)
				//{
					this->m_renderer->setTexturing(lastTexturing);
					//this->m_renderer->apply(1,0);
				//}

				titleHeight = (float)this->m_titleSize;
				
				//title text
				Font* font = this->m_titleFont;
				if(font)
				{
					Gradient lastColor = font->getGradient();
					font->setGradient(this->m_titleFontColor);

					font->setString(255, this->m_caption);
					int deltaY = (int)(titleHeight-font->getTextHeight())>>1;
					int deltaX = (int)(this->m_width-font->getTextWidth())>>1;
					//printf("font width = %d height = %d\n", deltaX, deltaY);
					if(deltaX>0 && deltaY>0)
						font->show((int)this->m_positionAbs.x+deltaX, (int)this->m_positionAbs.y+deltaY);
					else
						font->show((int)this->m_positionAbs.x, (int)this->m_positionAbs.y+deltaY);
						
					font->setGradient(lastColor);

					this->m_renderer->begin2D();
				}
			}

			lastTexturing = this->m_renderer->isTexturing();
			if(this->m_windowTexturing)
			{
				
				this->m_renderer->setTexturing(true);
				this->m_renderer->setTexture(this->m_windowTexture);
				this->m_renderer->apply(1,0);
			}
			else
				this->m_renderer->setTexturing(false);
			//draw window body
			this->m_renderer->drawFilledRect(this->m_positionAbs.x,
												this->m_positionAbs.y+titleHeight,
												this->m_width,
												this->m_height-titleHeight,
												this->m_position.z,
												this->m_windowColor);

			//if(this->m_windowTexturing)
			//{
				this->m_renderer->setTexturing(lastTexturing);
				//this->m_renderer->apply(1,0);
			//}

			//draw border rect
			this->m_renderer->drawRect(this->m_positionAbs.x,
												this->m_positionAbs.y,
												this->m_width,
												this->m_height,
												this->m_position.z,
												Color(0,0,0,1));
		}
		else
		{
			//draw topwindow
			//set texture
			lastTexturing = this->m_renderer->isTexturing();
			if(this->m_topwindowTexturing)
			{
				this->m_renderer->setTexturing(true);
				this->m_renderer->setTexture(this->m_topwindowTexture);
				this->m_renderer->apply(1,0);
			}
			else
				this->m_renderer->setTexturing(false);
		
			this->m_renderer->drawFilledRect(this->m_positionAbs.x,
												this->m_positionAbs.y,
												(float)GLOBALS->getWidth(),
												(float)GLOBALS->getHeight(),
												this->m_position.z,
												this->m_topwindowColor);

			//if(this->m_topwindowTexturing)
			//{
				this->m_renderer->setTexturing(lastTexturing);
				//this->m_renderer->apply(1,0);
			//}
		}

		RasterizerState *oldRasterizerState = this->m_renderer->getRasterizerState();
		bool lastScissor = oldRasterizerState->scissor;
		Rect oldScissorRect = this->m_renderer->getScissorRect();

		if(this->m_parent)
		{				
			//clip children on parents boundary
			
			oldRasterizerState->scissor = true;

			this->m_renderer->setRasterizerState(oldRasterizerState);

			Vector3D parentPosition = this->m_parent->getPositionAbs();

			Rect newScissorRect;

			//clip aganst parent rect, if child rect is outside of parent
			newScissorRect.left = (int)(parentPosition.x<this->m_positionAbs.x?this->m_positionAbs.x:parentPosition.x);
			newScissorRect.top  = (int)(parentPosition.y<this->m_positionAbs.y?this->m_positionAbs.y:parentPosition.y);
			newScissorRect.right = (int)(this->m_positionAbs.x + this->m_width);
			if(newScissorRect.right>(parentPosition.x+this->m_parent->getWidth()))
				newScissorRect.right = (int)(parentPosition.x+this->m_parent->getWidth());
			newScissorRect.bottom = (int)(this->m_positionAbs.y + this->m_height);
			if(newScissorRect.bottom>(parentPosition.y+this->m_parent->getHeight()))
				newScissorRect.bottom = (int)(parentPosition.y+this->m_parent->getHeight());

			this->m_renderer->SetScissorRect(newScissorRect);
		}

		for(unsigned int i=0; i<this->m_children.size(); ++i)
		{
			this->m_children[i]->show();
		}

		if(this->m_parent)
		{
			this->m_renderer->SetScissorRect(oldScissorRect);
			oldRasterizerState->scissor = lastScissor;
			//this->m_renderer->setRasterizerState(rasterizerState);
			this->m_renderer->setRasterizerState(oldRasterizerState);
		}

		
		//this->m_renderer->end2D();
		//this->m_renderer->setTexturing(lastTexturing);
		//this->paint();
		//this->m_renderer->setTexturing(lastTexturing);
	}

	void Window::setVisible(bool visible)
	{
		this->m_isVisible = visible;
	}

	bool Window::isVisible()
	{
		return this->m_isVisible;
	}

	void Window::showTitle(bool show)
	{
		this->m_showTitle = show;
	}

	bool Window::isTitleVisible()
	{
		return this->m_showTitle = false;
	}

	void Window::setActive(bool active)
	{
		this->m_active = active;
	}

	bool Window::isActive()
	{
		return this->m_active;
	}

	void Window::setMovable(bool movable)
	{
		this->m_isMovable = movable;
	}

	bool Window::isMovable()
	{
		return this->m_isMovable;
	}

	void Window::setParent(Window *parent)
	{
		if(parent)
			this->m_parent = parent;
	}

	Window* Window::getParent()
	{
		return this->m_parent;
	}

	void Window::addMouseListener(IMouseListener *mouseListener)
	{
		if(mouseListener)
			this->m_mouseListener.push_back(mouseListener);
	}

	void Window::removeMouseListener(IMouseListener *mouseListener)
	{
		//TODO: add remove code
	}

	void Window::addWindowListener(IWindowListener *windowListener)
	{
		if(windowListener)
			this->m_windowListener.push_back(windowListener);
	}

	void Window::removeWindowListener(IWindowListener *windowListener)
	{
		//TODO: add remove code
	}

	void Window::setColor(Color windowColor)
	{
		if(this->m_type != GUI_TYPE_TOPWINDOW)
			this->m_windowColor.startColor = this->m_windowColor.endColor = windowColor;
		else
			this->m_topwindowColor.startColor = this->m_topwindowColor.endColor = windowColor;
	}

	Color Window::getColor()
	{
		return this->m_windowColor.startColor;
	}

	bool Window::isTexturing()
	{
		//this->m_useSkin = false;
		return this->m_windowTexturing;
	}

	void Window::setTexturing(bool enableTex)
	{
		this->m_windowTexturing = enableTex;
		this->m_useSkin = false;
	}

	ITexture2D* Window::getTexture()
	{
		//this->m_useSkin = false;
		return this->m_windowTexture;
	}

	void Window::setTexture(ITexture2D *windowTexture)
	{
		this->m_windowTexture = windowTexture;
		this->m_useSkin = false;
	}

	void Window::addChild(Window *child)
	{
		if(child)
		{
			float newDepth = this->m_position.z-DEPTH_OFFSET;
			child->setDepth(newDepth);
			child->setParent(this);
			Vector3D childPosAbs = child->getPosition()+this->m_positionAbs;
			child->setPositionAbs(childPosAbs.x, childPosAbs.y);
			this->m_children.push_back(child);
		}
	}

	void Window::removeChild(Window *child)
	{
		//TODO: add remove code
	}

	void Window::removeChildren()
	{
		for(unsigned int i=0; i<this->m_children.size(); ++i)
		{
			SAFE_DELETE(this->m_children[i]);
		}

		this->m_children.clear();
	}

	void Window::setPosition(float x, float y)
	{
		this->m_position.x = x;
		this->m_position.y = y;

		if(!this->m_parent)
			this->setPositionAbs(x, y);

		/*for(int i=0; i<this->m_children.size(); ++i)
		{
			Vector3D childPosition = this->m_children[i]->getPositionAbs();
			this->m_children[i]->setPositionAbs(x+childPosition.x, y+childPosition.y);
		}*/
	}

	Vector3D Window::getPosition()
	{
		return this->m_position;
	}

	void Window::setPositionAbs(float x, float y)
	{	
		this->m_positionAbs.x = x;
		this->m_positionAbs.y = y;

		/*for(int i=0; i<this->m_children.size(); ++i)
		{
			Vector3D childPosition = this->m_children[i]->getPositionAbs();
			this->m_children[i]->setPositionAbs(x+childPosition.x, y+childPosition.y);
		}*/
	}

	Vector3D Window::getPositionAbs()
	{
		return this->m_positionAbs;
	}
			
	void Window::setDepth(float depth)
	{
		this->m_position.z = m_positionAbs.z = depth;

		for(unsigned int i=0; i<this->m_children.size(); ++i)
		{
			float newDepth = depth-DEPTH_OFFSET;
			this->m_children[i]->setDepth(newDepth);
		}
	}

	float Window::getDepth()
	{
		return this->m_position.z;
	}

	void Window::setSize(float width, float height)
	{
		//WindowDesc *windowDesc = this->m_renderer->getWindowDesc();
		//Viewport viewport = this->m_renderer->getViewport();

		/*printf("\nbefore width=%f height=%f\n",width, height);
		float newWidth  = (windowDesc->width*width)/viewport.width;
		float newHeight = (windowDesc->height*height)/viewport.height;
		printf("after width=%f height=%f\n",newWidth, newHeight);*/

		this->m_width  = width;
		this->m_height = height;
	}

	float Window::getWidth()
	{
		return this->m_width;
	}

	float Window::getHeight()
	{
		return this->m_height;
	}

	bool Window::pointInside(int x, int y) //check whether a point is inside window
	{
		return Math::pointInRect((float)x, (float)y, (float)this->m_positionAbs.x, (float)this->m_positionAbs.y, 
								 (float)this->m_width, (float)this->m_height);
	}

	bool Window::processEvents()
	{
		if(!this->m_isVisible)
			return false;

		//check mouse events
		//is mouse inside window?
		int mouseX, mouseY;
		this->m_mouseMove = false;
		GLOBALS->getMouseHandler()->getCurUnprojPos(mouseX, mouseY);
		if(this->pointInside(mouseX, mouseY))
		{
			bool childResult=false; //has child process the event?
			for(unsigned int i=0; i<this->m_children.size(); ++i)
			{
				childResult = this->m_children[i]->processEvents();
				if(childResult) break;
			}
			
			//if no child processed the event or we reached the final children
			if(!childResult || (this->m_children.size()<=0))
			{
				
				//mouseover event
				this->mouseOver(mouseX, mouseY);
				
				
				//mouseenter event
				if(!this->m_mouseIn)
				{
					this->m_mouseIn=true;
					//mouse justed entered
					this->mouseEnter();
				}

				//check for mousemove event
				if(GLOBALS->getMouseHandler()->isButtonDown(MOUSE_LBUTTON))
				{
					double deltaX = GLOBALS->getMouseHandler()->getDeltaX();
					double deltaY = GLOBALS->getMouseHandler()->getDeltaY();
					if(deltaX!=0 || deltaY!=0)
					{
						/*//check for mouse leave
						if(!this->pointInside((mouseX+deltaX), (mouseY+deltaY)) && this->m_mouseIn)
						{
							this->m_mouseIn = false;
							this->mouseLeave();
						}*/

						this->m_mouseMove = true;
						//process mousemove event
						this->mouseMove(mouseX, mouseY, deltaX, deltaY);

						if(this->m_isMovable)
						{
							//check for window move
							if(GLOBALS->getMouseHandler()->isButtonDown(MOUSE_LBUTTON))
							{
								this->windowMove(deltaX, deltaY);
								//this->windowMove(mouseX, mouseY);
							}
						}
					}
					else
						this->m_mouseMove = false;
				}
				//check mousekey up event

				//check for mousekey down event
				if(GLOBALS->getMouseHandler()->isButtonDown(MOUSE_LBUTTON) || GLOBALS->getMouseHandler()->isButtonDown(MOUSE_RBUTTON))
				{
					this->m_mouseKeyDown = true;
					bool lbutton = GLOBALS->getMouseHandler()->isButtonDown(MOUSE_LBUTTON);
					bool rbutton = GLOBALS->getMouseHandler()->isButtonDown(MOUSE_RBUTTON);
					//process mousekey event
					int keyCode= lbutton?lbutton:rbutton;
					this->m_lastMouseButton = keyCode;
					this->mouseKey(mouseX, mouseY, keyCode, true);
				}
				else if(this->m_mouseKeyDown && !GLOBALS->getMouseHandler()->isButtonDown(this->m_lastMouseButton))
				{
					this->mouseKey(mouseX, mouseY, this->m_lastMouseButton, false);

					//it's also a mouseclicked event
					this->mouseClicked(mouseX, mouseY, this->m_lastMouseButton);
					this->m_mouseKeyDown = false;
				}

			}//end if
			//event was processed
			return true;
		}//end if
		else
		{
			if(this->m_mouseIn)
			{
				this->m_mouseIn = false;
				this->mouseLeave();
			}
		}

		//event was not processed
		return false;
	}

	int Window::mouseKey(int x, int y, int keyCode, bool down)
	{
		MouseKeyEvent mouseKey;
		mouseKey.window = this;
		mouseKey.x = (int)(x - this->m_positionAbs.x);
		mouseKey.y = (int)(y - this->m_positionAbs.y);
		mouseKey.xAbs = x;
		mouseKey.yAbs = y;
		mouseKey.keyCode = keyCode;
		mouseKey.down = down;

		for(unsigned int i=0; i<this->m_mouseListener.size(); ++i)
			this->m_mouseListener[i]->mouseKey(mouseKey);

		return 0;
	}

	int Window::paint()
	{
		WindowEvent windowEvent;
		windowEvent.window = this;

		for(unsigned int i=0; i<this->m_windowListener.size(); ++i)
			this->m_windowListener[i]->windowPaint(windowEvent);

		return 0;
	}

	int Window::mouseMove(int x, int y, double deltaX, double deltaY)
	{
		MouseMoveEvent mouseEvent;
		mouseEvent.window = this;
		mouseEvent.x = (int)(x - this->m_positionAbs.x);
		mouseEvent.y = (int)(y - this->m_positionAbs.y);
		mouseEvent.xAbs = x;
		mouseEvent.yAbs = y;
		mouseEvent.deltaX = deltaX;
		mouseEvent.deltaY = deltaY;

		for(unsigned int i=0; i<this->m_mouseListener.size(); ++i)
			this->m_mouseListener[i]->mouseMove(mouseEvent);

		return 0;
	}

	/*int Window::mouseDown(int x, int y, int mouseKey)
	{

		return 0;
	}//end mousedown*/

	int Window::windowMove(double x, double y)
	{
		if(this->m_type!=GUI_TYPE_TOPWINDOW)
		{
			this->m_positionAbs.x += (float)x;
			this->m_positionAbs.y += (float)y;		
		
			this->m_position.x += (float)x;
			this->m_position.y += (float)y;

			for(unsigned int i=0; i<this->m_children.size(); ++i)
				this->m_children[i]->windowMove(x, y);

			WindowEvent windowEvent;
			windowEvent.window = this;
			windowEvent.deltaX = x;
			windowEvent.deltaY = y;
			windowEvent.x      = (int)this->m_position.x;
			windowEvent.y      = (int)this->m_position.y;
			windowEvent.xAbs   = (int)this->m_positionAbs.x;
			windowEvent.yAbs   = (int)this->m_positionAbs.y;

			for(unsigned int i=0; i<this->m_windowListener.size(); ++i)
				this->m_windowListener[i]->windowMove(windowEvent);
		}

		return 0;
	}

	int Window::mouseClicked(int x, int y, int mouseKey)
	{
		MouseKeyEvent event;
		event.window = this;
		event.x = (int)(x - this->m_positionAbs.x);
		event.y = (int)(y - this->m_positionAbs.y);
		event.xAbs = x;
		event.yAbs = y;
		event.keyCode = mouseKey;
		event.down = false;

		for(unsigned int i=0; i<this->m_mouseListener.size(); ++i)
			this->m_mouseListener[i]->mouseClicked(event);

		return 0;
	}

	int Window::mouseOver(int x, int y)
	{
		MouseEvent mouseEvent;
		mouseEvent.window = this;
		mouseEvent.x = (int)(x - this->m_positionAbs.x);
		mouseEvent.y = (int)(y - this->m_positionAbs.y);
		mouseEvent.xAbs = x;
		mouseEvent.yAbs = y;

		for(unsigned int i=0; i<this->m_mouseListener.size(); ++i)
			this->m_mouseListener[i]->mouseOver(mouseEvent);

		return 0;
	}

	int Window::mouseEnter()
	{
		this->m_mouseOver = true;

		MouseEvent mouseEvent;
		mouseEvent.window = this;
		mouseEvent.x = 0;
		mouseEvent.y = 0;
		mouseEvent.xAbs = 0;
		mouseEvent.yAbs = 0;

		for(unsigned int i=0; i<this->m_mouseListener.size(); ++i)
			this->m_mouseListener[i]->mouseEnter(mouseEvent);

		return 0;
	}

	int Window::mouseLeave()
	{
		this->m_mouseOver = false;

		MouseEvent mouseEvent;
		mouseEvent.window = this;
		mouseEvent.x = 0;
		mouseEvent.y = 0;
		mouseEvent.xAbs = 0;
		mouseEvent.yAbs = 0;

		for(unsigned int i=0; i<this->m_mouseListener.size(); ++i)
			this->m_mouseListener[i]->mouseLeave(mouseEvent);

		return 0;
	}

	GUI_ELEMENT_TYPE Window::getType()
	{
		return this->m_type;
	}

	void Window::setType(GUI_ELEMENT_TYPE type)
	{
		this->m_type = type;
	}

	int Window::getId()
	{
		return this->m_id;
	}

	void Window::setId(unsigned int newId)
	{
		this->m_id = newId;
	}

	std::string Window::getIdString()
	{
		return this->m_idString;
	}

	void Window::setIdString(std::string idString)
	{
		this->m_idString = idString;
	}

	void Window::setCaption(const char *caption)
	{
		memcpy(this->m_caption,caption,49);
	}

	const char* Window::getCaption()
	{
		return this->m_caption;
	}
}
