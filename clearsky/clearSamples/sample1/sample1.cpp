/*
*(C)2011 Symplatonix Software
*/


/*
*This sample creates empty clearsky window
*as a starting point for further projects
*/


#include <clearsky.h>

int init();
int render();
int update();
int shutdown();

//this is only simple example, in real projects use
//oop and try to avoid globals

clearsky::Engine app; //main interface to the engine
clearsky::IRenderer *renderer=NULL; //interface for drawing functions

int main()
{
	if(init())
		return 1;

	//start game main loop
	//exit with alt+f4
	while(app.run())
	{
		render();
		update();
	}

	shutdown();

	return 0;
}

int init()
{
	//return value of clearsky funktions
	clearsky::RETURN_VALUE result; 

	renderer = app.getRenderer(clearsky::RENDER_DX11);
	//create renderer with renderwindow in fullscreen
	result = renderer->create("clearsky sample1", 1024, 768);
	if(result!=clearsky::RETURN_OK)
	{
		clearsky::LOG->logMsg(clearsky::LT_STATUS, "can not create rendering interface");
		clearsky::LOG->logMsg(clearsky::LT_ERROR, " message: %s",clearsky::getReturnMessage(result));

		return 1;
	}

	return 0;
}

int render()
{
	//start rendering
	renderer->begin(true, true);
		
		//clear the render window to blue
		renderer->clear(0.0,0.0,1.0f);

	renderer->end();
	
	return 0;
}

int update()
{
	app.update();
	return 0;
}

int shutdown()
{
	app.release();

	return 0;
}
