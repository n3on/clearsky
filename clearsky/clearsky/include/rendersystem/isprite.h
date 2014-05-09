/*
*
*/
#pragma once
#ifndef _CLEARSKY_I_SPRITE_H_
#define _CLEARSKY_I_SPRITE_H_

namespace clearsky
{
	class ISprite
	{
		public:
			virtual RETURN_VALUE init(const char *filename, int spriteWidth, int spriteHeight, int num, int maxFramesX, int maxFramesY)=0;

			virtual void show()=0;
			virtual void drawAll()=0;

			virtual void nextFrame()=0;
			virtual void prevFrame()=0;

			virtual void setCurrentFrame(int frame)=0;
			virtual int getCurrentFrame()=0;
			virtual int getNumFrames()=0;

			virtual void resetTransform()=0;
			virtual void translate(float x, float y, float z)=0;
			virtual void scale(float x, float y, float z)=0;
			virtual void transform(Matrix *mat)=0;
			
			virtual void billboard(Matrix *camMatrix)=0;
			virtual void unbillboard()=0;

			virtual void setVisible(bool visible)=0;
			virtual bool getVisible()=0;

			virtual void release()=0;
		private:
	};
}

#endif

