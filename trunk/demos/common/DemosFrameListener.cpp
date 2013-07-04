
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
#include "DemosFrameListener.h"

ApplicationFrameListener::ApplicationFrameListener(Root* const root, RenderWindow* const win, Camera* const cam, SceneManager* const mgr, OgreNewtonWorld* const physicsWorld, OgreNewtonDebugger* const debugRender)
	:FrameListener()
	,OIS::KeyListener()
	,OIS::MouseListener()
	,WindowEventListener()
	,m_sceneMgr(mgr)
	,m_camera(cam)
	,m_physicsWorld(physicsWorld)
	,m_debugRender(debugRender)
	,m_rayPicker (new OgreNewtonRayPickManager (physicsWorld))
	,m_mousePickMemory(false)
	,m_shutDwoun(false)
{
	unsigned long windowHandle;
	win->getCustomAttribute("WINDOW", &windowHandle);

	OIS::ParamList oisParameters;
	oisParameters.insert(OIS::ParamList::value_type("WINDOW", StringConverter::toString(windowHandle)));

	m_ois = OIS::InputManager::createInputSystem(oisParameters);

	m_mouse = (OIS::Mouse*) m_ois->createInputObject(OIS::OISMouse, true);
	m_mouse->setEventCallback(this);
	const OIS::MouseState& mouseState = m_mouse->getMouseState();
	mouseState.width = win->getWidth();
	mouseState.height = win->getHeight();

	m_keyboard = (OIS::Keyboard*) m_ois->createInputObject(OIS::OISKeyboard, true);
	m_keyboard->setEventCallback(this);

	WindowEventUtilities::addWindowEventListener(win, this);		
}

ApplicationFrameListener::~ApplicationFrameListener(void)
{
	if (m_ois) {
		m_ois->destroyInputObject(m_mouse);
		m_ois->destroyInputObject(m_keyboard);
		OIS::InputManager::destroyInputSystem(m_ois);
	}
}


void ApplicationFrameListener::updateStats(void)
{
/*
	ExampleFrameListener::updateStats();
	try {
		// use one of the debug output to show the physics time
		OverlayElement* const gui = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
		dAssert (gui);
		double time = double (m_physicsWorld->GetPhysicsTimeInMicroSeconds()) * 1.0e-3f;
		char text[256];
		sprintf (text, "Physics time : %05.3f ms", time);
		gui->setCaption(text);
	}
	catch(...) 
	{ 
	}
*/
}


void ApplicationFrameListener::DoMousePick ()
{
	bool mouseKey1 = m_mouse->getMouseState().buttonDown(OIS::MB_Right);
	if (mouseKey1) {

		Real mx = Real (m_mouse->getMouseState().X.abs) / Real(m_mouse->getMouseState().width);
		Real my = Real (m_mouse->getMouseState().Y.abs) / Real(m_mouse->getMouseState().height);
		Ray camray = m_camera->getCameraToViewportRay(mx, my);

		Vector3 start (camray.getOrigin());
		Vector3 end (camray.getPoint (200.0f));

		if (!m_mousePickMemory) {
			Vector3 hitPoint;
			Vector3 hitNormal;
			m_rayPicker->SetPickedBody (NULL);
			OgreNewtonBody* const body = m_rayPicker->PickBody (start, end, hitPoint, hitNormal);
			if (body) {
				dAssert (0);
			}

		} else {
			dAssert (0);
		}

	} else {
		m_rayPicker->SetPickedBody (NULL);
	}
	m_mousePickMemory = mouseKey1;

}


bool ApplicationFrameListener::frameStarted(const FrameEvent &evt)
{
	// get the inputs
	m_mouse->capture();
	m_keyboard->capture();
	WindowEventUtilities::messagePump();

	// set the debug render mode
	m_debugTriggerKey.Update (m_keyboard->isKeyDown(OIS::KC_F3) ? true : false);

	DoMousePick ();

	m_debugRender->SetDebugMode (m_debugTriggerKey.m_state);

	// check for termination
	if (m_shutDwoun || m_keyboard->isKeyDown(OIS::KC_ESCAPE))
		return false;

	return true;
}


