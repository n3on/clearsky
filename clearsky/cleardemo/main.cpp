#include <stdlib.h>
#include <vector>

#include "clearsky.h"

#include "vld.h" //visual leak detector

using namespace clearsky;

#define WIDTH 1024
#define HEIGHT 768

//#define _CRTDBG_MAP_ALLOC
//#define _CRTDBG_MAPALLOC

// Mute sounds
//#define MUTE 

int init();
void render();
void update();
void shutdown();

clearsky::Engine app;
IRenderer *renderer=NULL;
clearsky::KeyboardHandler *keyboard = NULL;
MouseHandler *mouse;
GUI *gui;
clearsky::IInputLayout *input;

ITexture2D *terrainLayer1=NULL;
ITexture2D *terrainLayer2=NULL;
ITexture2D *treeTex=NULL;
ITexture2D *terrainTex=NULL;
ITexture2D *heightmapTex = NULL;

Skybox skybox;
MOFMesh *vadertie=NULL;
MOFMesh temple;
MOFMesh shotgun;
//Object triangle;
MOFMesh *vehicle=NULL;
MOFMesh *testMesh=NULL;
MOFMesh *tunnel=NULL;
MOFMesh *torret=NULL;
MOFMesh *spaceship=NULL;

Camera *camera=NULL;
ViewFrustum viewFrustum;

ISprite *weaponSprite;

MOFMesh *weapon1=NULL;
MOFMesh *weapon2=NULL;
MOFMesh *weapon3=NULL;
MOFMesh *weapon4=NULL;

std::vector<MOFMesh*> meshes;

PrimitiveDrawer primitiveDrawer;

Box box;

Timer globalTimer;
unsigned int startFrameTick;
unsigned int endFrameTick;

//Font font;
Font fontFPS;
Font cameraPositionFont;

float xPos=0;
float yPos=0;

int fps = 60;

Audio3D* camEar=NULL;
Sound3D* heli=NULL;
SoundPool* sp=NULL;
Sound* mouseEnterSound = NULL;
Sound* gunshot = NULL;
Sound* wavesSound = NULL;

/*
NetworkManager* netMan;
ServerSocket* serv;
ClientSocket* client;
NetworkQueueTS* queue;
boost::thread* server;
*/

clearsky::Timer timer;

Terrain terrain;
Terrain water;
IEffect *waterEffect;

X3DAUDIO_DISTANCE_CURVE_POINT VolumeCurvePoints[6] = 
{ 
	0.0f, 10.0f,
	0.05f, 8.0f, 
	0.1f, 3.0f, 
	0.3f, 2.0f,
	0.4f, 1.0f,
	1.0f, 0.5f 
};

X3DAUDIO_DISTANCE_CURVE VolumeCurve = { (X3DAUDIO_DISTANCE_CURVE_POINT*)&VolumeCurvePoints[0], 6 };

bool warpKeyDown = true;
bool warpMode = false;
bool godMode = true; //no collision in godmode

struct Player
{
	Vector3D speed;
};

//evil crazy sh00t0r bullet
struct Bullet
{
	Bullet()
	{
		position = Vector3D(0,0,0);
		velocity = Vector3D(0,0,0);
		life = 10.0f;
		active = true;
	}
	
	Box bullet;
	Vector3D position;
	Vector3D velocity;
	float life;
	bool active;
};

std::vector<Bullet*> bullets;
Player player;

int main()
{	
	if(init())
		return 1;

	while(app.run() && 	!keyboard->isKeyDown(VK_ESCAPE))
	{
		//globalTimer.start();
		render();
		update();
		//globalTimer.stop();
		//GLOBALS->setDeltaTime(globalTimer.getDuration());
	}

	shutdown();

	return 0;
}

PointLight light;
PointLight light2;
DirLight backLight;
Fog fog;

Window *dialog1;

/*Window *myWindow;
Window *myWindow2;
Window *myWindow3;
Window *myWindow4;*/

class MouseListener : public IMouseListener
{
	public:
		void mouseMove(MouseMoveEvent mouseEvent)
		{
			//clearsky::LOG->logMsg(clearsky::LT_STATUS, "\nMouselistener: Mouse was moved: X=%d Y=%d XAbs=%d, YAbs=%d\n", mouseEvent.x, mouseEvent.y, mouseEvent.xAbs, mouseEvent.yAbs);
		}

		void mouseKey(MouseKeyEvent mouseEvent)
		{
			//clearsky::LOG->logMsg(clearsky::LT_STATUS, "\nMouselistener: MouseKey was pressed\n");
		}

		void mouseClicked(MouseKeyEvent mouseEvent)
		{
			//check the warp button
			Button* button = (Button*)mouseEvent.window;
			if(button->getIdString()=="WARP_BUTTON")
			{
					warpMode = !warpMode;
					char caption[50];
					if(warpMode)
						strncpy_s(caption,"ON",49);
					else
						strncpy_s(caption,"OFF",49);

					Button *sender = (Button*)mouseEvent.window;
					sender->setCaption(caption);
			}
			else if(button->getIdString()=="SKIN_BUTTON")
			{
				if(gui->deleteSkin("Stylesheet1")!=RETURN_OK)
					clearsky::LOG->logMsg(clearsky::LT_STATUS, "\nerror deleting stylesheet\n");

				if(gui->loadSkin("..\\data\\gui\\stylesheet.gui")!=RETURN_OK)
					clearsky::LOG->logMsg(clearsky::LT_ERROR, "error reloading stylesheet\n");

				gui->setCurrentSkin("Stylesheet1");
			}
		}


		void mouseEnter(MouseEvent mouseEvent)
		{
			//play some sound
			if(mouseEnterSound->isRunning())
				mouseEnterSound->stop();

			if (!sp->loadSound(L"..\\data\\sounds\\Effects\\explo.ogg", FT_OGG, mouseEnterSound))
				clearsky::LOG->logMsg(clearsky::LT_ERROR, "eplosion_2 mousenter Sound NOT loaded!");
			mouseEnterSound->play();
		}

		void mouseLeave(MouseEvent mouseEvent)
		{
			//mouseEnterSound->stop();
		}

};

class WindowListener : public IWindowListener
{
	public:
		void windowMove(WindowEvent windowEvent)
		{
			clearsky::LOG->logMsg(clearsky::LT_STATUS, "\nWindow listener: Window was moved\n");
		}
};

//object terrain collision
bool terrainCollision(Terrain *terrain, float objX, float objZ, float objY, float objHeight)
{
	double height;
	if(terrain->getHeight((double)objX, (double)objZ, height)==RETURN_OK)
	{
		if((objY-height)<objHeight)
			return RETURN_OK;
	}

	return RETURN_ERROR;
}

//TODO: proper init
int init()
{
	RETURN_VALUE result;

	//app.init();

	clearsky::LOG->logMsg(clearsky::LT_STATUS, "Init Engine ...\n");

	//app.setCameraKeyboard(true);
	//app.setCameraMouse(true);

	renderer = app.getRenderer(RENDER_DX11);	
	//renderer->setFullScreen(false);
	//renderer->setMultisampling(1,0);
	clearsky::StartParams engineParams;
	if(engineParams.load("..\\data\\startparam.config")!=RETURN_OK)
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "cant load startparam.config");

	result = renderer->create(&engineParams);
	if(result!=RETURN_OK)
	{
		clearsky::LOG->logMsg(clearsky::LT_ERROR, getReturnMessage(result));
		clearsky::LOG->logMsg(clearsky::LT_STATUS, "Install current DX Runtime");
	}

	clearsky::LOG->logMsg(clearsky::LT_STATUS, "feature level: %s",renderer->getFeatureLevel());
	clearsky::LOG->logMsg(clearsky::LT_STATUS, "deviceType: %s", renderer->getDriverType());

	if(!renderer)
	{
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "Can't create DX Renderer");
		return 1;
	}

	//renderer->setViewport(10,10,100,100);
	keyboard = app.getKeyboard(); 
	mouse = app.getMouseHandler();

	app.setConstFPS(60);

	if(primitiveDrawer.init(50)!=RETURN_OK)
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "Can't create primitive drawer");

	primitiveDrawer.addLine(Vector3D(1,1,1),Vector3D(0,1000,0),Color(1.0f, 0.0f, 0.0f, 1.0f));
	primitiveDrawer.addLine(Vector3D(2,2,2),Vector3D(2000,2000,2000),Color(0.0f, 1.0f, 0.0f, 1.0f));
	primitiveDrawer.commitLineBuffer();

	primitiveDrawer.addFilledRect(Vector3D(0,0,0), 100,100,Color(0.0f, 0.0f, 1.0f, 1.0f));
	primitiveDrawer.commitFilledRectBuffer();

	/*DepthStencilState *depthStencilState;
	depthStencilState = renderer->getDepthStencilState();
	depthStencilState->depthEnable = true;
	renderer->setDepthStencilState(depthStencilState);*/

	terrainLayer1 = renderer->createTexture2D();
	terrainLayer2 = renderer->createTexture2D();

	//char data[516*156];
	//memset(data,100,516*516);
	//textureBox2->create(data,516,516,FORMAT_R8G8B8A8_UINT,false);
	terrainLayer1->loadFile("..\\data\\textures\\ground.jpg");
	terrainLayer2->loadFile("..\\data\\textures\\layer2_1.bmp");

	terrain.setScaleFactors(1.0f,0.3f,1.0f);
	terrain.setLOD(1);
	if(terrain.loadFromFile("..\\data\\textures\\terrain.hmp",1024,1024)!=RETURN_OK)
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "can't load terrain");
	terrain.setChunkSize(32,32);
	//terrain.generateFlat(1024,1024,50);
	terrain.setDetailMap(terrainLayer1);
	terrain.setColorMap(terrainLayer2);
	terrain.setDetailMap(terrainLayer1);
	terrain.create();

	//terrainTex = renderer->createTexture2D();
	//terrainTex->loadFile("..\\data\\textures\\water1.jpg");

	//treeTex = Resources->loadTexture2D("..\\data\\textures\\box.jpg");

	/*renderer->setTexturing(true);
	renderer->setLighting(true);
	renderer->setBlending(false);
	renderer->setFogging(false);*/
	renderer->setTexturing(true);
	renderer->setLighting(true);
	renderer->setFogging(false);
	renderer->setBlending(false);
	
	//renderer->setWireframe(false);
	//renderer->setCullOrdnerClockwise(truF
	//mouse->showWinCursor(true);
	//renderer->setWireframe(true);

	fog.setType(FT_EXPONENT);   
	//fog.setColor(Color(0.7f,0.8f,0.71f,1.0f));
	fog.setColor(Color(1,1,1,1));
	fog.setDimensions(0,1000);
	fog.setDensity(0.010f);
	//renderer->setFog(&fog);
	renderer->setFog(&fog);

	if(skybox.create(renderer->getCamera())!=RETURN_OK)
	{
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "skybox init");
		return 0;
	}

	result = skybox.setTextures("..\\data\\textures\\skybox\\north.png","..\\data\\textures\\skybox\\south.png",
						"..\\data\\textures\\skybox\\west.png", "..\\data\\textures\\skybox\\east.png",
						"..\\data\\textures\\skybox\\up.png", "..\\data\\textures\\skybox\\down.png");
	
	if(result!=RETURN_OK)
	{
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "Can't load skybox texture");
	}


	//load meshes
	MOFMesh *object = Resources->loadObject("..\\data\\objects\\industrial3\\ind3_building1_lod0.mof");
	if(object)
	{
		object->scale(4.0f,4.0f,4.0f);
		object->translate(-30,43,160);
		meshes.push_back(object);
	}

	/*object = Resources->loadObject("..\\data\\objects\\medieval\\villager_free.mof");
	if(object)
	{
		object->scale(2.0f,2.0f,2.0f);
		object->rotateY(55);
		object->translate(-55,43,156);
		meshes.push_back(object);
	}

	object = Resources->loadObject("..\\data\\objects\\medieval\\f1_barrel.mof");
	if(object)
	{
		object->scale(4.0f,4.0f,4.0f);
		object->translate(-55,43,140);
		meshes.push_back(object);
	}

	object = Resources->loadObject("..\\data\\objects\\medieval\\haywagon.mof");
	if(object)
	{
		object->scale(5.0f,5.0f,5.0f);
		object->rotateY(140);
		object->translate(-85,41,160);
		meshes.push_back(object);
	}

	object = Resources->loadObject("..\\data\\objects\\medieval\\booth01_LOD01.mof");
	if(object)
	{
		object->scale(5.0f,5.0f,5.0f);
		object->rotateY(-50);
		object->translate(-57,43,180);
		meshes.push_back(object);
	}*/

	/*object = Resources->loadObject("..\\data\\objects\\medieval\\flower_6.mof");
	if(object)
	{
		object->scale(5.0f,5.0f,5.0f);
		//object->rotateY(-50);
		object->translate(-70,41,156);
		meshes.push_back(object);
	}

	object = Resources->loadObject("..\\data\\objects\\medieval\\flower_10.mof");
	if(object)
	{
		object->scale(5.0f,5.0f,5.0f);
		//object->rotateY(-50);
		object->translate(-65,41,156);
		meshes.push_back(object);
	}
	object = Resources->loadObject("..\\data\\objects\\medieval\\litGr.mof");
	if(object)
	{
		object->scale(5.0f,5.0f,5.0f);
		//object->rotateY(-50);
		object->translate(-42,41,175);
		meshes.push_back(object);
	}*/

	//end load meshes

	//vehicle = Resources->loadObject("..\\data\\objects\\heli.mof");
	/*vehicle = Resources->loadObject("..\\data\\objects\\mi-2\\mi-2.mof");
	if(!vehicle)
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "can't load vehicle");*/

	/*testMesh = Resources->loadObject("..\\data\\objects\\ARifle\\ARifle.mof");
	if(!testMesh)
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "can't load testMesh");
	else
	{
	}*/

	weapon1 = Resources->loadObject("..\\data\\objects\\pistol.mof");
	if(weapon1==NULL)
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "can't load testMesh weapon1");

	//weapon2 = NULL;//Resources->loadObject(/*L"..\\data\\objects\\pistol.mof"*/"..\\data\\objects\\ARifle\\ARifle.mof");
	/*if(weapon2==NULL)
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "can't load testMesh weapon2");
	else
		weapon2->translate(-10, 50, 50);
	*/

	/*weapon3 = Resources->loadObject("..\\data\\objects\\uzi.mof");
	if(!weapon3)
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "can't load testMesh lazer");
	{
		weapon3->translate(20, 20, 50);
	}

	weapon4 = new MOFMesh();
	if(!weapon4)
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "can't load testMesh uzi");
	weapon4->load("..\\data\\objects\\smach.mof");//Resources->loadObject(L"..\\data\\objects\\uzi.mof");

	tunnel = Resources->loadObject("..\\data\\objects\\tunnel.mof");
	if(!tunnel)
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "can't load tunnel");*/

	/*torret = Resources->loadObject("..\\data\\objects\\copter\\copter.mof");
	if(!torret)
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "can't load torret");
	else
	{
		torret->scale(10,10,10);
		torret->translate(-65,50,0);
	}*/

	
	/*spaceship = Resources->loadObject("..\\data\\objects\\tank\\Tank.mof");
	if(!spaceship)
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "can't load tunnel");

	weapon4->translate(30, 20, 50);
	testMesh->translate(-10, 10, 50);*/
	//vehicle->scale(20,20,20);
	//vehicle->translate(-500,80,50);
	
	//tunnel->scale(20,40,40);
	//tunnel->rotateY(-90);
	//tunnel->translate(-10,15,100);

	//torret->scale(10,10,10);
	//torret->rotateY(180);
	//torret->translate(-65,50,0);

	//spaceship->scale(0.2f,0.2f,0.2f);
	//spaceship->rotateY(180);

	//spaceship->translate(40,30,100);

	//vadertie->scale(-10.0f,10.0f,10.0f);
	//vadertie->translate(-50,20,50);
	weaponSprite = renderer->createSprite();
	
	//weaponSprite->init(L"..\\data\\sprites\\explosion2.png",64,64,25,5,5);
	//weaponSprite->init(L"..\\data\\sprites\\explosion3.png",64,64,16,4,4);
	if(weaponSprite->init("..\\data\\sprites\\explosion1.png",75,60,18,4,5)!=RETURN_OK)
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "can't load weapon sprite");

	weaponSprite->scale(3,3,3);
	weaponSprite->translate(0, 20, 50);

	//get terrain data to show it in a texture
	unsigned char *terrainData = terrain.getHeightmap();
	heightmapTex = renderer->createTexture2D();
	heightmapTex->setPitch(1024);
	//result = heightmapTex->create(terrainData, 1024, 1024,  FORMAT_R8G8B8A8_UNORM/*FORMAT_R32G32B32A32_FLOAT*//*FORMAT_A8_UNORM*//*FORMAT_R8_TYPELESS*/,true);
	result = heightmapTex->createRenderTarget(1024, 1024,  true, FORMAT_R8G8B8A8_UNORM);
	if(result!=RETURN_OK)
	{
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "create heightmap Texture");
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "  message: %s", getReturnMessage(result));
	}
	
	IEffect *waterEffect = renderer->createEffect();
	waterEffect->create("..\\data\\shaders\\water.fx");
	water.generateFlat(1024,1024,0);
	water.setChunkSize(32,32);
	water.setDetailMap(terrainTex);
	//water.create();
	//water.setEffect(waterEffect);

	box.create();
	//testMesh->scale(100,100,100);
	//testMesh->translate(0, 30, 50);

	/*shotgun.scale(0.1f,0.1f,0.1f);
	//shotgun.rotateY(-90);
	shotgun.translate(0,0,0);
	
	vadertie->scale(0.3f,0.3f,0.3f);
	vadertie->translate(-50,50,150);
	
	temple.scale(10,10,10);
	temple.translate(100,-15,50);
	*/
	light.create(Vector3D(-10, 0.5, -10), 
					Vector3D(0,-1,0), 
					D3DXCOLOR(1.0f, 1, 1, 1.0f), 
					D3DXCOLOR(1.0f, 1, 1, 1.0f), 
					D3DXCOLOR(1.0f, 1, 1, 1.0f),
					100000.0f);
	light.setAtt(0.0f,0.2f,0.0f);

	light2.create(Vector3D(-20, 1, -20),
					Vector3D(0,1,0), 
					D3DXCOLOR(0.0f, 1, 1, 1.0f), 
					D3DXCOLOR(1.0f, 1, 1, 1.0f), 
					D3DXCOLOR(1.0f, 1, 1, 1.0f),
					10000.0f);
	light2.setAtt(0.0f,0.2f,0.0f);

	Vector3D dir = Vector3D(0.55f, -0.55f, 0.55f);
	//Vector3D dir = Vector3D(0.0f, -1.0f, 0.0f);
	//D3DXVec3Normalize(&dir,&dir);
	backLight.create(dir,
					Color(0.0f, 0.0f, 0.0f, 1.0f),
					Color(0.5f,0.5f,0.5f,1),
					Color(1.0f,1.0f,1.0f,1));
	 //renderer->setLight(0, (Light*)&backLight);
	renderer->setLight(0, (Light*)&backLight);

	Material material;
	material.ambient  = Color(0.9f,0.9f,0.9f,1.0f);
	material.diffuse  = Color(0.9f,0.9f,0.9f,1.0f);
	material.specular = Color(0.9f,0.9f,0.9f,1.0f);

	//renderer->setMaterial(material);
	renderer->setMaterial(material);

	gui = app.getGUI();

	camEar = new Audio3D();

	if (camEar->initialiseAudio())	
	{		
		// set to 1000?
		camEar->setCurveDistanceScaler(1300.0f);

		camEar->setReverb(12);
		camEar->setListenerPosition(D3DXVECTOR3(0,15,0));
		Sound* test = new Sound(camEar);
		
		mouseEnterSound = new Sound(camEar);
		gunshot = new Sound(camEar);
		wavesSound = new Sound(camEar);
		wavesSound->setLoop(true);

		heli = new Sound3D(camEar, 1);
		//heli->setVolumeCurve(&VolumeCurve);
		heli->setEmitterPos(D3DXVECTOR3(0, 15, 0));
		heli->setDopplerScaler(1.0f);

		sp = new SoundPool();

		gunshot->setLoop(false);
		test->setLoop();
		heli->setLoop();

		if (!sp->loadSound(L"..\\data\\sounds\\test"))
			clearsky::LOG->logMsg(clearsky::LT_ERROR, "heli Sound NOT loaded!");

		if (!sp->loadSound(L"..\\data\\sounds\\Nature Ambiente\\wind.ogg", test))
			clearsky::LOG->logMsg(clearsky::LT_ERROR, "mad Sound NOT loaded!");

		if (!sp->loadSound(L"..\\data\\sounds\\Effects\\explo.ogg", mouseEnterSound))
			clearsky::LOG->logMsg(clearsky::LT_ERROR, "eplosion_2 mousenter Sound NOT loaded!");

		if (!sp->loadSound(L"..\\data\\sounds\\test", heli))
			clearsky::LOG->logMsg(clearsky::LT_ERROR, "heli Sound NOT loaded!");

		if (!sp->loadSound(L"..\\data\\sounds\\Weapons\\uzi.wav", gunshot))
			clearsky::LOG->logMsg(clearsky::LT_ERROR, "gunshot Sound NOT loaded!");


		if (!sp->loadSound(L"..\\data\\sounds\\oceanwave.oga", wavesSound))
			clearsky::LOG->logMsg(clearsky::LT_ERROR, "wavesSound Sound NOT loaded!");

		/*
		ovl = new OggVorbisLoader();
		ovl->setTargetSound(heli);
		if (!ovl->loadStaticSound("data\\sounds\\test.ogg"))
			clearsky::LOG->logMsg(clearsky::LT_ERROR, "Sound NOT loaded!");
			*/

		//checked 28.12.2010, works perfectly:
		//if (!heli->calculateEmitterEcho(150, 1))
			//clearsky::LOG->logMsg(clearsky::LT_ERROR, "Echo not set!");
		
		//ovl->submitStaticSound();
		test->alterVolume(1.0f);
		test->play();
		//heli->play();
		//wavesSound->play();

	}
	else
	{
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "Sound init failed!");
	}

	/*
	netMan = new NetworkManager();
	queue = new NetworkQueueTS();
	if (netMan->isInitialized())
	{
		serv = new ServerSocket(*netMan, 9666);
		if (serv->isListening())
		{
			LOG->logMsg(LT_STATUS, "Server is listening on port 9666");
			server = new boost::thread(boost::bind(&ServerSocket::fillQueue, serv, *queue));
			//(*server).detach();
		}
		else
		{
			LOG->logMsg(LT_ERROR, "Sock init failed!");
		}
	}
	else
	{
		LOG->logMsg(LT_ERROR, "Network init failed!");
	}
	*/
	//========BEGIN GUI TESTING STUFF

	if(gui->loadSkin("..\\data\\gui\\stylesheet.gui")!=RETURN_OK)
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "can't load gui skin!");

	if(gui->setCurrentSkin("Stylesheet1")!=RETURN_OK)
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "Stylesheet1 not found!");

	Window *topWindow = gui->getTopWindow();
	
	dialog1 = gui->createWindow();
	dialog1->setVisible(false);
	dialog1->setCaption("MAD Window Test");
	dialog1->setPosition(50,70);
	dialog1->setSize(400,500);

	StaticText *warpText = gui->createStaticText();
	warpText->setPosition(10,50);
	warpText->setCaption("Activate WARP: ");
	dialog1->addChild(warpText);

	Button *warpButton = gui->createButton();
	warpButton->setPosition(200,50);
	//warpButton->setType(GUI_TYPE_PUSHBUTTON);
	warpButton->setIdString("WARP_BUTTON");
	warpButton->setCaption("OFF");
	warpButton->addMouseListener(new MouseListener);
	dialog1->addChild(warpButton);

	StaticText *skinText = gui->createStaticText();
	skinText->setPosition(10, 110);
	skinText->setCaption("Reload Styleshit: ");
	dialog1->addChild(skinText);

	Button *skinButton = gui->createButton();
	skinButton->setPosition(200, 110);
	skinButton->setIdString("SKIN_BUTTON");
	skinButton->setCaption("RELOAD");
	skinButton->addMouseListener(new MouseListener);
	dialog1->addChild(skinButton);

	topWindow->addChild(dialog1);

	//gui->detach();

	//gui->setTopWindow(myWindow);
	//========END GUI TESTING STUFF


	/*font.setSize(20,20);
	if(font.load("..\\data\\fonts\\automati.ttf")!=RETURN_OK)
		clearsky::LOG->logMsg(clearsky::LT_STATUS, "Weird font error");

	
	font.setString(255, "Evil Freetype String0r");*/

	Gradient gradient;
	gradient.startColor = Color(0.9f,0.9f,0.9f,1);
	gradient.endColor   = Color(0.9f,0.9f,0.9f,1.0f);
	//font.setGradient(gradient);
	//font.setSize(20,20);

	fontFPS.setSize(16,16);
	fontFPS.load("..\\data\\fonts\\arial.ttf");
	fontFPS.setGradient(gradient);
	//font.setString(255, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!?0123456789");
	cameraPositionFont.setSize(12,12);
	cameraPositionFont.load("..\\data\\fonts\\arial.ttf");
	cameraPositionFont.setGradient(gradient);

	camera = renderer->getCamera();

	if(godMode)
	{
		camera->position.y = 50;
		camera->lookDir.y  = 50;
	}
	else
	{
		camera->position.y = 350;
		camera->lookDir.y  = 350;
	}

	/*IEffect *newEffect = renderer->createEffect();
	if(newEffect->create(DEFAULT_EFFECT_FILE)!=RETURN_OK)
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "cann't create effect file\n");
	if(renderer->setEffect(newEffect)!=RETURN_OK)
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "cann't create effect file\n");
	IInputLayout *newInputLayout = renderer->createInputLayout();
	newInputLayout->setEffect(newEffect);
	if(newInputLayout->create(VF_DEFAULT)!=RETURN_OK)
		clearsky::LOG->logMsg(clearsky::LT_ERROR, "cann't create input layout\n");
	newInputLayout->activate();
	renderer->setInputLayout(newInputLayout);
	*/

	player.speed = Vector3D(50,0,50);

	printf("\nsizeof Vertex: %d\n", sizeof(Vertex));
	clearsky::LOG->logMsg(clearsky::LT_STATUS, "Engine Initialised\n");
	/*
	client = new ClientSocket(*netMan);
	client->initiate(9666, inet_addr("127.0.0.1"));
	*/

	//AABB test
	Vertex vertex[10];
	vertex[0].position = Vector3D(1,2,3);
	vertex[1].position = Vector3D(-1.001f,2.23f,3.3f);
	vertex[2].position = Vector3D(1,2,3);
	vertex[3].position = Vector3D(-3,2,3);
	vertex[4].position = Vector3D(1,2,3);
	vertex[5].position = Vector3D(-3.001f,2.0f,3.0f);
	vertex[6].position = Vector3D(134.0f,2.0f,3.0f);
	vertex[7].position = Vector3D(123.34f,2.0f,3.0f);
	vertex[8].position = Vector3D(1,2.45f,3);
	vertex[9].position = Vector3D(1,223,3);

	AABB aabb;
	aabb.loadFromPosition(vertex,10);
	//printf("aabb test min: %f %f %f\n", aabb.min.x, aabb.min.y, aabb.min.z);
	//printf("aabb test max: %f %f %f\n", aabb.max.x, aabb.max.y, aabb.max.z);
	LOG->logMsg(LT_STATUS, "Init complete");
	return 0;
}

float mTheta = 0;
float mPhi = (float)D3DX_PI * 0.25f;

float rot;
float col;
int fac = 1;

float lX, lY, lZ;

float lineX = 0;

int i=0;
int turnFactor = 1;

float scaling = 1;
float scaleFactor = 0.01f;

bool playWeaponAnim=false;

float weaponRot = 0.0f;
float weaponScale =1.0f;

void render()
{
	double delta = globalTimer.getDelta();

	/*
	*TODO: Make some dynamic buffer tests... e.g. simple water or stuff like that
	*/
	renderer->begin(true, true);	
	//renderer->clear(1.0,0.0,1.0f);

	/*renderer->saveWorld();
		CULL_MODE cullMode = renderer->setCullMode(CULL_FRONT);
		renderer->setLighting(false);
		renderer->apply(0,0);
		//renderer->setTexture(textureBox2);
		*/
	renderer->setBlending(false);
	skybox.draw();
	/*
		renderer->setLighting(true);
		renderer->setCullMode(cullMode);	
	renderer->restoreWorld();*/

	//testMesh->draw();
	if(weapon1)
		weapon1->draw();
	
	if(weapon2)
	{
		weapon2->resetTransform();
		weapon2->rotateY(weaponRot);
		weapon2->scale(weaponScale, weaponScale, weaponScale);
		weapon2->translate(-15, 45, 50);
		if(weaponRot>360)
			weaponRot=0;
		if((weaponScale>2.0f) || (weaponScale<1.0f))
		{
			scaling = -scaling;
		}

		//printf("delta: %f\n",7*delta);
		weaponRot += 200.0f*delta;
		weaponScale += scaling*0.25*delta;
		weapon2->draw();
	}
	//weapon3->draw();
	//weapon4->draw();
	if(vehicle)
		vehicle->draw();
	//testMesh->draw();
	//tunnel->draw();
	if(torret)
		torret->draw();
	//spaceship->draw();

	///renderer->setTexture(textureBox2);
	//terrain.setTexture(NULL);
	
	//terrain.draw(&viewFrustum);

	//renderer->setCullMode(CULL_MODE_NONE);
	//water.draw(&viewFrustum);
	//renderer->setCullMode(CULL_MODE_BACK);

	/*renderer->saveWorld();
		renderer->setBlending(false);
		renderer->setTexture(heightmapTex);
		renderer->scale(10,10,10);
		renderer->translate(10,30,65);
		renderer->apply(0,0);
		box.draw();
		renderer->setBlending(true);
	renderer->restoreWorld();
	*/
	
	//renderer->setBlending(false);

	renderer->setBlending(false);
	for(int i=0; i<meshes.size(); ++i)
	{
		meshes[i]->draw();
	}

	fps = timer.getFPS();

	renderer->setBlending(true);
	//renderer->setBlending(true);
	fontFPS.setString(255, "FPS %d", fps);
	fontFPS.show(10,10);

	cameraPositionFont.setString(255,"Camera Pos. %f %f %f",camera->position.x, camera->position.y, camera->position.z);
	cameraPositionFont.show(10,35);
	//renderer->setBlending(false);

	//primitiveDrawer.show();

	//gui->show();


	/*heightmapTex->startRenderTarget();

	skybox.draw();
	weapon1->draw();
	
	weapon2->draw();
	weapon3->draw();
	weapon4->draw();
	vehicle->draw();
	//testMesh->draw();
	tunnel->draw();
	torret->draw();
	spaceship->draw();

	//renderer->setTexture(textureBox2);

	water.draw(&viewFrustum);
	weaponSprite->show();


	fps = timer.getFPS();

	fontFPS.setString(255, "FPS %d", fps);
	fontFPS.show(10,10);

	cameraPositionFont.setString(255,"Camera Pos. %f %f %f",camera->position.x, camera->position.y, camera->position.z);
	cameraPositionFont.show(10,35);

	gui->show();

	heightmapTex->endRenderTarget();*/

	renderer->setBlending(false);
	terrain.draw(&viewFrustum);

	renderer->setBlending(true);
	weaponSprite->show();

	renderer->setBlending(true);
	gui->show();

	renderer->end();
}

float x,y,z;
float speed;

int mouseX=0;
int mouseY=0;
int oldMouseX = WIDTH>>1;
int oldMouseY = HEIGHT>>1;

float vehicleRotorRotation=0;
float vehicleBodyRotation=0;

Vector3D gravity = Vector3D(0.0f,-4.8f,0.0f);
Vector3D gravityVel = Vector3D(0.0f, 0.0f, 0.0f);

//Vector3D camVelocity = Vector3D(2.0f, 2.0f, 2.0f);

unsigned long startWeaponShot = 0;
unsigned long endWeaponShot = 0;

unsigned long startWeaponAnim = 0;
unsigned long endWeaponAnim = 0;

float gameTime = 0.0f;
void update()
{
	double delta = globalTimer.getDelta();

	Vector3D curDir = backLight.getDirection();
	curDir.x = 50*sinf(gameTime);
	//curDir.y = 50*sinf(gameTime);
	curDir.z = 50*cosf(gameTime);
	backLight.setDirection(curDir);
	//renderer->setLight(0, (Light*)&backLight);

	gameTime += 0.01f;

	//shotgun.resetTransform()
	//terrain collision
	if(!godMode)
	{
		double terrainHeight;
		if(terrain.getHeight(camera->position.x, camera->position.z, terrainHeight)==RETURN_OK)
		{
			if(camera->position.y>(12+terrainHeight) || camera->position.y<(-12+terrainHeight))
			{
				if(!godMode)
				{
					gravityVel += gravity;
					//move due gravity
					camera->move(gravityVel);
				}
			}
			else if((camera->position.y-terrainHeight)<10 && (camera->position.y-terrainHeight)>-10)
			{
				gravityVel.y = 0.0f;

				//camera->move(CM_UP, 2.0f);
				camera->position.y = (float)terrainHeight+10.0f;
				camera->lookDir.y = (float)terrainHeight+10.0f; 
			}
		}
		else
		{
			if(!godMode)
			{
				gravityVel += gravity;
				camera->move(gravityVel);
			}
		}
	}

	//printf("cam posx = %f, posy = %f\n", camera->position.x, camera->position.y);

	//update bullets
	/*for(unsigned int i=0; i<bullets.size(); ++i)
	{
		if(bullets[i]->life<0.001f)
			bullets[i]->active = false;

		if(bullets[i]->active)
		{
			bullets[i]->life -= (float)delta;
			
			if(terrain.getHeight(bullets[i]->position.x, bullets[i]->position.z, terrainHeight)==RETURN_OK)
			{
				if((bullets[i]->position.y-terrainHeight)<5 && (bullets[i]->position.y-terrainHeight)>-5)
				{
					//camera->move(CM_UP, 2.0f);
					bullets[i]->velocity = Vector3D(0,0,0);
					bullets[i]->position.y = (float)terrainHeight+5.0f;
				}
				{
					bullets[i]->velocity += gravity*(float)delta;
				}
			}
		}
		else
		{
			bullets.erase(bullets.begin()+i);
		}
	}*/

	if(playWeaponAnim && (weaponSprite->getCurrentFrame()<weaponSprite->getNumFrames()-1))
	{
		//printf("cur: %d, num: %d\n",weaponSprite->getCurrentFrame(),weaponSprite->getNumFrames());
		endWeaponAnim = Timer::getTickCount();
		//if((endWeaponAnim-startWeaponAnim)>(5000*delta))
		if((endWeaponAnim-10)>startWeaponAnim)
		{
			startWeaponAnim=endWeaponAnim;
			endWeaponAnim=0;
			weaponSprite->nextFrame();
		}
	}
	else
	{
		//printf("stop\n");
		playWeaponAnim=false;
		weaponSprite->setVisible(false);
		if(weaponSprite->getCurrentFrame()>=(weaponSprite->getNumFrames()-1))
		{
			weaponSprite->nextFrame();
		}
	}

	/*if(keyboard->keyClicked(VK_SPACE))
	{
		if(!playWeaponAnim)
		{
			weaponSprite->resetTransform();
			weaponSprite->scale(0.5,0.5,0.5);
			weaponSprite->transform(&camera->getRotationMatrix());
			Vector3D trans = camera->position-1.1*camera->rightDir+4.1*camera->getLookDir()-camera->up;
			weaponSprite->translate(trans.x, trans.y, trans.z);
		}
	}*/

	//shoot -> create bullet
	if(keyboard->isKeyDown(VK_SPACE))
	{
		playWeaponAnim = true;
		weaponSprite->setVisible(true);
		/*Bullet *bullet = new Bullet;
		bullet->bullet.create();
		
		Vector3D camLook = renderer->getCamera()->lookDir - renderer->getCamera()->position;
		D3DXVec3Normalize(&camLook, &camLook);

		bullet->position = Vector3D(camera->position.x, camera->position.y-2.5f, camera->position.z)+camLook*25;
		bullet->velocity = Vector3D(camLook.x,camLook.y,camLook.z)*300*(float)delta;

		bullets.push_back(bullet);
		*/

		endWeaponShot = globalTimer.getTime();
		//if((endWeaponShot-startWeaponShot)>(12000*delta))
		if((endWeaponShot-100)>startWeaponShot)
		{
			//weaponSprite->nextFrame();
			startWeaponShot=endWeaponShot;
			endWeaponShot=0;
			sp->reloadSound(gunshot);
			gunshot->play();
		}
	}

	//hide/show gui
	if(keyboard->keyClicked(VK_G))
	{
		dialog1->setVisible(!dialog1->isVisible());
	}
	if(keyboard->keyClicked(VK_N))
	{
		//weaponSprite->drawAll();
		//weaponSprite->translate(camera->position.x, camera->position.y, camera->position.z);
		
		//weaponSprite->billboard(&camera->getRotationMatrix());
		//weaponSprite->translate(0, 20, 50);
		//weaponSprite->nextFrame();

		_CrtDumpMemoryLeaks();
	}

	//jump
	if(keyboard->isKeyDown(VK_E))
	{
		camera->position.y += 20;
		camera->lookDir.y  += 20;
	}

	//toggle wireframe
	if(keyboard->keyClicked(VK_R))
	{
		renderer->setWireframe(!renderer->isWireframe());
	}

	//WARP Effect
	if(keyboard->keyClicked(VK_Q))
	{
		godMode = !godMode;
	}
	else
		warpKeyDown = false;

	if(keyboard->isKeyDown(VK_W))
	{
		if(warpMode)
		{
			float curFOV = renderer->getFOV();
			if(curFOV<175)
			{
				++curFOV;
				renderer->setFOV(curFOV);
				speed = curFOV;
			}

			renderer->getCamera()->move(CM_FORWARD, speed-player.speed.z*2);
		}
		else
			renderer->getCamera()->move(CM_FORWARD, player.speed.z);
	}
	if(keyboard->isKeyDown(VK_S))
	{
		if(warpMode)
		{
			float curFOV = renderer->getFOV();
			if(curFOV>45)
			{
				--curFOV;
				renderer->setFOV(curFOV);
				speed = curFOV;
			}
		}

		renderer->getCamera()->move(CM_BACK, player.speed.z);

	}

	if(keyboard->isKeyDown(VK_A))
		renderer->getCamera()->move(CM_LEFT,player.speed.x);
	if(keyboard->isKeyDown(VK_D))
		renderer->getCamera()->move(CM_RIGHT,player.speed.x);
	if(keyboard->isKeyDown(VK_LEFT))
	{
		//weapon1->rotateY(-5);
		renderer->apply(0,0);
		renderer->getCamera()->move(CM_TURN_LEFT, 0.3f);
	}
	if(keyboard->isKeyDown(VK_RIGHT))
	{
		//weapon1->rotateY(5);
		renderer->apply(0,0);
		renderer->getCamera()->move(CM_TURN_RIGHT, 0.3f);
	}
	if(keyboard->isKeyDown(VK_UP))
	{
		//weapon1->rotateY(5);
		renderer->getCamera()->move(CM_TURN_UP, 0.3f);
	}
	if(keyboard->isKeyDown(VK_DOWN))
	{
		//weapon1->rotateY(-5);
		renderer->getCamera()->move(CM_TURN_DOWN, 0.3f);
	}

	if(keyboard->isKeyDown(VK_I))
	{
		++player.speed.z;

	}
	if(keyboard->isKeyDown(VK_K))
	{
		--player.speed.z;
	}
	if(keyboard->isKeyDown(VK_O))
	{
		++player.speed.x;

	}
	if(keyboard->isKeyDown(VK_L))
	{
		--player.speed.x;
	}

	//TODO: include the camera-mouse controlling into the engine
	if(mouse->isButtonDown(MOUSE_RBUTTON))
	{
		//if(mouse->isWinCursorVisible())
			//mouse->showCursor(true);
		if(gui->getMouse()->isVisible())
			gui->getMouse()->setVisible(false);		
		
		//mouse->getCurClientPos(oldMouseX, oldMouseY);

		oldMouseX = GLOBALS->getWidth()>>1;
		oldMouseY = GLOBALS->getHeight()>>1;
		mouse->getCurScreenPos(mouseX, mouseY);
		mouse->setCurScreenPos(mouseX, oldMouseY);
		//if((mouseY-oldMouseY)>0.0001f)
			renderer->getCamera()->move(CM_TURN_DOWN, (mouseY-oldMouseY)*0.1f);
		//weapon1->rotateZ((mouseY-oldMouseY)*0.1f);

		mouse->getCurScreenPos(mouseX, mouseY);
		mouse->setCurScreenPos(oldMouseX, mouseY);
		//if((mouseX-oldMouseX)>0.0001f)
			renderer->getCamera()->move(CM_TURN_RIGHT, (mouseX-oldMouseX)*0.1f);
		//weapon1->rotateY((mouseX-oldMouseX)*0.1f);
		//mouse->centerCursor();
		
		//mouse->showCursor(true);
		//app.setCameraMouse(true);
	}
	else
	{
		//if(!mouse->isWinCursorVisible())
		//	mouse->showWinCursor(true);
		if(!gui->getMouse()->isVisible())
			gui->getMouse()->setVisible(true);

		//app.setCameraMouse(false);
	}

	//Matrix camMatrix;
	//D3DXMatrixLookAtLH(&camMatrix,&camera->position, &(camera->lookDir-camera->position), &camera->up);
	//D3DXMatrixTranslation(&camMatrix,0,10,50);
	//weapon1->transform(&camMatrix);
	if(weapon1)
	{
		weapon1->resetTransform();
		Matrix rotationMatrix = camera->getRotationMatrix();
		weapon1->rotateY(-90);
		weapon1->transform(&rotationMatrix);
	
		//Vector3D trans = camera->position+(-0.9f*camera->rightDir-camera->up+3.2f*camera->getLookDir());
		//Vector3D trans=Vector3D(-0.9f,-1.0f,3.2f);
		Vector3D trans=Vector3D(-0.2f,-1.2f,0.5f);
		D3DXVec3TransformCoord(&trans,&trans,&rotationMatrix);
		trans += camera->position;
		//weapon1->rotateY(90);
		weapon1->translate(trans.x, trans.y, trans.z);
	

		weaponSprite->resetTransform();
		//weaponSprite->scale(0.5f,0.5f,0.5f);
		weaponSprite->transform(&rotationMatrix);
		trans = camera->position-0.8f*camera->rightDir+2.5f*camera->getLookDir()-camera->up;
		weaponSprite->translate(trans.x, trans.y, trans.z);
	}

	if(vehicle)
	{
		/*vehicle->resetTransform();
		vehicle->rotateX(90);
		vehicle->scale(5,5,5);
		vehicle->getSubmesh(8)->rotateY(vehicleRotorRotation);
		vehicle->translate(-500,150,100);
		vehicle->rotateY(vehicleBodyRotation);

		vehicleRotorRotation = (vehicleRotorRotation+2500*delta);
		if(vehicleRotorRotation>=360)
			vehicleRotorRotation=0;

		vehicleBodyRotation = (vehicleBodyRotation+100*delta);
		if(vehicleBodyRotation>=360)
			vehicleBodyRotation=0;*/
	}
	//vehicle->rotateY(6.0f*delta);
	/*vehicle->getSubmesh(1)->rotateY(6.0f*delta);
	vehicle->getSubmesh(0)->rotateY(6.0f*delta);
	Vector3D curPos = vehicle->getSubmesh(1)->getPosition();
	vehicle->getSubmesh(0)->translate(-curPos.x,-curPos.y,-curPos.z);
	vehicle->getSubmesh(0)->rotateY(500.0f*delta);
	vehicle->getSubmesh(0)->translate(curPos.x,curPos.y,curPos.z);*/
	//vehicle->rotateY(6.0f*delta);
	//vehicle->getSubmesh(0)->rotateY(6.0f*delta);
	//vehicle->getSubmesh(0)->translate(500,-80,-50);
	//vehicle->getSubmesh(0)->resetTransform();
	//vehicle->getSubmesh(0)->rotateY(100.0f*delta);
	//vehicle->getSubmesh(1)->rotateY(6.0f*delta);
	
	
	//weapon1->setRotation(camera->rotation.x, camera->rotation.y, camera->rotation.z);
	//weapon1->rotateY(180);
	//weapon1->setPosition(renderer->getCamera()->position.x, renderer->getCamera()->position.y-5, renderer->getCamera()->position.z+10);

	//=====GUI TEST STUFF
	gui->update();
	//=====GUI TEST STUFF

	/*camEar->setListenerPosition(renderer->getCamera()->position);
	//if(vehicle)
	//	heli->setEmitterPos(vehicle->getSubmesh(1)->getPosition());
	D3DXVECTOR3 vDelta = renderer->getCamera()->lookDir - renderer->getCamera()->position;
	D3DXVec3Normalize(&vDelta, &vDelta);
	camEar->setOrientFront(vDelta);

	heli->calculate((float)delta);
	*/
	viewFrustum.build(renderer->getViewTransformation(), renderer->getProjection());

	if(vehicle)
	{
		AABB aabb = vehicle->getBoudingBox();
	//printf("aabb pos min: %f %f %f\n",aabb.min.x, aabb.min.y, aabb.min.z);
	//printf("aabb pos max: %f %f %f\n",aabb.max.x, aabb.max.y, aabb.max.z);
		if(aabb.pointInside(camera->position.x, camera->position.y, camera->position.z))
			printf("collision\n");
	}

	//app.makeConstFPS();

	app.update();
}

void shutdown()
{
	clearsky::LOG->logMsg(clearsky::LT_STATUS, "Shutdown engine...\n");

	//SAFE_DELETE(vadertie);
	//temple.release();
	//shotgun.release();
	/*
	LOG->logMsg(LT_STATUS, "Stop client queuing...");
	client->stopQueuing();
	LOG->logMsg(LT_STATUS, "Stop server queuing...");
	serv->stopQueuing();
	LOG->logMsg(LT_STATUS, "Interrupting thread...");
	server->interrupt();
	LOG->logMsg(LT_STATUS, "Joining thread...");
	server->join();
	SAFE_DELETE(server);
	SAFE_DELETE(serv);
	SAFE_DELETE(client);
	SAFE_DELETE(netMan);
	*/
	//SAFE_DELETE(sp);
	//fontFPS.release();
	//TODO: make proper release
	//app.release();

	//LOG->release();
}
