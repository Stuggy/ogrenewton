
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
#include <OgreNewtonRayPeekingManager.h>
#include <OgreNewtonExampleApplication.h>

using namespace Ogre;



class OgreNewtonApplication: public OgreNewtonExampleApplication
{
	public:
	class ApplicationFrameListener: public ExampleFrameListener
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
		ApplicationFrameListener(Root* const root, RenderWindow* const win, Camera* const cam, SceneManager* const mgr, OgreNewtonWorld* const physicsWorld, OgreNewtonDebugger* const debugRender)
			:ExampleFrameListener(win, cam)
			,m_sceneMgr(mgr)
			,m_physicsWorld(physicsWorld)
			,m_debugRender(debugRender)
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
			// set the debug render mode
			m_debugTriggerKey.Update (mKeyboard->isKeyDown(OIS::KC_F3) ? true : false);
			m_debugRender->SetDebugMode (m_debugTriggerKey.m_state);

			// check for termination
			if (mKeyboard->isKeyDown(OIS::KC_ESCAPE))
				return false;

			return true;
		}

		SceneManager* m_sceneMgr;
		OgreNewtonWorld* m_physicsWorld;
		OgreNewtonDebugger* m_debugRender;
		KeyTrigger m_debugTriggerKey;
	};


	OgreNewtonApplication()
		:OgreNewtonExampleApplication()
		,m_listener(NULL)
	{
	}

	virtual ~OgreNewtonApplication()
	{
		if (m_listener) {
			mRoot->removeFrameListener(m_listener);
			delete m_listener;
		}
	}

	protected:
	void createFrameListener()
	{
		// this is our custom frame listener for this app, that lets us shoot cylinders with the space bar, move the camera, etc.
		m_listener = new ApplicationFrameListener (mRoot, mWindow, mCamera, mSceneMgr, m_physicsWorld, m_debugRender);
		mRoot->addFrameListener(m_listener);
	}


	SceneNode* CreateNode (const String& mesh, const String& name, const Vector3& position, const Vector3& scale, const Quaternion& orientation)
	{
		Entity* const entity = mSceneMgr->createEntity(name, mesh);
		entity->setCastShadows(true);

		SceneNode* const node = mSceneMgr->getRootSceneNode()->createChildSceneNode();

		node->attachObject(entity);
		node->setScale(scale);
		node->setPosition(position);
		node->setOrientation(orientation);
		return node;
	}

	String MakeName (const char* const name) const 
	{
		static int enumeration = 0;
		char text[256];
		sprintf (text, "%s_%d", name, enumeration);
		enumeration ++;
		return String (text);
	}

	void loadStaticScene ()
	{
		// create a scene body to add all static collidable meshes in the world 
		OgreNewtonSceneBody* const sceneBody = new OgreNewtonSceneBody (m_physicsWorld);

		// start adding collision shape to the scene body
		sceneBody->BeginAddRemoveCollision();

		// floor object!
		Entity* const floor = mSceneMgr->createEntity("Floor", "flatplane.mesh" );		
		//Entity* const floor = mSceneMgr->createEntity("Floor", "playground.mesh" );
		//Entity* const floor = mSceneMgr->createEntity("Floor", "castle.mesh" );

		SceneNode* const floorNode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "FloorNode" );
		floorNode->attachObject( floor );
		floor->setCastShadows( false );

		// add this collision to the scene body
		sceneBody->AddCollisionTree (floorNode);


		// done adding collision shape to the scene body, now optimize the scene
		sceneBody->EndAddRemoveCollision();
	}

	void BuildJenga(const Vector3& location, int high)
	{
		Vector3 blockBoxSize (0.8f, 0.5f, 0.8f * 3.0f);

		// find the floor position
		Vector3 start(location + Vector3 (0.0f, 10.0f, 0.0f));
		Vector3 end (start - Vector3 (0.0f, 20.0f, 0.0f));
		OgreNewtonRayCast raycaster(m_physicsWorld); 
		raycaster.CastRay (&start.x, &end.x);
		Vector3 position (raycaster.m_contact + Vector3 (0.0f, blockBoxSize.y * 0.5f, 0.0f));

		Matrix4 baseMatrix (Matrix4::IDENTITY);
		baseMatrix.setTrans (position);


		// set realistic mass and inertia matrix for each block
		Real mass = 5.0f;

		// create a 90 degree rotation matrix
		Matrix4 rotMatrix (Quaternion (Degree(90.0f), Vector3 (0.0f, 1.0f, 0.0f)));

		Real collisionPenetration = 1.0f / 256.0f;

		for (int i = 0; i < high; i ++) { 
			Matrix4 matrix(baseMatrix);
			Vector3 step_x (matrix[0][0], matrix[0][1], matrix[0][2]); 

			step_x = step_x * blockBoxSize.x;
			matrix.setTrans (matrix.getTrans() - step_x);

			for (int j = 0; j < 3; j ++) { 
				SceneNode* const node = CreateNode("box.mesh", MakeName ("jengaBox"), matrix.getTrans(), blockBoxSize, matrix.extractQuaternion());
				OgreNewtonBody::CreateBox (m_physicsWorld, node, mass, matrix);
				matrix.setTrans (matrix.getTrans() + step_x);
			}

			baseMatrix = baseMatrix * rotMatrix;
			Vector3 step_y (matrix[1][0], matrix[1][1], matrix[1][2]); 
			step_y = step_y * (blockBoxSize.y - collisionPenetration);
			baseMatrix.setTrans (baseMatrix.getTrans() + step_y);
		}
	}

	void LoadDynamicScene(const Vector3& origin)
	{
		BuildJenga (origin + Vector3(-10.0f, 0.0f, -40.0f) , 40);
//		BuildJenga (origin + Vector3( 10.0f, 0.0f, -40.0f) , 40);
//		BuildJenga (origin + Vector3(-10.0f, 0.0f, -60.0f) , 40);
//		BuildJenga (origin + Vector3( 10.0f, 0.0f, -60.0f) , 40);
	}

	void createScene()
	{
		// create the physic world first
		OgreNewtonExampleApplication::createScene();

		// create a ray cast peeker 
		m_rayPeeker = new OgreNewtonRayPeekManager (m_physicsWorld);

		//make a light
		Light* const light0 = mSceneMgr->createLight( "Light0" );
		Light* const light3 = mSceneMgr->createLight( "Light3" );

		light0->setType (Light::LT_POINT );
		light0->setPosition (Vector3(-100.0f, 100.0f, -100.0f) );

		light3->setType (Light::LT_POINT );
		light3->setPosition (Vector3(100.0f, 100.0f, 100.0f) );

		// sky box.
		mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");

		// load all of the static geometry
		loadStaticScene ();

		// position camera using the ray cast functionality
		Vector3 start(0.0f, 1000.0f, 10.0f);
		Vector3 end(0.0f, -1000.0f, 10.0f);
		OgreNewtonRayCast raycaster(m_physicsWorld); 
		raycaster.CastRay (&start.x, &end.x);

		Vector3 origin (raycaster.m_contact + Vector3 (0.0f, 2.0f, 0.0f));
		mCamera->setPosition(origin);

		// set the near and far clip plane
		mCamera->setNearClipDistance(0.1f);
		mCamera->setFarClipDistance(10000.0f);

		// now load the dynamics Scene
		LoadDynamicScene(origin);
	}

	protected:
	ApplicationFrameListener* m_listener;
	OgreNewtonRayPeekManager* m_rayPeeker;
};



INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
	OgreNewtonApplication application;

	try {
		application.go();
	} catch(Exception &e) {
		MessageBox(NULL, e.getFullDescription().c_str(), "Well, this is embarrassing.. an Ogre exception has occurred.", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}

	return 0;
}

