/*
*(C)2011 Symplatonix Software
*/


/*
*example setting shaders and vertexlayouts
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

clearsky::IBuffer *cubeDataVertex=NULL; //vertexbuffer for the cube
clearsky::IBuffer *cubeDataIndex=NULL; //indexbuffer

clearsky::IBuffer   *triangleData=NULL; //will be used to render the triangle

clearsky::IEffect *effect  = NULL;
clearsky::IEffect *effect2 = NULL;

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

	result = renderer->create("clearsky sample5", 1024, 768);
	if(result!=clearsky::RETURN_OK)
	{
		clearsky::LOG->logMsg(clearsky::LT_STATUS, "can not create rendering interface");
		clearsky::LOG->logMsg(clearsky::LT_ERROR, " message: %s",clearsky::getReturnMessage(result));

		return 1;
	}

	//initiale effect file and inputlayout to load the shader
	effect = renderer->createEffect();
	result = effect->create(L"..\\data\\shaders\\sample5.fx");
	if(result!=clearsky::RETURN_OK)
	{
		clearsky::LOG->logMsg(clearsky::LT_STATUS, "can not create effect file");
		clearsky::LOG->logMsg(clearsky::LT_ERROR, " message: %s",clearsky::getReturnMessage(result));

		return 1;
	}
	renderer->setEffect(effect);

	effect2 = renderer->createEffect();
	result = effect2->create(L"..\\data\\shaders\\sample5_2.fx");
	if(result!=clearsky::RETURN_OK)
	{
		clearsky::LOG->logMsg(clearsky::LT_STATUS, "can not create effect2 file");
		clearsky::LOG->logMsg(clearsky::LT_ERROR, " message: %s",clearsky::getReturnMessage(result));

		return 1;
	}

	renderer->setEffect(effect2);

	clearsky::IInputLayout *inputLayout = renderer->createInputLayout();
	result = inputLayout->create(clearsky::VF_PC, effect);
	if(result!=clearsky::RETURN_OK)
	{
		clearsky::LOG->logMsg(clearsky::LT_STATUS, "can not create inputlayout");
		clearsky::LOG->logMsg(clearsky::LT_ERROR, " message: %s",clearsky::getReturnMessage(result));

		return 1;
	}
	inputLayout->activate();

	//renderer->setOrthoProjection(1024,768,0,100);
	renderer->setCullMode(clearsky::CULL_MODE_NONE); //to prevent that backfaces are culled while rotation

	cubeDataVertex = renderer->createBuffer();
	cubeDataIndex  = renderer->createBuffer();

	clearsky::VertexPC vertices[8];

	vertices[0].position = clearsky::Vector3D(-1.0f, -1.0f, -1.0f);
	vertices[0].color    = clearsky::Color(1.0f,0.0f,0.0f,1.0f);

	vertices[1].position = clearsky::Vector3D(-1.0f, +1.0f, -1.0f);
	vertices[1].color    = clearsky::Color(0.0f,1.0f,0.0f,1.0f);

	vertices[2].position = clearsky::Vector3D(+1.0f, +1.0f, -1.0f);
	vertices[2].color    = clearsky::Color(0.0f,0.0f,1.0f,1.0f);

	vertices[3].position = clearsky::Vector3D(+1.0f, -1.0f, -1.0f);
	vertices[3].color    = clearsky::Color(1.0f,1.0f,0.0f,1.0f);

	vertices[4].position = clearsky::Vector3D(-1.0f, -1.0f, +1.0f);
	vertices[4].color    = clearsky::Color(0.0f,1.0f,1.0f,1.0f);

	vertices[5].position = clearsky::Vector3D(-1.0f, +1.0f, +1.0f);
	vertices[5].color    = clearsky::Color(1.0f,0.0f,1.0f,1.0f);

	vertices[6].position = clearsky::Vector3D(+1.0f, +1.0f, +1.0f);
	vertices[6].color    = clearsky::Color(1.0f,1.0f,1.0f,1.0f);

	vertices[7].position = clearsky::Vector3D(+1.0f, -1.0f, +1.0f);
	vertices[7].color    = clearsky::Color(0.0f,0.0f,0.0f,1.0f);

	result = cubeDataVertex->create(vertices, sizeof(clearsky::VertexPC),8);
	if(result!=clearsky::RETURN_OK)
	{
		clearsky::LOG->logMsg(clearsky::LT_STATUS, "can not create vertex buffer");
		clearsky::LOG->logMsg(clearsky::LT_ERROR, " message: %s",clearsky::getReturnMessage(result));

		return 1;
	}

	DWORD indices[] = 
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,		
		4, 3, 7
	};

	result = cubeDataIndex->create(indices, sizeof(DWORD), 12*3, clearsky::BT_INDEX);
	if(result!=clearsky::RETURN_OK)
	{
		clearsky::LOG->logMsg(clearsky::LT_STATUS, "can not create index buffer");
		clearsky::LOG->logMsg(clearsky::LT_ERROR, " message: %s",clearsky::getReturnMessage(result));

		return 1;
	}

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

	renderer->getCamera()->position = clearsky::Vector3D(2,3,-5);
	renderer->getCamera()->lookDir  = clearsky::Vector3D(0,0,0);

	return 0;
}

bool renderEffect1 = false;

int render()
{
	renderer->setEffect(effect2);

	//start rendering
	renderer->begin(true, true);
		
		//clear the render window to blue
		renderer->clear(0.0,0.0,0.0f);

		//frame independent rotation
		float rotation = (float)clearsky::GLOBALS->getDeltaTime()*100;

		//renderer->rotateX(rotation);
		renderer->rotateY(rotation);
		//renderer->rotateZ(rotation);

		cubeDataVertex->activate();//activate the vertexbuffer
		cubeDataIndex->activate();

		renderer->applyTransform(0,0);
			renderer->drawIndexed(6*3,0,0);
		renderer->applyTransform(0,1);
			renderer->drawIndexed(6*6,0,0);
		
	renderer->end();
	
	return 0;
}

int update()
{
	effect2->setInt("Time",(int)clearsky::Timer::getTickCount());

	app.update();
	return 0;
}

int shutdown()
{
	
	app.release();

	return 0;
}
