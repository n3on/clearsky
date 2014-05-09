/*
*(C)2011 Symplatonix Software
*/


/*
*This sample creates colored and rotated tringles
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
clearsky::IBuffer   *triangleData2=NULL; //will be used to render the second triangle

float angleTri1 = 0.0f; //rotation angle for triangle1
float angleTri2 = 0.0f; //rotation angle for triangle2

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

	result = renderer->create("clearsky sample3", 1024, 768);
	if(result!=clearsky::RETURN_OK)
	{
		clearsky::LOG->logMsg(clearsky::LT_STATUS, "can not create rendering interface");
		clearsky::LOG->logMsg(clearsky::LT_ERROR, " message: %s",clearsky::getReturnMessage(result));

		return 1;
	}

	//renderer->setOrthoProjection(1024,768,0,100);
	renderer->setCullMode(clearsky::CULL_MODE_NONE); //to prevent that backfaces are culled while rotation

	//create Triangle vertices
	clearsky::Vertex triangle[3];
	triangle[0].position = clearsky::Vector3D(-1,0,0);
	triangle[0].color    = clearsky::Color(1,0,0,0);
	triangle[1].position = clearsky::Vector3D(0,1,0);
	triangle[1].color    = clearsky::Color(0,1,0,0);
	triangle[2].position = clearsky::Vector3D(1,0,0);
	triangle[2].color    = clearsky::Color(0,0,1,0);

	renderer->getCamera()->position = clearsky::Vector3D(0,0,-5);

	triangleData = renderer->createBuffer();
	result = triangleData->create(triangle,sizeof(clearsky::Vertex),3);
	if(result!=clearsky::RETURN_OK)
	{
		clearsky::LOG->logMsg(clearsky::LT_STATUS, "can not create vertex buffer");
		clearsky::LOG->logMsg(clearsky::LT_ERROR, " message: %s",clearsky::getReturnMessage(result));

		return 1;
	}

	//create vertices for the second triangle
	clearsky::Vertex triangle2[3];
	triangle2[0].position = clearsky::Vector3D(-1,0,0);
	triangle2[0].color    = clearsky::Color(0,0,1,0);
	triangle2[1].position = clearsky::Vector3D(0,1,0);
	triangle2[1].color    = clearsky::Color(0,0,1,0);
	triangle2[2].position = clearsky::Vector3D(1,0,0);
	triangle2[2].color    = clearsky::Color(0,0,1,0);


	triangleData2 = renderer->createBuffer();
	result = triangleData2->create(triangle2,sizeof(clearsky::Vertex),3);
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

		//frame independent rotation
		angleTri1 += (float)(clearsky::GLOBALS->getDeltaTime()*100);
		if(angleTri1>360)
			angleTri1 = 0;

		angleTri2 = -angleTri1;

		renderer->saveWorld(); //make sure, that the transformation does not affect further rendering objects
			renderer->rotateY(angleTri1);
			renderer->translate(0,-1,0);
			renderer->applyTransform(0,0); //apple the rotation
			triangleData->activate();//activate the vertexbuffer
			renderer->draw(3,0); //draw the triangle buffer, param1=number of vertices
		renderer->restoreWorld();

		renderer->saveWorld();
			renderer->rotateX(180); //flip upside down
			renderer->rotateY(angleTri2);
			renderer->translate(0,1,0);
			renderer->applyTransform(0,0);
			triangleData2->activate();//activate the vertexbuffer
			renderer->draw(3,0); //draw the triangle buffer, param1=number of vertices
		renderer->restoreWorld();
		

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

	if(!triangleData2)
	{
		triangleData2->release();
		delete triangleData2;
	}
	app.release();

	return 0;
}
