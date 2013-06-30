
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
#include <OgreNewtonCollisionTree.h>

using namespace Ogre;

#include <OgreOverlay.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <ExampleApplication.h>
#include <ExampleFrameListener.h>


class OgreNewtonApplication: public ExampleApplication
{
	public:

	class ApplicationRayCast: public dNewtonRayCast
	{
		public:
		ApplicationRayCast(dNewton* const world)
			:dNewtonRayCast(world)
			,m_param(1.0f)
		{
		}

		void CastRay (const dFloat* const p0, const dFloat* const p1, int threadIndex = 0)
		{
			m_param = 1.2f;
			m_bodyHit = NULL;
			dNewtonRayCast::CastRay(p0, p1);
		}

		dFloat OnRayHit (const dNewtonBody* const body, const dNewtonCollision* const shape, const dFloat* const contact, const dFloat* const normal, const int* const collisionID, dFloat intersectParam)
		{
			if (intersectParam < m_param) {
				m_param = intersectParam;
				m_normal = Vector3 (normal[0], normal[1], normal[2]); 
				m_contact = Vector3 (contact[0], contact[1], contact[2]); 
			}
			return intersectParam;
		}

		Vector3 m_normal;
		Vector3 m_contact;
		dNewtonBody* m_bodyHit;
		Real m_param;
	};

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


	SceneNode* CreateNode (const String& mesh, const String& name, const Vector3& position, const Vector3& scale, const Quaternion& orientation)
	{
		Entity* const entity = mSceneMgr->createEntity(name, mesh);
		entity->setCastShadows(true);

		Ogre::SceneNode* const node = mSceneMgr->getRootSceneNode()->createChildSceneNode();

		node->attachObject(entity);
		node->setScale(scale);
		node->setPosition(position);
		node->setOrientation(orientation);
		return node;
	}



	void loadStaticScene ()
	{
		// create a scene body to add all static collidable meshes in the world 
		dNewtonSceneBody* const sceneBody = new dNewtonSceneBody (m_physicsWorld);

		// start adding collision shape to the scene body
		sceneBody->BeginAddRemoveCollision();

		// floor object!
		Entity* const floor = mSceneMgr->createEntity("Floor", "flatplane.mesh" );		
		//Entity* const floor = mSceneMgr->createEntity("Floor", "playground.mesh" );
		//Entity* const floor = mSceneMgr->createEntity("Floor", "castle.mesh" );

		SceneNode* const floornode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "FloorNode" );
		floornode->attachObject( floor );
		floor->setCastShadows( false );

		// create a collision tree mesh
		OgreNewtonCollisionTree meshCollision (m_physicsWorld, floornode);

		// add this collision to the scene body
		sceneBody->AddCollision(&meshCollision);


		// done adding collision shape to the scene body, now optimize the scene
		sceneBody->EndAddRemoveCollision();
	}

	void LoadDynamicsScene(const Vector3& origin)
	{
		Vector3 posit (origin + Vector3(0.0f, 0.0f, -5.0f)); 


		SceneNode* const node = CreateNode("box.mesh", "box", posit, Vector3(1.0f, 1.0f, 1.0f), Quaternion (Degree(0), Vector3::UNIT_Y));

		SceneNode* const node1 = CreateNode("box.mesh", "box1", posit + Vector3(1.0f, 1.0f, 1.0f), Vector3(1.0f, 1.0f, 1.0f), Quaternion (Degree(0), Vector3::UNIT_Y));
	}

	void createScene()
	{
		// create the physic world first
		m_physicsWorld = new OgreNewtonWorld (mWindow);
		mRoot->addFrameListener(m_physicsWorld);


		//make a light
		Light* const light0 = mSceneMgr->createLight( "Light0" );
//		Light* const light1 = mSceneMgr->createLight( "Light1" );
//		Light* const light2 = mSceneMgr->createLight( "Light2" );
		Light* const light3 = mSceneMgr->createLight( "Light3" );

		light0->setType (Light::LT_POINT );
		light0->setPosition (Vector3(-100.0f, 100.0f, -100.0f) );
		
//		light1->setType (Light::LT_POINT );
//		light1->setPosition (Vector3(100.0f, 100.0f, -100.0f) );

//		light2->setType (Light::LT_POINT );
//		light2->setPosition (Vector3(-100.0f, 100.0f, 100.0f) );

		light3->setType (Light::LT_POINT );
		light3->setPosition (Vector3(100.0f, 100.0f, 100.0f) );



		// sky box.
		mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");


		// load all of the static geometry
		loadStaticScene ();

		// position camera using the ray cast functionality
		Ogre::Vector3 start(0.0f, 1000.0f, 10.0f);
		Ogre::Vector3 end(0.0f, -1000.0f, 10.0f);
		ApplicationRayCast raycaster(m_physicsWorld); 
		raycaster.CastRay (&start.x, &end.x);

		Vector3 origin (raycaster.m_contact + Vector3 (0.0f, 5.0f, 0.0f));
		mCamera->setPosition(origin);

		// set the near and far clip plane
		mCamera->setNearClipDistance(0.1f);
		mCamera->setFarClipDistance(10000.0f);

		// now load the dynamcics Scene
		LoadDynamicsScene(origin);
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

