
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
#include "OgreNewtonBody.h"
#include <OgreNewtonWorld.h>
#include <OgreNewtonRayCast.h>
#include <OgreNewtonDebugger.h>
#include <OgreNewtonSceneBody.h>
#include <OgreNewtonRayPickManager.h>
#include <OgreNewtonExampleApplication.h>



using namespace Ogre;



class ApplicationFrameListener: public FrameListener, public OIS::KeyListener, public OIS::MouseListener, public WindowEventListener
{
	class KeyTrigger
	{
		public:
		KeyTrigger ()
			:m_state(false)
			,m_mem0(false)
			,m_mem1(false)
		{
		}
		
		void Update (bool input)
		{
			m_mem0 = m_mem1;
			m_mem1 = input;
			m_state = (!m_mem0 & m_mem1) ^ m_state;
		}

		bool m_state;

		private:
		bool m_mem0;
		bool m_mem1;
	};


	public:
	ApplicationFrameListener(Root* const root, RenderWindow* const win, Camera* const cam, SceneManager* const mgr, OgreNewtonWorld* const physicsWorld, OgreNewtonDebugger* const debugRender);
	virtual ~ApplicationFrameListener(void);

	void DoMousePick ();
	virtual void updateStats(void);
	bool frameStarted(const FrameEvent &evt);



	virtual bool keyPressed(const OIS::KeyEvent &evt)
	{
		return true;
	}

	virtual bool keyReleased(const OIS::KeyEvent &evt)
	{
		return true;
	}

	virtual bool mouseMoved (const OIS::MouseEvent &arg )
	{
//			if (cursor != NULL) {
//				// Update cursor position
//				cursor->updatePosition(evt.state.X.abs, evt.state.Y.abs);
//			}
		return true;
	}

	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
	{
		return true;
	}

	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
	{
		return true;
	}

	void windowMoved (RenderWindow* rw) 
	{
	}

	void windowFocusChange(RenderWindow* rw) 
	{
	}

	void windowResized(RenderWindow* rw)
	{
	}

	void windowClosed(RenderWindow* rw)
	{
		m_shutDwoun = true;
	}

	SceneManager* m_sceneMgr;
	Camera* m_camera;
	OgreNewtonWorld* m_physicsWorld;
	OgreNewtonDebugger* m_debugRender;
	OgreNewtonRayPickManager* m_rayPicker;
	KeyTrigger m_debugTriggerKey;
	bool m_mousePickMemory;

//		MouseCursor* cursor;
	OIS::Mouse* m_mouse;
	OIS::InputManager* m_ois;
	OIS::Keyboard* m_keyboard;
	bool m_shutDwoun;
};


