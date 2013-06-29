
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

//#include <OgreException.h>

//#include <OgreRoot.h>
//#include <OgreRenderWindow.h>
//#include <OgreSceneManager.h>
//#include <OgreCamera.h>
//#include <OgreWindowEventUtilities.h>
#include <OgreOverlay.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <ExampleApplication.h>
#include <ExampleFrameListener.h>


class OgreNewtonApplication: public ExampleApplication
{
	public:


	//class OgreNewtonFrameListener: public OgreNewtonWorld
	class ApplicationFrameListener: public ExampleFrameListener
	{
		public:
		ApplicationFrameListener(Root* const root, RenderWindow* const win, Camera* const cam, SceneManager* const mgr)
			:ExampleFrameListener(win, cam)
			,m_root(root)
			,m_sceneMgr(mgr)
		{
			m_physicsWorld = new OgreNewtonWorld (win);
			root->addFrameListener(m_physicsWorld);
		}

		virtual ~ApplicationFrameListener(void)
		{
			m_root->removeFrameListener(m_physicsWorld);
			delete m_physicsWorld;
		}

		bool frameStarted(const FrameEvent &evt)
		{
			if (mKeyboard->isKeyDown(OIS::KC_ESCAPE))
				return false;

			return true;
		}

		Root* m_root;
		SceneManager* m_sceneMgr;
		OgreNewtonWorld* m_physicsWorld;
	};


	OgreNewtonApplication()
		:ExampleApplication()
		,m_listener(NULL)
	{
	}

	virtual ~OgreNewtonApplication()
	{
		mRoot->removeFrameListener(m_listener);
		delete m_listener;
	}

	protected:
	void createFrameListener()
	{
		// this is our custom frame listener for this app, that lets us shoot cylinders with the space bar, move the camera, etc.
		m_listener = new ApplicationFrameListener (mRoot, mWindow, mCamera, mSceneMgr);
		mRoot->addFrameListener(m_listener);
	}

	void createScene()
	{
		// sky box.
		mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");


		// this will be a static object that we can throw objects at.  we'll use a simple cylinder primitive.
		// first I load the visual mesh that represents it.  I have some simple primitive shaped .mesh files in
		// the "primitives" directory to make this simple... all of them have a basic size of "1" so that they
		// can easily be scaled to fit any size primitive.
//		Entity* floor;
//		SceneNode* floornode;
//		floor = mSceneMgr->createEntity("Floor", "cylinder.mesh" );
//		floornode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "FloorNode" );
//		floornode->attachObject( floor );
//		floor->setMaterialName( "Simple/BeachStones" );
//		floor->setCastShadows( false );

	}

//	OgreNewt::Body* makeSimpleBox( Ogre::Vector3& size, Ogre::Vector3& pos, Ogre::Quaternion& orient );
//	OgreNewt::Body* makeSimpleShere( Ogre::Vector3& size, Ogre::Vector3& pos, Ogre::Quaternion& orient );

	private:

//	void SwinDoors (Ogre::Vector3 pos, Ogre::Vector3 size);
//	void SlidingDoors (Ogre::Vector3 pos, Ogre::Vector3 size);
//	void SimpleRobot (Ogre::Vector3 pos, Ogre::Vector3 size);
//	void BallAndSocketRope(Ogre::Vector3 pos, Ogre::Vector3 size);
//	OgreNewt::World* m_World;
//	Ogre::FrameListener* mNewtonListener;
//	int mEntityCount;

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

