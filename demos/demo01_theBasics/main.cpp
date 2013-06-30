
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



#include "OgreNewtonStdAfx.h"
#include <OgreNewtonWorld.h>

using namespace Ogre;

#include <OgreOverlay.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <ExampleApplication.h>
#include <ExampleFrameListener.h>


class OgreNewtonApplication: public ExampleApplication
{
	public:

	class ApplicationFrameListener: public ExampleFrameListener
	{
		public:
		ApplicationFrameListener(Root* const root, RenderWindow* const win, Camera* const cam, SceneManager* const mgr, OgreNewtonWorld* const physicsWorld)
			:ExampleFrameListener(win, cam)
			,m_sceneMgr(mgr)
			,m_physicsWorld(physicsWorld)
		{
		}

		virtual ~ApplicationFrameListener(void)
		{
		}

		virtual void updateStats(void)
		{
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
		}

		bool frameStarted(const FrameEvent &evt)
		{
			if (mKeyboard->isKeyDown(OIS::KC_ESCAPE))
				return false;

			return true;
		}

		SceneManager* m_sceneMgr;
		OgreNewtonWorld* m_physicsWorld;
	};


	OgreNewtonApplication()
		:ExampleApplication()
		,m_listener(NULL)
		,m_physicsWorld(NULL)
	{
	}

	virtual ~OgreNewtonApplication()
	{
		if (m_listener) {
			mRoot->removeFrameListener(m_listener);
			delete m_listener;
		}

		if (m_physicsWorld) {
			mRoot->removeFrameListener(m_physicsWorld);
			delete m_physicsWorld;
		}
	}

	protected:
	void createFrameListener()
	{
		// this is our custom frame listener for this app, that lets us shoot cylinders with the space bar, move the camera, etc.
		m_listener = new ApplicationFrameListener (mRoot, mWindow, mCamera, mSceneMgr, m_physicsWorld);
		mRoot->addFrameListener(m_listener);
	}

	void loadStaticLevel ()
	{
		// create a scene body to add all static collidable meshes in the world 
		dNewtonSceneBody* const sceneBody = new dNewtonSceneBody (m_physicsWorld);


		// floor object!
		//floor = mSceneMgr->createEntity("Floor", "simple_terrain.mesh" );
		Entity* const floor = mSceneMgr->createEntity("Floor", "playground.mesh" );
		//	floor = mSceneMgr->createEntity("Floor", "castle.mesh" );
		//	floor = mSceneMgr->createEntity("Floor", "xxx.mesh" );

		SceneNode* const floornode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "FloorNode" );
		floornode->attachObject( floor );
		floor->setCastShadows( false );


	}


	void createScene()
	{
		// create the physic world first
		m_physicsWorld = new OgreNewtonWorld (mWindow);
		mRoot->addFrameListener(m_physicsWorld);


		//make a light
		Ogre::Light* const light = mSceneMgr->createLight( "Light1" );
		light->setType( Ogre::Light::LT_POINT );
		light->setPosition( Ogre::Vector3(0.0f, 100.0f, 100.0f) );



		// sky box.
		mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");


		// load all of the static geometry
		loadStaticLevel ();



		// position camera
//		Ogre::Vector3 start(0.0f, 1000.0f, 10.0f);
//		Ogre::Vector3 end(0.0f, -1000.0f, 10.0f);
//		OgreNewt::BasicRaycast castRay (m_World, start, end, true);
//		OgreNewt::BasicRaycast::BasicRaycastInfo info = castRay.getFirstHit();
//		mCamera->setPosition(start.x, 2.0f + start.y + (end.y - start.y) * info.mDistance, start.z);

		// set the near and far clip plane
//		mCamera->setNearClipDistance(0.2f);
//		mCamera->setFarClipDistance(1000.0f);
	}

	protected:
	OgreNewtonWorld* m_physicsWorld;
	ApplicationFrameListener* m_listener;
};



INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{

	OgreNewtonApplication application;

	try
    {
		application.go();
	}
	catch(Ogre::Exception &e)
	{
		MessageBox(NULL, e.getFullDescription().c_str(), "Well, this is embarrassing.. an Ogre exception has occurred.", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}

	return 0;
}

