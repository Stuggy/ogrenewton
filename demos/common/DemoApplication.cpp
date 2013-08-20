
/* Copyright (c) <2003-2011> <Julio Jerez, Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 
* 3. This notice may not be removed or altered from any source distribution.
*/


#include "HelpersStdAfx.h"

#include "MouseCursor.h"
#include "ScreenWriter.h"
#include "ShootRigidBody.h"
#include "DemoApplication.h"


#define FREE_CAMERA_SPEED 0.25f
#define FREE_CAMERA_YAW_SPEED (1.0f * 3.141592f / 180.0f)
#define FREE_CAMERA_PITCH_SPEED (1.0f * 3.141592f / 180.0f)

DemoApplication::SmoothCamera::SmoothCamera()
	:dNewtonTransformLerp()
	,m_cameraYawAngle(0.0f)
	,m_cameraPitchAngle(0.0f)
	,m_cameraTranslation(0.0f, 0.0f, 0.0f)
{
}

void DemoApplication::SmoothCamera::Move (Real deltaTranslation, Real deltaStrafe, Radian pitchAngleStep, Radian yawAngleStep)
{
	// here we update the camera movement at simulation rate
	m_cameraYawAngle = fmodf (m_cameraYawAngle.valueRadians() + yawAngleStep.valueRadians(), 3.141592f * 2.0f);
	m_cameraPitchAngle = Math::Clamp (m_cameraPitchAngle.valueRadians() + pitchAngleStep.valueRadians(), - 80.0f * 3.141592f / 180.0f, 80.0f * 3.141592f / 180.0f);

	Matrix3 rot; 
	rot.FromEulerAnglesZYX (Radian (0.0f), m_cameraYawAngle, m_cameraPitchAngle);
	Matrix4 matrix (rot);
	m_cameraTranslation += Vector3 (matrix[0][2], matrix[1][2], matrix[2][2]) * deltaTranslation;   
	m_cameraTranslation += Vector3 (matrix[0][0], matrix[1][0], matrix[2][0]) * deltaStrafe;   

	matrix.setTrans(m_cameraTranslation);
	matrix = matrix.transpose();
	Update (matrix[0]);
}

void DemoApplication::SmoothCamera::Reset (const Vector3& posit, const Quaternion& rotation)
{
	Matrix4 matrix;
	matrix.makeTransform (posit, Vector3 (1.0f, 1.0f, 1.0f), rotation);
	matrix = matrix.transpose();

	Matrix3 rot;
	matrix.extract3x3Matrix(rot);

	Radian rotX;
	Radian rotY;
	Radian rotZ;
	rot.ToEulerAnglesZYX (rotZ, rotY, rotX);

	m_cameraYawAngle = rotY;
	m_cameraPitchAngle = rotX;
	m_cameraTranslation = posit;
	ResetMatrix (&matrix[0][0]);

	Matrix4 cameraMatrix;
	InterplateMatrix (0.0f, cameraMatrix[0]);
	cameraMatrix = cameraMatrix.transpose();
}


DemoApplication::OgreNewtonExample::OgreNewtonExample (DemoApplication* const application)
	:m_application(application)
{
	GetRayPickManager()->SetCollisionMask(m_mousePick);
}

DemoApplication::OgreNewtonExample::~OgreNewtonExample()
{
}

// called asynchronous at the beginning of a physics update. 
void DemoApplication::OgreNewtonExample::OnBeginUpdate (dFloat timestepInSecunds)
{
	m_application->m_mouse->capture();
	m_application->m_keyboard->capture();
//	dNewton::ScopeLock lock (&m_application->m_scopeLock);
	m_application->OnPhysicUpdateBegin (timestepInSecunds);
}

// called asynchronous at the beginning end a physics update. 
void DemoApplication::OgreNewtonExample::OnEndUpdate (dFloat timestepInSecunds)
{
	m_application->m_mouse->capture();
	m_application->m_keyboard->capture();
//	dNewton::ScopeLock lock (&m_application->m_scopeLock);
	m_application->OnPhysicUpdateEnd (timestepInSecunds);
}

// called synchronous with ogre update loop, at the beginning of setting all node transform after a physics update  
void DemoApplication::OgreNewtonExample::OnNodesTransformBegin(Real interpolationParam)
{
	m_application->m_mouse->capture();
	m_application->m_keyboard->capture();
//	dNewton::ScopeLock lock (&m_application->m_scopeLock);
	m_application->OnRenderUpdateBegin(interpolationParam);
}

// called synchronous with ogre update loop, at the end of setting all node transform after a physics update  
void DemoApplication::OgreNewtonExample::OnNodesTransformEnd(Real interpolationParam)
{
	m_application->m_mouse->capture();
	m_application->m_keyboard->capture();
//	dNewton::ScopeLock lock (&m_application->m_scopeLock);
	m_application->OnRenderUpdateEnd(interpolationParam);
}


bool DemoApplication::OgreNewtonExample::OnBodiesAABBOverlap (const dNewtonBody* const body0, const dNewtonBody* const body1, int threadIndex) const
{
	dNewtonCollision* const collision0 = body0->GetCollision();
	dNewtonCollision* const collision1 = body1->GetCollision();

	// check if these twp collision shape are part of a hierarchical model
	void* const node0 = collision0->GetUserData();
	void* const node1 = collision1->GetUserData();
	if (node0 && node1) {
		//both collision are child nodes, check if there are self colliding
		return GetHierarchyTransformManager()->SelfCollisionTest (node0, node1);
	}

	// check all other collision using the bitfield mask, 
	//for now simple return true
	return true;
}

bool DemoApplication::OgreNewtonExample::OnCompoundSubCollisionAABBOverlap (const dNewtonBody* const body0, const dNewtonCollision* const subShape0, const dNewtonBody* const body1, const dNewtonCollision* const subShape1, int threadIndex) const
{
	return true;
}

void DemoApplication::OgreNewtonExample::OnContactProcess (dNewtonContactMaterial* const contactMaterial, dFloat timestep, int threadIndex) const
{
	OgreNewtonWorld::OnContactProcess (contactMaterial, timestep, threadIndex);
}



DemoApplication::DemoApplication()
	:OIS::MouseListener()
	,OIS::KeyListener()
	,WindowEventListener()
	,mRoot(NULL)
	,mCamera(NULL)
	,mWindow(NULL)
	,mSceneMgr(NULL)
	,m_physicsWorld(NULL)
	,m_debugRender(NULL)
	,m_ois(NULL)
	,m_mouse(NULL)
	,m_keyboard(NULL)
	,m_cursor(NULL)
	,m_screen(NULL)
	,m_onScreeHelp(true)
	,m_debugTriggerKey(false)
	,m_asyncronousUpdateKey(false)
	,mResourcesCfg(Ogre::StringUtil::BLANK)
	,mPluginsCfg(Ogre::StringUtil::BLANK)
	,m_pickParam(0.0f)
//	,m_scopeLock(0)
	,m_mousePickMemory(false)
	,m_exitApplication(false)
	,m_initializationSuccessful (false)
	,m_shootBodyFrameListener(NULL)
{
}

DemoApplication::~DemoApplication(void)
{
	if (m_initializationSuccessful ) {
		delete m_shootBodyFrameListener;

		delete m_screen;
		delete m_cursor;
		m_ois->destroyInputObject(m_mouse);
		m_ois->destroyInputObject(m_keyboard);
		OIS::InputManager::destroyInputSystem(m_ois);

		delete m_debugRender;
		delete m_physicsWorld;
	}

	delete mRoot;
}

void DemoApplication::windowMoved (RenderWindow* rw)
{
}

void DemoApplication::windowResized(RenderWindow* rw)
{
}

void DemoApplication::windowFocusChange(RenderWindow* rw)
{
}

void DemoApplication::windowClosed(RenderWindow* rw)
{
}

bool DemoApplication::keyPressed(const OIS::KeyEvent &evt)
{
	return true;
}

bool DemoApplication::keyReleased(const OIS::KeyEvent &evt)
{
	return true;
}

bool DemoApplication::mouseMoved (const OIS::MouseEvent &evt)
{
	int buttons = m_mouseState.buttons;

	m_mouseState = evt.state;
	m_mouseState.buttons = buttons;
	m_cursor->updatePosition(evt.state.X.abs, evt.state.Y.abs);
	return true;
}

bool DemoApplication::mousePressed (const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	m_mouseState.buttons = m_mouseState.buttons | (1<<id);
	return true;
}

bool DemoApplication::mouseReleased( const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	m_mouseState.buttons = m_mouseState.buttons & ~(1<<id);
	return true;
}

SceneManager* DemoApplication::GetSceneManager() const
{
	return mSceneMgr;
}

OIS::Keyboard* DemoApplication::GetKeyboard() const
{
	return m_keyboard;
}

OgreNewtonWorld* DemoApplication::GetPhysics() const 
{ 
	return m_physicsWorld;
}


DemoApplication::OgreNewtonExample* DemoApplication::OnCreateWorld(DemoApplication* const application)
{
	return new OgreNewtonExample(application);
}

Matrix4 DemoApplication::GetCameraTransform () const
{
	Matrix4 matrix;
 	m_cameraSmoothing.GetTargetMatrix(&matrix[0][0]);
	return matrix.transpose();
}

Real DemoApplication::GetCameraYawAngle() const
{
	return m_cameraSmoothing.m_cameraYawAngle.valueRadians();
}

void DemoApplication::SeCameraTransform (const Matrix4& matrix)
{
	Matrix4 tmp (matrix.transpose());
	m_cameraSmoothing.SetTargetMatrix(&tmp[0][0]);
}

void DemoApplication::ResetCamera (const Vector3& posit, const Quaternion& rot)
{
	m_cameraSmoothing.Reset (posit, rot);
}

void DemoApplication::InitCamera()
{
	// Create the camera
	mCamera = mSceneMgr->createCamera("PlayerCam");

	// Position it at 500 in Z direction
	mCamera->setPosition(Ogre::Vector3(0,0,80));
	// Look back along -Z
	mCamera->lookAt(Ogre::Vector3(0,0,-200));
	mCamera->setNearClipDistance(0.1f);

	// create view ports
	// Create one viewport, entire window
	Ogre::Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

void DemoApplication::InitInputSystem()
{
	unsigned long windowHandle;
	mWindow->getCustomAttribute("WINDOW", &windowHandle);

	OIS::ParamList oisParameters;
	oisParameters.insert(OIS::ParamList::value_type("WINDOW", StringConverter::toString(windowHandle)));

	m_ois = OIS::InputManager::createInputSystem(oisParameters);

	m_keyboard = (OIS::Keyboard*) m_ois->createInputObject(OIS::OISKeyboard, true);
	m_keyboard->setEventCallback(this);

	m_mouse = (OIS::Mouse*) m_ois->createInputObject(OIS::OISMouse, true);
	m_mouse->setEventCallback(this);

	const OIS::MouseState& mouseState = m_mouse->getMouseState();
	mouseState.width = mWindow->getWidth();
	mouseState.height = mWindow->getHeight();
	m_mouseState = mouseState;

	WindowEventUtilities::addWindowEventListener(mWindow, this);		

	m_cursor = new MouseCursor();
	m_cursor->setImage("cursor.png");
	m_cursor->setWindowDimensions(mWindow->getWidth(), mWindow->getHeight());
	m_cursor->setVisible(false);

	m_screen = new ScreenWriter(mWindow->getWidth(), mWindow->getHeight());
}



// this is called at simulation time
void DemoApplication::UpdateFreeCamera ()
{
	Real moveScale = FREE_CAMERA_SPEED;
	if(m_keyboard->isKeyDown(OIS::KC_LSHIFT)) {
		moveScale *= 2.0f;
	}

	Real strafe = 0.0f;
	Real translation = 0.0f;
	Radian pitch (0.0f);
	Radian yaw (0.0f);

	if (m_keyboard->isKeyDown(OIS::KC_W)) {
		translation = -moveScale;
	}

	if (m_keyboard->isKeyDown(OIS::KC_S)) {
		translation = moveScale;
	}

	if (m_keyboard->isKeyDown(OIS::KC_A)) {
		strafe = -moveScale;
	}

	if (m_keyboard->isKeyDown(OIS::KC_D)) {
		strafe = moveScale;
	}

	if (!(m_keyboard->isKeyDown(OIS::KC_LCONTROL) || m_keyboard->isKeyDown(OIS::KC_RCONTROL))) {
		
		if (m_mouseState.X.rel > 2) {
			yaw = Radian (-FREE_CAMERA_YAW_SPEED);
		} else if (m_mouseState.X.rel < -2) {
			yaw = Radian (FREE_CAMERA_YAW_SPEED);
		}


		if (m_mouseState.Y.rel > 2) {
			pitch = Radian (-FREE_CAMERA_PITCH_SPEED);
		} else if (m_mouseState.Y.rel < -2) {
			pitch = Radian (FREE_CAMERA_PITCH_SPEED);
		}
	}

	// move camera according to user input
	m_cameraSmoothing.Move (translation, strafe, pitch, yaw);
}


// this is called at simulation time
void DemoApplication::UpdateMousePick ()
{
	bool mouseKey1 = m_mouseState.buttonDown(OIS::MB_Left);

	OgreNewtonRayPickManager* const rayPicker = m_physicsWorld->GetRayPickManager();
	if (m_keyboard->isKeyDown(OIS::KC_LCONTROL) || m_keyboard->isKeyDown(OIS::KC_RCONTROL)) {
		m_cursor->setVisible(true);
		if (mouseKey1) {
			Real mx = Real (m_mouseState.X.abs) / Real(m_mouseState.width);
			Real my = Real (m_mouseState.Y.abs) / Real(m_mouseState.height);
			Ray camray (mCamera->getCameraToViewportRay(mx, my));

			Vector3 start (camray.getOrigin());
			Vector3 end (camray.getPoint (200.0f));

			if (!m_mousePickMemory) {
				rayPicker->SetPickedBody (NULL);
				dNewtonBody* const body = rayPicker->RayCast (start, end, m_pickParam);
				if (body) {
					rayPicker->SetPickedBody (body, start + (end - start) * m_pickParam);
				}
			} else {
				rayPicker->SetTarget (start + (end - start) * m_pickParam);
			}

		} else {
			rayPicker->SetPickedBody (NULL);
		}
	} else {
		m_cursor->setVisible(false);
		rayPicker->SetPickedBody (NULL);
	}
	m_mousePickMemory = mouseKey1;
}



// called asynchronous at the beginning of a physics update. do any pre-physics update here  
void DemoApplication::OnPhysicUpdateBegin(dFloat timestepInSecunds)
{
	m_onScreeHelp.Update (m_keyboard->isKeyDown(OIS::KC_F1) ? true : false);
	m_debugTriggerKey.Update (m_keyboard->isKeyDown(OIS::KC_F3) ? true : false);
	m_asyncronousUpdateKey.Update (m_keyboard->isKeyDown(OIS::KC_F2) ? true : false);

	if (m_asyncronousUpdateKey.TriggerUp()) {
		m_physicsWorld->SetConcurrentUpdateMode(!m_physicsWorld->GetConcurrentUpdateMode());
	}

	// see if we pick a body form the screen
	UpdateMousePick ();
}


// called asynchronous at the beginning of a physics update. do any post-physics update here, moving the camera matrix  
void DemoApplication::OnPhysicUpdateEnd(dFloat timestepInSecunds)
{
	// camera update at physics simulation time
	UpdateFreeCamera ();

	// see if debug display was activated
	m_debugRender->SetDebugMode (m_debugTriggerKey.m_state);

	// see if we want to quit application
	m_exitApplication = m_keyboard->isKeyDown(OIS::KC_ESCAPE);
}


// called synchronous from ogre update loop before updating all sceneNodes controlled by a physic body  
void DemoApplication::OnRenderUpdateBegin(dFloat updateParam)
{
}

// called synchronous from ogre update loop after of updating updating all sceneNodes controlled by a physic body  
void DemoApplication::OnRenderUpdateEnd(dFloat updateParam)
{
	

	// set the camera interpolated matrix
	Matrix4 matrix;
	Vector3 cameraPosit;
	Quaternion cameraRotation;
	
	// interpolate the camera matrix at rendering time
	m_cameraSmoothing.InterplateMatrix (updateParam, &matrix[0][0]);
	matrix = matrix.transpose();
	mCamera->setPosition (matrix.getTrans());
	mCamera->setOrientation (matrix.extractQuaternion());

	// show statistic and help options
	if (m_onScreeHelp.m_state) {
		int row = 0;
		const RenderTarget::FrameStats& stats = mWindow->getStatistics();
		row = m_screen->write(20, row + 20, "FPS:  %05.3f", stats.lastFPS);
		row = m_screen->write(20, row + 20, "Physics time:  %05.3f ms", float (double (m_physicsWorld->GetPhysicsTimeInMicroSeconds()) * 1.0e-3f));
		row = m_screen->write(20, row + 20, "Number of rigid bodies:  %d", m_physicsWorld->GetBodyCount());
		row = m_screen->write(20, row + 20, "F1:  Hide debug help text");
		row = m_screen->write(20, row + 20, "F2:  Toggle %s simulation update", m_physicsWorld->GetConcurrentUpdateMode() ? "Asynchronous" : "Synchronous");
		row = m_screen->write(20, row + 20, "F3:  Toggle display physic debug");
		row = m_screen->write(20, row + 30, "W, S, A, D:  Free camera navigation");
		row = m_screen->write(20, row + 20, "Hold CTRL and Left Mouse Key:  Show mouse cursor and pick objects from the screen");
		row = m_screen->write(20, row + 20, "ESC:  Exit application");
	} else if (m_onScreeHelp.TriggerDown()){
		m_screen->removeAll();
	}

	m_screen->update();
}



void DemoApplication::createScene(void)
{
}

void DemoApplication::destroyScene(void)
{
}

void DemoApplication::createFrameListener()
{
	m_shootBodyFrameListener = new ShootRigidBody(this);
	mRoot->addFrameListener(m_shootBodyFrameListener);
}


bool DemoApplication::go(void)
{
	#ifdef _DEBUG
		mResourcesCfg = "resources_d.cfg";
		mPluginsCfg = "plugins_d.cfg";
	#else
		mResourcesCfg = "resources.cfg";
		mPluginsCfg = "plugins.cfg";
	#endif

	// construct Ogre::Root
	mRoot = new Ogre::Root(mPluginsCfg);

	if(!mRoot->showConfigDialog()) {
		return false;
	}

	// setup resources
	// Load resource paths from config file
	Ogre::ConfigFile cf;
	cf.load(mResourcesCfg);

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}

	// configure
	// Show the configuration dialog and initialise the system
	// You can skip this and use root.restoreConfig() to load configuration
	// settings if you were sure there are valid ones saved in ogre.cfg
	if(mRoot->restoreConfig() || mRoot->showConfigDialog()) {
		// If returned true, user clicked OK so initialise
		// Here we choose to let the system create a default rendering window by passing 'true'
		mWindow = mRoot->initialise(true, "Render Window");
	} else {
		return false;
	}

	m_initializationSuccessful = true;

	// choose scene manager
	// Get the SceneManager, in this case a generic one
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
   

	//-------------------------------------------------------------------------------------
	// Set default mip map level (NB some APIs ignore this)
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(MIP_UNLIMITED);

	// Create any resource listeners (for loading screens)
	//createResourceListener();

	// load resources
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	// Init the input system
	InitCamera();

	// Init the input system
	InitInputSystem();

	//
	// create a a physics world
	m_physicsWorld = OnCreateWorld (this);

	//
	// create a debug Renderer for showing physics data visually
	m_debugRender = new OgreNewtonDebugger (mSceneMgr->getRootSceneNode()->createChildSceneNode("_OgreNewton_Debugger_Node_"), m_physicsWorld);

	// crate frame listeners
	createFrameListener();

	// call for creation of the scene
	createScene();

	while(!m_exitApplication)
	{
		// Pump window messages for nice behavior
		Ogre::WindowEventUtilities::messagePump();

		if(mWindow->isClosed()) {
			break;
		}

		// do the physics update
		m_physicsWorld->Update();

		// Render a frame 
		if(!mRoot->renderOneFrame()) {
			break;
		}
	}

	// destroy the scene before quiting
	m_physicsWorld->WaitForUpdateToFinish();
	destroyScene();

	return true;
}


