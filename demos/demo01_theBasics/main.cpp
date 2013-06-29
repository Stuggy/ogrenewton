
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






/*
class OgreNewtonFrameListener: public OgreNewtonWorld
{
//protected:
//	OgreNewt::World* m_World;
//	//SceneNode* msnCam;
//	SceneManager* mSceneMgr;
//	int count;
//	float timer;

	public:
	OgreNewtonFrameListener(RenderWindow* win, Camera* cam, SceneManager* mgr)
		:OgreNewtonWorld(win)
	{
	}

	virtual ~OgreNewtonFrameListener(void)
	{
	}

//	bool frameStarted(const FrameEvent &evt);

};
*/


class OgreNewtonApplication: public ExampleApplication
{
public:
	OgreNewtonApplication()
		:ExampleApplication()
	{
	}

	virtual ~OgreNewtonApplication()
	{
	}

	protected:
//	void createFrameListener();
	void createScene()
	{

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

};



INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{

	try
    {
		OgreNewtonApplication application;
		
//		if (application.initialise() && application.setup())
//		{
//			application.run();
//		}
	}
	catch(Ogre::Exception &e)
	{
		MessageBox(NULL, e.getFullDescription().c_str(), "Well, this is embarrassing.. an Ogre exception has occurred.", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}

	return 0;
}

