
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
#include <OgreNewtonMesh.h>
#include <OgreNewtonWorld.h>
#include <OgreNewtonRayCast.h>
#include <OgreNewtonDebugger.h>
#include <OgreNewtonSceneBody.h>
#include <OgreNewtonDynamicBody.h>
#include <OgreNewtonPlayerManager.h>
#include <OgreNewtonRayPickManager.h>
#include <OgreNewtonExampleApplication.h>


#include "Utils.h"
#include "BuildJenga.h"
#include "BuildPyramid.h"
#include "DemoApplication.h"


using namespace Ogre;

class OgreNewtonDemoApplication: public DemoApplication
{
public:

	OgreNewtonDemoApplication()
		:DemoApplication()
		,m_shootingTimer(0.25f)
		,m_playerManager(NULL)
	{
	}

	virtual ~OgreNewtonDemoApplication()
	{
	}

protected:
	void loadStaticScene ()
	{
		// create a scene body to add all static collidable meshes in the world 
		OgreNewtonSceneBody* const sceneBody = new OgreNewtonSceneBody (m_physicsWorld);

		// start adding collision shape to the scene body
		sceneBody->BeginAddRemoveCollision();

		// floor object!
		//Entity* const levelMap = mSceneMgr->createEntity(MakeName("Level"), "chiropteradm.mesh" );
		Entity* const levelMap = mSceneMgr->createEntity(MakeName("Level"), "castle.mesh" );

		SceneNode* const floorNode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "FloorNode" );
		floorNode->attachObject( levelMap );
		levelMap->setCastShadows( false );

		// add this collision to the scene body
		sceneBody->AddCollisionTree (floorNode);

		// done adding collision shape to the scene body, now optimize the scene
		sceneBody->EndAddRemoveCollision();
	}

	void LoadDynamicScene(const Vector3& origin)
	{
	}


	void createFrameListener()
	{
		// this is our custom frame listener for this app, that lets us shoot cylinders with the space bar, move the camera, etc.
		//		m_listener = new ApplicationFrameListener (mRoot, mWindow, mCamera, mSceneMgr, this, m_debugRender);
		//		mRoot->addFrameListener(m_listener);
	}

	void OnPhysicUpdateBegin(dFloat timestepInSecunds)
	{
		DemoApplication::OnPhysicUpdateBegin(timestepInSecunds);

		m_shootingTimer -= timestepInSecunds;
		if ((m_shootingTimer < 0.0f) && m_keyboard->isKeyDown(OIS::KC_SPACE)) {
			m_shootingTimer = 0.1f;

			Vector3 cameraPosit;
			Quaternion cameraRotation;
			GetInterpolatedCameraMatrix (cameraPosit, cameraRotation);

			int index = (rand() >> 3) % int (sizeof (m_shootingMesh) / sizeof (m_shootingMesh[0]));

			Entity* const ent = mSceneMgr->createEntity(MakeName ("shootObject"), m_shootingMesh[index]);
			SceneNode* const node = CreateNode (mSceneMgr, ent, cameraPosit, cameraRotation);
			Matrix4 matrix;
			matrix.makeTransform (cameraPosit, Vector3(1.0f, 1.0f, 1.0f), cameraRotation);
			OgreNewtonDynamicBody* const body = new OgreNewtonDynamicBody (m_physicsWorld, 30.0f, m_shootingCollisions[index], node, matrix);

			const Real speed = -40.0f;
			Vector3 veloc (Vector3 (matrix[0][2], matrix[1][2], matrix[2][2]) * speed);   
			body->SetVeloc(veloc);
		}

	}

	virtual void destroyScene()
	{
		for (int i = 0; i < int (sizeof (m_shootingCollisions) / sizeof (m_shootingCollisions[0])); i ++) {
			delete m_shootingCollisions[i];
		}
	}

	void createScene()
	{
		// create the physic world first
		DemoApplication::createScene();

		//make a light
		mSceneMgr->setAmbientLight(ColourValue(0.2f, 0.2f, 0.2f));

		Light* const light0 = mSceneMgr->createLight( "Light0" );
		Vector3 lightdir0(-0.55f, -0.5f, -0.75f);
		light0->setType(Light::LT_DIRECTIONAL);
		light0->setDirection(lightdir0);
		light0->setDiffuseColour(ColourValue(1.0f, 1.0f, 1.0f));
		light0->setSpecularColour(ColourValue(0.4f, 0.4f, 0.4f));


		Light* const light1 = mSceneMgr->createLight( "Light1" );
		Vector3 lightdir1(0.4f, -0.9f, 0.5f);
		light1->setType(Light::LT_DIRECTIONAL);
		light1->setDirection(lightdir1);
		light1->setDiffuseColour(ColourValue(1.0f, 1.0f, 1.0f));
		light1->setSpecularColour(ColourValue(0.4f, 0.4f, 0.4f));



		// sky box.
		//mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");
		mSceneMgr->setSkyBox(true, "Examples/MorningSkyBox");

		// load all of the static geometry
		loadStaticScene ();

		// position camera using the ray cast functionality
		Vector3 start(-1.0f,  1000.0f, 4.0f);
		Vector3 end  (-1.0f, -1000.0f, 4.0f);
		OgreNewtonRayCast raycaster(m_physicsWorld); 
		raycaster.CastRay (&start.x, &end.x);

		Vector3 origin (raycaster.m_contact + Vector3 (0.0f, 1.0f, 0.0f));
		mCamera->setPosition(origin);

		// set the near and far clip plane
		mCamera->setNearClipDistance(0.1f);
		mCamera->setFarClipDistance(10000.0f);

		// now load the dynamics Scene
		LoadDynamicScene(origin);

		// create shutting components
		CreateComponentsForShutting(sizeof (m_shootingCollisions)/sizeof (m_shootingCollisions[0]), m_physicsWorld, m_shootingCollisions, m_shootingMesh);

		// initialize the Camera position after the scene was loaded
		ResetCamera (mCamera->getPosition(), mCamera->getOrientation());

		// create a player manager for controll all players
		m_playerManager = new OgreNewtonPlayerManager (m_physicsWorld);

OgreNewtonPlayerManager::OgreNetwonPlayer* player = new OgreNewtonPlayerManager::OgreNetwonPlayer(m_playerManager, NULL, 200.0f, 1.0f, 0.25f, 2.0f, 0.5f);

	}

	Real m_shootingTimer;
	MeshPtr m_shootingMesh[2];
	dNewtonCollision* m_shootingCollisions[2];

	OgreNewtonPlayerManager* m_playerManager;

};



INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
	OgreNewtonDemoApplication application;

	try {
		application.go();
	} catch(Exception &e) {
		MessageBox(NULL, e.getFullDescription().c_str(), "Well, this is embarrassing.. an Ogre exception has occurred.", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}

	return 0;
}




 
 
