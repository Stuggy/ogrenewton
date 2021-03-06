
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


#include <HelpersStdAfx.h>
#include "Utils.h"
#include "Forklift.h"
#include "LumberYard.h"
#include "ScreenWriter.h"
#include "ShootRigidBody.h"
#include "DemoApplication.h"


#define ARTICULATED_VEHICLE_CAMERA_DISTANCE			10.0f
#define ARTICULATED_VEHICLE_CAMERA_HIGH_ABOVE_HEAD	2.5f

using namespace Ogre;

class OgreNewtonDemoApplication: public DemoApplication
{
	public:
	OgreNewtonDemoApplication()
		:DemoApplication()
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

		// floor object
		Entity* const floor = mSceneMgr->createEntity(MakeName("Floor"), "flatplane.mesh" );		
		//Entity* const floor = mSceneMgr->createEntity(MakeName("Floor"), "playground.mesh" );

		SceneNode* const floorNode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "FloorNode" );
		floorNode->attachObject( floor );
		floor->setCastShadows( false );

		// add this collision to the scene body
		sceneBody->AddCollisionTree (floorNode);

		// done adding collision shape to the scene body, now optimize the scene
		sceneBody->EndAddRemoveCollision();
	}

	void SpawnRegularScaledCollisionShape (int count, const Vector3& origin, const dNewtonCollision& shape)
	{
		TexturePtr texture = Ogre::TextureManager::getSingleton().load("wood.tga", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		// make a material to use with this mesh
		MaterialPtr renderMaterial = MaterialManager::getSingleton().create("spawnMaterial", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		renderMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(true);
		renderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("wood.tga");
		renderMaterial->setAmbient(1.0f, 1.0f, 1.0f);

		OgreNewtonMesh mesh (&shape);
		mesh.Triangulate();
		int materialId = mesh.AddMaterial(renderMaterial);
		mesh.ApplyBoxMapping(materialId, materialId, materialId);

		ManualObject* const object = mesh.CreateEntity(MakeName ("spawnMesh"));
		MeshPtr visualMesh (object->convertToMesh (MakeName ("spawnMesh")));
		delete object;

		Real scaleStep = (2.0f - 0.5f) / count;
		Real scaleY = 0.5f;

		for (int i = 0; i < count; i ++) {
			Matrix4 matrix;
			Vector3 posit (origin + Vector3 (0.0f, 0.0f, -i * 4.0f));

			// make a ogre node
			Entity* const ent = mSceneMgr->createEntity(MakeName ("spawnMesh"), visualMesh);
			SceneNode* const node = CreateNode (mSceneMgr, ent, posit, Quaternion::IDENTITY);
			matrix.makeTransform (posit, Vector3(1.0f, 1.0f, 1.0f), Quaternion::IDENTITY);

			// make a dynamic body
			OgreNewtonDynamicBody* const body = new OgreNewtonDynamicBody (m_physicsWorld, 30.0f, &shape, node, matrix);

			// apply non uniform scale to both 
			dNewtonCollision* const collision = body->GetCollision();
			node->setScale(1.0f, scaleY, 1.0f);
			collision->SetScale(1.0f, scaleY, 1.0f);

			scaleY += scaleStep;
		}
	}


	void LoadDynamicScene (const Vector3& origin)
	{
		const int compoundCount = 1;

		// some automatically generated convex decompositions
		Vector3 start(origin + Vector3 (0.0f, 100.0f, 0.0f));
		Vector3 end (origin - Vector3 (0.0f, 100.0f, 0.0f));
		OgreNewtonRayCast raycaster(m_physicsWorld, m_rayCast); 
		raycaster.CastRay (&start.x, &end.x);
		LumberYard (mSceneMgr, m_physicsWorld, raycaster.m_contact, compoundCount, compoundCount, 10);
	}									  

	void createFrameListener()
	{
		DemoApplication::createFrameListener();
	}

	void OnPhysicUpdateBegin(dFloat timestepInSecunds)
	{
		DemoApplication::OnPhysicUpdateBegin(timestepInSecunds);
		const OgreNewtonInputManager::OgrePlayerUserDataPair& playerData = m_physicsWorld->GetInputManager()->GetPlayer();

		dNewtonDynamicBody* const playerBody = playerData.m_player;
		ForkliftPhysicsModel* const playerController = (ForkliftPhysicsModel*)playerData.m_userData;

		// set all of the player inputs
		ForkliftPhysicsModel::InputRecored inputs;
		inputs.m_throtler = (m_keyboard->isKeyDown(OIS::KC_W) - m_keyboard->isKeyDown(OIS::KC_S));
		inputs.m_steering = (m_keyboard->isKeyDown(OIS::KC_A) - m_keyboard->isKeyDown(OIS::KC_D));
		inputs.m_lift = (m_keyboard->isKeyDown(OIS::KC_Q) - m_keyboard->isKeyDown(OIS::KC_E));
		inputs.m_tilt = (m_keyboard->isKeyDown(OIS::KC_X) - m_keyboard->isKeyDown(OIS::KC_Z));
		inputs.m_palette = (m_keyboard->isKeyDown(OIS::KC_F) - m_keyboard->isKeyDown(OIS::KC_G));


		// check if there are some vehicle input, if there is, then wakeup the vehicle
		if (m_keyboard->isKeyDown(OIS::KC_W) || 
			m_keyboard->isKeyDown(OIS::KC_S) || 
			m_keyboard->isKeyDown(OIS::KC_A) || 
			m_keyboard->isKeyDown(OIS::KC_D) ||	
			m_keyboard->isKeyDown(OIS::KC_F) ||	
			m_keyboard->isKeyDown(OIS::KC_G) ||	
			m_keyboard->isKeyDown(OIS::KC_Z) ||	
			m_keyboard->isKeyDown(OIS::KC_C) ||	
			m_keyboard->isKeyDown(OIS::KC_Q) ||	
			m_keyboard->isKeyDown(OIS::KC_E)) {
			playerBody->SetSleepState(false);
		}

		playerController->ApplyInputs (inputs);
	}


	void OnPhysicUpdateEnd(dFloat timestepInSecunds)
	{
		DemoApplication::OnPhysicUpdateEnd (timestepInSecunds);

		OgreNewtonInputManager* const inputManager = m_physicsWorld->GetInputManager();
		OgreNewtonDynamicBody* const player = (OgreNewtonDynamicBody*) inputManager->GetPlayer().m_player;

		// reposition the camera origin to point to the player
		Matrix4 camMatrix(GetCameraTransform());

		Matrix4 playerMatrix(player->GetMatrix());
		Vector3 frontDir (camMatrix[0][2], camMatrix[1][2], camMatrix[2][2]);
		Vector3 camOrigin (playerMatrix.transformAffine(Vector3(0.0f, ARTICULATED_VEHICLE_CAMERA_HIGH_ABOVE_HEAD, 0.0f)));
		camOrigin += frontDir * ARTICULATED_VEHICLE_CAMERA_DISTANCE;
		camMatrix.setTrans(camOrigin); 

		SeCameraTransform (camMatrix);
	}


	void OnRenderUpdateEnd(dFloat updateParam)
	{
		DemoApplication::OnRenderUpdateEnd(updateParam);
		if (m_onScreeHelp.m_state) {
			int row = 0;
			const RenderTarget::FrameStats& stats = mWindow->getStatistics();
			row = m_screen->write(20, row + 20, "FPS:  %05.3f", stats.lastFPS);
			row = m_screen->write(20, row + 20, "Physics time:  %05.3f ms", float (double (m_physicsWorld->GetPhysicsTimeInMicroSeconds()) * 1.0e-3f));
			row = m_screen->write(20, row + 20, "Number of rigid bodies:  %d", m_physicsWorld->GetBodyCount());
			row = m_screen->write(20, row + 20, "F1:  Hide debug help text");
			row = m_screen->write(20, row + 20, "F2:  Toggle %s simulation update", m_physicsWorld->GetConcurrentUpdateMode() ? "Asynchronous" : "Synchronous");
			row = m_screen->write(20, row + 20, "F3:  Toggle display physic debug");
			row = m_screen->write(20, row + 30, "W, S, A, D:  Drive Vehicle");
			row = m_screen->write(20, row + 20, "Q E:  Rise and lower Lifter Apparatus");
			row = m_screen->write(20, row + 20, "Z X:  Rotate Lifter Apparatus");
			row = m_screen->write(20, row + 20, "Hold CTRL and Left Mouse Key:  Show mouse cursor and pick objects from the screen");
			row = m_screen->write(20, row + 20, "ESC:  Exit application");
		}
		m_screen->update();
	}


	virtual void destroyScene()
	{
	}

	void createScene()
	{
		// create the physic world first
		DemoApplication::createScene();

		// sky box.
		//mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");
		mSceneMgr->setSkyBox(true, "Examples/MorningSkyBox");

		// load all of the static geometry
		loadStaticScene ();

		// position camera using the ray cast functionality
		Vector3 start(0.0f, 1000.0f, 10.0f);
		Vector3 end(0.0f, -1000.0f, 10.0f);
		OgreNewtonRayCast raycaster(m_physicsWorld, m_rayCast); 
		raycaster.CastRay (&start.x, &end.x);

		Vector3 origin (raycaster.m_contact + Vector3 (0.0f, 2.0f, 0.0f));
		mCamera->setPosition(origin);

		// set the near and far clip plane
		mCamera->setNearClipDistance(0.1f);
		mCamera->setFarClipDistance(10000.0f);
		//mCamera->setPolygonMode(Ogre::PM_WIREFRAME);

		// now load the dynamics Scene
		LoadDynamicScene(origin + Vector3 (0.0f, 0.0f, -30.0f));

		// load articulated ForkLift
		ForkliftPhysicsModel* const forkLift = new ForkliftPhysicsModel(this, "forklift.scene", raycaster.m_contact + Vector3 (0.0f, 0.5f, 10.0f), "lifter1_");

		// set this object as the player
		m_physicsWorld->GetInputManager()->SetPlayer(forkLift->m_rootBody, forkLift);

		// add a second vehicle just because we can
//		new ForkliftPhysicsModel(this, "forklift.scene", raycaster.m_contact + Vector3 (4.0f, 0.5f, 10.0f), "lifter2_");

		// initialize the Camera position after the scene was loaded
		ResetCamera (mCamera->getPosition(), mCamera->getOrientation());

		//set light after the scene is loaded 
		mSceneMgr->setAmbientLight(ColourValue(0.3f, 0.3f, 0.3f));

		Light* const light0 = mSceneMgr->createLight( "Light0" );
		Vector3 lightdir(-0.3f, -0.7f, -0.5f);
		light0->setType(Light::LT_DIRECTIONAL);
		light0->setDirection(lightdir);
		light0->setDiffuseColour(ColourValue(0.9f, 0.9f, 0.9f));
		light0->setSpecularColour(ColourValue(0.4f, 0.4f, 0.4f));
	}

//	OgreNewtonDynamicBody* m_player;
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

