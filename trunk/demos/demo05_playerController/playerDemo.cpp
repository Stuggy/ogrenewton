
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
#include <OgreNewtonArticulatedTransformManager.h>

#include "Utils.h"
#include "PlayerExample.h"
#include "ShootRigidBody.h"
#include "DemoApplication.h"


#define PLAYER_SPEED					4.0f
#define ARTICULATED_VEHICLE_CAMERA_DISTANCE			6.0f
#define ARTICULATED_VEHICLE_CAMERA_HIGH_ABOVE_HEAD	0.5f


using namespace Ogre;

class OgreNewtonDemoApplication: public DemoApplication
{
	public:

	OgreNewtonDemoApplication()
		:DemoApplication()
		,m_shootRigidBody(NULL)
		,m_player(NULL)
		,m_localTransformManager(NULL)
	{
	}

	virtual ~OgreNewtonDemoApplication()
	{
	}

	protected:
	void loadStaticScene ()
	{
		// create a scene body to add all static collidable meshes in the world 
		OgreNewtonSceneBody* const sceneBody = new OgreNewtonSceneBody (m_physicsWorld, m_allExcludingMousePick);

		// start adding collision shape to the scene body
		sceneBody->BeginAddRemoveCollision();

		// floor object!
		//Entity* const levelMap = mSceneMgr->createEntity(MakeName("Floor"), "flatplane.mesh" );		
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

	void OnPhysicUpdateBegin (dFloat timestepInSecunds)
	{
		// bypass the call to DemoApplication::OnPhysicUpdateBegin
		DemoApplication::OnPhysicUpdateBegin(timestepInSecunds);

		// handle shotting objects
		if (m_keyboard->isKeyDown(OIS::KC_SPACE)) {
			dNewton::ScopeLock lock (&m_scopeLock);
			m_shootRigidBody->ShootRandomBody (this, mSceneMgr, timestepInSecunds);
		}

		// update main player first
		m_player->ApplyPlayerInputs (this, timestepInSecunds);

		// update all players controller 
		OgreNewtonPlayerManager* const playerManager = m_physicsWorld->GetPlayerManager();
		for (MyPlayerContyroller* player = (MyPlayerContyroller*) playerManager->GetFirstPlayer(); player; player = (MyPlayerContyroller*) playerManager->GetNextPlayer(player)) {
			if (player != m_player) {
				player->ApplyNPCInputs (this, timestepInSecunds);
			}
		}
	}

	void OnPhysicUpdateEnd(dFloat timestepInSecunds)
	{
		DemoApplication::OnPhysicUpdateEnd (timestepInSecunds);

		// reposition the camera origin to point to the player
		Matrix4 camMatrix(GetCameraTransform());

		Matrix4 playerMatrix;
		m_player->GetMatrix (&playerMatrix[0][0]);
		playerMatrix = playerMatrix.transpose();

		Vector3 frontDir (camMatrix[0][2], camMatrix[1][2], camMatrix[2][2]);
		Vector3 camOrigin (playerMatrix.transformAffine(Vector3(0.0f, m_player->GetPlayerHigh() + ARTICULATED_VEHICLE_CAMERA_HIGH_ABOVE_HEAD, 0.0f)));
		camOrigin += frontDir * ARTICULATED_VEHICLE_CAMERA_DISTANCE;
		camMatrix.setTrans(camOrigin); 

		SeCameraTransform (camMatrix);
	}

	virtual void destroyScene()
	{
		delete (m_shootRigidBody);
	}

	MyPlayerContyroller* CreatePlayer()
	{
		// create main model
		Entity* const playerMesh = mSceneMgr->createEntity(MakeName("SinbadBody"), "Sinbad.mesh");
		SceneNode* const playerNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(MakeName("SinbadBody"));
		playerNode->attachObject(playerMesh);

		// the model is about 10 meters hight, calculate the scale to make the play 2 meter high
		playerNode->_updateBounds();
		AxisAlignedBox bBox (playerNode->_getWorldAABB());
		Real high = bBox.getMaximum().y - bBox.getMinimum().y;
		Real scale = 2.0f / high;
		playerNode->setScale(scale, scale, scale);
/*
		// create swords and attach to sheath
		LogManager::getSingleton().logMessage("Creating swords");
		mSword1 = sceneMgr->createEntity("SinbadSword1", "Sword.mesh");
		mSword2 = sceneMgr->createEntity("SinbadSword2", "Sword.mesh");
		mBodyEnt->attachObjectToBone("Sheath.L", mSword1);
		mBodyEnt->attachObjectToBone("Sheath.R", mSword2);

		LogManager::getSingleton().logMessage("Creating the chains");
		// create a couple of ribbon trails for the swords, just for fun
		NameValuePairList params;
		params["numberOfChains"] = "2";
		params["maxElements"] = "80";
		mSwordTrail = (RibbonTrail*)sceneMgr->createMovableObject("RibbonTrail", &params);
		mSwordTrail->setMaterialName("Examples/LightRibbonTrail");
		mSwordTrail->setTrailLength(20);
		mSwordTrail->setVisible(false);
		sceneMgr->getRootSceneNode()->attachObject(mSwordTrail);
		for (int i = 0; i < 2; i++)
		{
			mSwordTrail->setInitialColour(i, 1, 0.8, 0);
			mSwordTrail->setColourChange(i, 0.75, 1.25, 1.25, 1.25);
			mSwordTrail->setWidthChange(i, 1);
			mSwordTrail->setInitialWidth(i, 0.5);
		}

		mKeyDirection = Vector3::ZERO;
		mVerticalVelocity = 0;
*/

		high = 2.0f;
		Real mass = 200.0f;
		Real stairStep = 0.25f * high;
		Real outerRadius = scale * (bBox.getMaximum().z - bBox.getMinimum().z) * 0.4f;
		Real innerRadius = outerRadius * 0.25f;
		Vector3 playerPivotOffset (0.0f, - scale * bBox.getMinimum().y, 0.0f);
		Quaternion aligmentRot (Radian (Degree (180.0f)), Vector3 (0.0f, 1.0f, 0.0f));

		OgreNewtonPlayerManager* const playerManager = m_physicsWorld->GetPlayerManager();
		MyPlayerContyroller* const player = new MyPlayerContyroller(playerManager, playerMesh, playerNode, mass, outerRadius, innerRadius, high, stairStep, playerPivotOffset, aligmentRot, PLAYER_SPEED);

		return player;
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
		Vector3 start(1.0f,  1000.0f, 4.0f);
		Vector3 end  (1.0f, -1000.0f, 4.0f);
		OgreNewtonRayCast raycaster(m_physicsWorld, m_rayCast); 
		raycaster.CastRay (&start.x, &end.x);

		Vector3 origin (raycaster.m_contact + Vector3 (0.0f, 1.0f, 0.0f));
		mCamera->setPosition(origin);

		// set the near and far clip plane
		mCamera->setNearClipDistance(0.1f);
		mCamera->setFarClipDistance(10000.0f);

		// now load the dynamics Scene
		LoadDynamicScene(origin);

		// create shutting components
		m_shootRigidBody = new ShootRigidBody(m_physicsWorld);

		// initialize the Camera position after the scene was loaded
		ResetCamera (mCamera->getPosition(), mCamera->getOrientation());


		// add some players
		m_player = CreatePlayer();
	}

	ShootRigidBody* m_shootRigidBody;
	MyPlayerContyroller* m_player;
	
	OgreNewtonArticulatedTransformManager* m_localTransformManager;
};


INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
	OgreNewtonDemoApplication application;

	try {
		application.go();
	} catch(Exception &e) {
		MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}

	return 0;
}




 
 

