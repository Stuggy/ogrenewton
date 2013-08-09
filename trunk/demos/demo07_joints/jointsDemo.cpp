
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
#include <OgreNewtonRayPickManager.h>
#include <OgreNewtonExampleApplication.h>
#include <OgreNewtonHierarchyTransformManager.h>

#include "Utils.h"
#include "forklift.h"
#include "ScreenWriter.h"
#include "ShootRigidBody.h"
#include "DemoApplication.h"


#define PLAYER_CAMERA_DISTANCE			10.0f
#define PLAYER_CAMERA_HIGHT_ABOVE_HEAD	2.5f

using namespace Ogre;

class OgreNewtonDemoApplication: public DemoApplication
{
	public:
	OgreNewtonDemoApplication()
		:DemoApplication()
		,m_player(NULL)
		,m_shootRigidBody(NULL)
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


	void LoadWoodPallete (int count, const Vector3& origin)
	{
// for now do no place any object until I finish the CCD for compound collision
return;
		Real scale = 0.25f;
		Entity* const entity = mSceneMgr->createEntity(MakeName("compound"), "woodPallet.mesh");		
		entity->setCastShadows (true);

		// convert the ogre entity to a newton mesh
		OgreNewtonMesh mesh (m_physicsWorld, entity);
		mesh.ApplyTransform(Vector3 (0.0f, 0.0f, 0.0f), Vector3 (scale, scale, scale), Quaternion::IDENTITY);

		// make a convex approximation for the newton mesh 
		OgreNewtonMesh convexAproximation (m_physicsWorld);
		convexAproximation.CreateApproximateConvexDecomposition(mesh, 0.01f, 0.2f, 32, 100);

		// now make a compound collision form the convex approximation
		dNewtonCollisionCompound compoundShape (m_physicsWorld, convexAproximation, m_all);

		const int stackHigh = 1;

		// now use this shape a place few instances in the world
		MeshPtr visualMesh = entity->getMesh();
		for (int j = 0; j < count; j ++) {
			for (int i = 0; i < count; i ++) {
				for (int k = 0; k < stackHigh; k ++) {
					Matrix4 matrix;
					Vector3 posit (origin + Vector3 (i * 4.0f, k * 0.2f, j * 4.0f));

					// make a ogre node
					Entity* const ent = mSceneMgr->createEntity(MakeName ("spawnMesh"), visualMesh);
					SceneNode* const node = CreateNode (mSceneMgr, ent, posit, Quaternion::IDENTITY);
					node->setScale(Vector3 (scale, scale, scale));
					matrix.makeTransform (posit, Vector3(1.0f, 1.0f, 1.0f), Quaternion::IDENTITY);

					// make a dynamic body
					OgreNewtonDynamicBody* const body = new OgreNewtonDynamicBody (m_physicsWorld, 30.0f, &compoundShape, node, matrix);

					// the collision sub pasts are too small, set continue collision mode 
					body->SetCCDMode (true);
				}
			}
		}
	}


	void LoadDynamicScene (const Vector3& origin)
	{
		const int compoundCount = 5;

		// some automatically generated convex decompositions
		Vector3 start(origin + Vector3 (0.0f, 100.0f, 0.0f));
		Vector3 end (origin - Vector3 (0.0f, 100.0f, 0.0f));
		OgreNewtonRayCast raycaster(m_physicsWorld, m_rayCast); 
		raycaster.CastRay (&start.x, &end.x);
		LoadWoodPallete (compoundCount, raycaster.m_contact + Vector3 (-10.0f, 0.25f, -50.0f));


		// make a convex hull from 200 random points
		dNewtonScopeBuffer<Vector3> points(200);
		for (int i = 0; i < points.GetElementsCount(); i ++) {
			points[i] = Vector3 (Rand (0.5f), Rand (0.5f), Rand (0.5f));
		}
		
		// add samples the single solid primitive with non uniform scaling
//		const int spawnCount = 20;
//		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 (-16.0f, 4.0f, -10.0f), dNewtonCollisionSphere (m_physicsWorld, 0.5f, m_all));
//		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 (-12.0f, 4.0f, -10.0f), dNewtonCollisionBox (m_physicsWorld, 0.5f, 0.5f, 0.5f, m_all));
//		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 ( -8.0f, 4.0f, -10.0f), dNewtonCollisionCapsule (m_physicsWorld, 0.25f, 0.5f, 0));
//		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 ( -4.0f, 4.0f, -10.0f), dNewtonCollisionTaperedCapsule (m_physicsWorld, 0.25f, 0.35f, 0.75f, m_all));
//		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 (  0.0f, 4.0f, -10.0f), dNewtonCollisionCone (m_physicsWorld, 0.25f, 0.75f, m_all));
//		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 (  4.0f, 4.0f, -10.0f), dNewtonCollisionCylinder (m_physicsWorld, 0.25f, 0.75f, m_all));
//		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 (  8.0f, 4.0f, -10.0f), dNewtonCollisionTaperedCylinder (m_physicsWorld, 0.25f, 0.35f, 0.75f, m_all));
//		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 ( 12.0f, 4.0f, -10.0f), dNewtonCollisionChamferedCylinder (m_physicsWorld, 0.25f, 0.75f, m_all));
//		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 ( 16.0f, 4.0f, -10.0f), dNewtonCollisionConvexHull (m_physicsWorld, points.GetElementsCount(), &points[0].x, sizeof (Vector3), 0.0f, m_all));
	}									  

	void createFrameListener()
	{
	}

	void OnPhysicUpdateBegin(dFloat timestepInSecunds)
	{
		DemoApplication::OnPhysicUpdateBegin(timestepInSecunds);
		m_shootRigidBody->ShootRandomBody (this, mSceneMgr, timestepInSecunds);

		// check if there are some vehicle input, if there is, then wakeup the vehicle
		m_keyboard->capture();
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
			m_player->SetSleepState(false);
		}
	}


	void OnPhysicUpdateEnd(dFloat timestepInSecunds)
	{
		DemoApplication::OnPhysicUpdateEnd (timestepInSecunds);

		// reposition the camera origin to point to the player
		Matrix4 camMatrix;
		m_cameraTransform.GetTargetMatrix (&camMatrix[0][0]);
		camMatrix = camMatrix.transpose();

		Matrix4 playerMatrix(m_player->GetMatrix());
		Vector3 frontDir (camMatrix[0][2], camMatrix[1][2], camMatrix[2][2]);
		Vector3 camOrigin (playerMatrix.transformAffine(Vector3(0.0f, PLAYER_CAMERA_HIGHT_ABOVE_HEAD, 0.0f)));
		camOrigin += frontDir * PLAYER_CAMERA_DISTANCE;
		camMatrix.setTrans(camOrigin); 

		camMatrix = camMatrix.transpose();
		m_cameraTransform.SetTargetMatrix (&camMatrix[0][0]);
	}


	bool OnRenderUpdateEnd(dFloat updateParam)
	{
		DemoApplication::OnRenderUpdateEnd(updateParam);
		if (m_onScreeHelp.m_state) {
			m_screen->write(20,  80, "F1:  Hide debug help text");
			m_screen->write(20, 100, "F3:  Toggle display physic debug");
			m_screen->write(20, 120, "W, S, A, D:  Drive Vehicle");
			m_screen->write(20, 140, "Q E:  Rise and lower Lifter Apparatus");
			m_screen->write(20, 160, "Z C:  Rotate Lifter Apparatus");
			m_screen->write(20, 180, "Hold CTRL and Left Mouse Key:  Show mouse cursor and pick objects from the screen");
			m_screen->write(20, 120, "ESC:  Exit application");
		}
		m_screen->update();
		return true;
	}


	virtual void destroyScene()
	{
		delete (m_shootRigidBody);
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
		LoadDynamicScene(origin);

		// load articulated ForkLift
		ForkliftPhysicsModel* const forkLift = new ForkliftPhysicsModel(this, "forklift.scene", raycaster.m_contact + Vector3 (0.0f, 0.5f, -10.0f));
		m_player = forkLift->m_rootBody;

		// create shutting components
		m_shootRigidBody = new ShootRigidBody(m_physicsWorld);

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

	ShootRigidBody* m_shootRigidBody;
	OgreNewtonDynamicBody* m_player;
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

