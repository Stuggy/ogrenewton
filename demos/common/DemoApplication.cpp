
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


#include <OgreNewtonStdAfx.h>
#include "MouseCursor.h"
#include "ScreenWriter.h"
#include "DemoApplication.h"


#define CAMERA_SPEED 0.25f
#define CAMERA_YAW_SPEED (1.0f * 3.141592f / 180.0f)
#define CAMERA_PITCH_SPEED (1.0f * 3.141592f / 180.0f)

DemoApplication::DemoApplication()
	:OgreNewtonExampleApplication()
	,OIS::KeyListener()
	,OIS::MouseListener()
	,WindowEventListener()
	,m_debugTriggerKey(false)
	,m_onScreeHelp(true)
	,m_mousePickMemory(false)
	,m_shutDwoun(false)
	,m_pickParam(0.0f)
{
}


DemoApplication::~DemoApplication(void)
{
	delete m_screen;
	if (m_ois) {
		delete m_cursor;
		m_ois->destroyInputObject(m_mouse);
		m_ois->destroyInputObject(m_keyboard);
		OIS::InputManager::destroyInputSystem(m_ois);
	}
}

void DemoApplication::createScene(void)
{
	OgreNewtonExampleApplication::createScene();



	unsigned long windowHandle;
	mWindow->getCustomAttribute("WINDOW", &windowHandle);

	OIS::ParamList oisParameters;
	oisParameters.insert(OIS::ParamList::value_type("WINDOW", StringConverter::toString(windowHandle)));

	m_ois = OIS::InputManager::createInputSystem(oisParameters);

	m_mouse = (OIS::Mouse*) m_ois->createInputObject(OIS::OISMouse, true);
	m_mouse->setEventCallback(this);
	const OIS::MouseState& mouseState = m_mouse->getMouseState();
	mouseState.width = mWindow->getWidth();
	mouseState.height = mWindow->getHeight();

	m_keyboard = (OIS::Keyboard*) m_ois->createInputObject(OIS::OISKeyboard, true);
	m_keyboard->setEventCallback(this);

	WindowEventUtilities::addWindowEventListener(mWindow, this);		

	m_cursor = new MouseCursor();
	m_cursor->setImage("cursor.png");
	m_cursor->setWindowDimensions(mWindow->getWidth(), mWindow->getHeight());
	m_cursor->setVisible(true);

	m_screen = new ScreenWriter(mWindow->getWidth(), mWindow->getHeight());
}

void DemoApplication::destroyScene(void)
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
	if (m_cursor != NULL) {
		// Update cursor position
		m_cursor->updatePosition(evt.state.X.abs, evt.state.Y.abs);
	}
	return true;
}

bool DemoApplication::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return true;
}

bool DemoApplication::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return true;
}

void DemoApplication::windowMoved (RenderWindow* rw) 
{
}

void DemoApplication::windowFocusChange(RenderWindow* rw) 
{
}

void DemoApplication::windowResized(RenderWindow* rw)
{
}

void DemoApplication::windowClosed(RenderWindow* rw)
{
	m_shutDwoun = true;
}

// this is called at rendering time
bool DemoApplication::OnRenderUpdateBegin(dFloat updateParam)
{
	// set the camera matrix for rendering time
	OgreNewtonExampleApplication::OnRenderUpdateBegin(updateParam);

	// update visual debug 
	m_debugRender->SetDebugMode (m_debugTriggerKey.m_state);

	// check for termination
	if (m_shutDwoun || m_keyboard->isKeyDown(OIS::KC_ESCAPE))
		return false;

	return true;
}

// this is called at rendering time
bool DemoApplication::OnRenderUpdateEnd(dFloat updateParam)
{
	const RenderTarget::FrameStats& stats = mWindow->getStatistics();

	// set the camera interpolated matrix
	Vector3 cameraPosit;
	Quaternion cameraRotation;
	GetInterpolatedCameraMatrix(cameraPosit, cameraRotation);
	mCamera->setPosition(cameraPosit);
	mCamera->setOrientation(cameraRotation);


	OgreNewtonWorld* const physics = GetPhysics();
	m_screen->write(20, 20, "FPS: %05.3f", stats.lastFPS);
	m_screen->write(20, 36, "Physics time: %05.3f ms", float (double (physics->GetPhysicsTimeInMicroSeconds()) * 1.0e-3f));
	m_screen->write(20, 52, "Number of rigid bodies: %d", physics->GetBodyCount());
	if (m_onScreeHelp.m_state) {
		m_screen->write(20,  68, "F1: Hide debug help text");
		m_screen->write(20,  84, "F3: toggle display physic debug");
		m_screen->write(20, 100, "W, S, A, D: Free camera navigation");
		m_screen->write(20, 116, "Hold CTRL and Left Mouse Key: show mouse cursor and pick");
		m_screen->write(20, 132, "ESC: Exit application");

	} else if (m_onScreeHelp.TriggerDown()){
		m_screen->removeAll();
	}
	m_screen->update();
	return true;
}


// this is called at simulation time
void DemoApplication::UpdateMousePick ()
{
	const OIS::MouseState& mouseState = m_mouse->getMouseState();
	bool mouseKey1 = mouseState.buttonDown(OIS::MB_Left);

	if (m_keyboard->isKeyDown(OIS::KC_LCONTROL) || m_keyboard->isKeyDown(OIS::KC_RCONTROL)) {
		m_cursor->setVisible(true);
		if (mouseKey1) {
			Real mx = Real (mouseState.X.abs) / Real(mouseState.width);
			Real my = Real (mouseState.Y.abs) / Real(mouseState.height);
			Ray camray = mCamera->getCameraToViewportRay(mx, my);

			Vector3 start (camray.getOrigin());
			Vector3 end (camray.getPoint (200.0f));

			if (!m_mousePickMemory) {
				m_rayPicker->SetPickedBody (NULL);
				dNewtonBody* const body = m_rayPicker->RayCast (start, end, m_pickParam);
				if (body) {
					m_rayPicker->SetPickedBody (body, start + (end - start) * m_pickParam);
				}
			} else {
				m_rayPicker->SetTarget (start + (end - start) * m_pickParam);
			}

		} else {
			m_rayPicker->SetPickedBody (NULL);
		}
	} else {
		m_cursor->setVisible(false);
		m_rayPicker->SetPickedBody (NULL);
	}
	m_mousePickMemory = mouseKey1;
}

// this is called at simulation time
void DemoApplication::UpdateFreeCamera ()
{
	OgreNewtonWorld* const physics = GetPhysics();

	Real moveScale = CAMERA_SPEED;
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
		const OIS::MouseState& mouseState = m_mouse->getMouseState();
		//dTrace (("%d\n", mouseState.X.rel))

		if (mouseState.X.rel > 2) {
			yaw = Radian (-CAMERA_YAW_SPEED);
		} else if (mouseState.X.rel < -2) {
			yaw = Radian (CAMERA_YAW_SPEED);
		}

		if (mouseState.Y.rel > 2) {
			pitch = Radian (-CAMERA_PITCH_SPEED);
		} else if (mouseState.Y.rel < -2) {
			pitch = Radian (CAMERA_PITCH_SPEED);
		}
	}

	// move camera according to user input
	MoveCamera (translation, strafe, pitch, yaw);
}

// this is called at simulation time
void DemoApplication::OnPhysicUpdateBegin(dFloat timestepInSecunds)
{
	OgreNewtonExampleApplication::OnPhysicUpdateBegin(timestepInSecunds);

	// get the inputs
	m_mouse->capture();
	m_keyboard->capture();
	WindowEventUtilities::messagePump();

	// set the debug render mode
	m_debugTriggerKey.Update (m_keyboard->isKeyDown(OIS::KC_F3) ? true : false);
	m_onScreeHelp.Update (m_keyboard->isKeyDown(OIS::KC_F1) ? true : false);

	// see if we have a object on the pick queue
	UpdateMousePick ();
	UpdateFreeCamera ();
}

// this is calle at simulation time
void DemoApplication::OnPhysicUpdateEnd(dFloat timestepInSecunds)
{
	OgreNewtonExampleApplication::OnPhysicUpdateEnd(timestepInSecunds);
}
