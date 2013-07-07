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

class MouseCursor;
class ScreenWriter;

//class ApplicationFrameListener: public FrameListener, public OIS::KeyListener, public OIS::MouseListener, public WindowEventListener
class ApplicationFrameListener: public FrameListener, public OIS::KeyListener, public OIS::MouseListener, public WindowEventListener
{
	class KeyTrigger
	{
		public:
		KeyTrigger (bool state)
			:m_state(state)
			,m_mem0(false)
			,m_mem1(false)
		{
		}

		bool TriggerUp() const
		{
			return !m_mem0 & m_mem1;
		}

		bool TriggerDown() const
		{
			return m_mem0 & !m_mem1;
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

	void UpdateMousePick ();
	void UpdateFreeCamera ();
	
	bool frameStarted(const FrameEvent &evt);
	bool frameEnded(const FrameEvent& evt);

	virtual void windowMoved (RenderWindow* rw); 
	virtual void windowFocusChange(RenderWindow* rw);
	virtual void windowResized(RenderWindow* rw);
	virtual void windowClosed(RenderWindow* rw);

	virtual bool keyPressed(const OIS::KeyEvent &evt);
	virtual bool keyReleased(const OIS::KeyEvent &evt);
	virtual bool mouseMoved (const OIS::MouseEvent &arg);
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	Camera* m_camera;
	SceneManager* m_sceneMgr;
	RenderWindow* m_renderWindow;
	OgreNewtonWorld* m_physicsWorld;
	OgreNewtonDebugger* m_debugRender;
	OgreNewtonRayPickManager* m_rayPicker;
	KeyTrigger m_debugTriggerKey;
	KeyTrigger m_onScreeHelp;
	bool m_mousePickMemory;

	MouseCursor* m_cursor;
	OIS::Mouse* m_mouse;
	OIS::InputManager* m_ois;
	OIS::Keyboard* m_keyboard;
	ScreenWriter* m_screen;



	bool m_shutDwoun;
	Real m_pickParam;
};


