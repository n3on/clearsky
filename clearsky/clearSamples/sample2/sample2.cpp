/*
*(C)2011 Symplatonix Software
*/


/*
*This sample creates a colored tringle
*
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
clearsky::IBuffer   *triangleData=NULL; //will be used to render the triangle

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

	//create renderer with renderwindow in window mode
	renderer->setFullScreen(false);

	result = renderer->create("clearsky sample2", 1024, 768);
	if(result!=clearsky::RETURN_OK)
	{
		clearsky::LOG->logMsg(clearsky::LT_STATUS, "can not create rendering interface");
		clearsky::LOG->logMsg(clearsky::LT_ERROR, " message: %s",clearsky::getReturnMessage(result));

		return 1;
	}

	renderer->setOrthoProjection(1024,768,0,100);
	
	//create Triangle vertices
	clearsky::Vertex triangle[3];
	triangle[0].position = clearsky::Vector3D(-200,1,0);
	triangle[0].color    = clearsky::Color(1,0,0,0);
	triangle[1].position = clearsky::Vector3D(0,200,0);
	triangle[1].color    = clearsky::Color(0,1,0,0);
	triangle[2].position = clearsky::Vector3D(200,1,0);
	triangle[2].color    = clearsky::Color(0,0,1,0);

	renderer->setPrimitive(clearsky::PT_TRIANGLELIST);
	triangleData = renderer->createBuffer();
	result = triangleData->create(triangle,sizeof(clearsky::Vertex),3);
	if(result!=clearsky::RETURN_OK)
	{
		clearsky::LOG->logMsg(clearsky::LT_STATUS, "can not create vertex buffer");
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
		renderer->clear(0.0,0.0,0.0f);

		triangleData->activate();//activate the vertexbuffer
		renderer->draw(3,0); //draw the triangle buffer, param1=number of vertices

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
	if(!triangleData)
	{
		triangleData->release();
		delete triangleData;
	}

	app.release();

	return 0;
}
