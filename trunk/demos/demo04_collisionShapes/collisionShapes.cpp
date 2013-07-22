
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

#include "Utils.h"
#include "BuildJenga.h"
#include "BuildPyramid.h"
#include "ShootRigidBody.h"
#include "DemoApplication.h"


using namespace Ogre;

class OgreNewtonDemoApplication: public DemoApplication
{
	public:

	OgreNewtonDemoApplication()
		:DemoApplication()
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
		OgreNewtonSceneBody* const sceneBody = new OgreNewtonSceneBody (m_physicsWorld);

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


	void SpawnManualCompoundCollisionShapes (int count, const Vector3& origin)
	{
		// make a long Box 
		dNewtonCollisionBox boxShape (m_physicsWorld, 2.0f, 0.25f, 0.25f, 0);

		// make a compound and add three instance of the box to make a Cruz 
		dNewtonCollisionCompound compoundShape (m_physicsWorld, 0);
		compoundShape.BeginAddRemoveCollision();

			Matrix4 matrix (Matrix4::IDENTITY);
			matrix = matrix.transpose();
			boxShape.SetMatrix(&matrix[0][0]);
			compoundShape.AddCollision(&boxShape);

			matrix.makeTransform (Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), Quaternion (Radian (Degree(90.0f)), Vector3 (0.0f, 1.0f, 0.0f)));
			matrix = matrix.transpose();
			boxShape.SetMatrix(&matrix[0][0]);
			compoundShape.AddCollision(&boxShape);

			matrix.makeTransform (Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), Quaternion (Radian (Degree(90.0f)), Vector3 (0.0f, 0.0f, 1.0f)));
			matrix = matrix.transpose();
			boxShape.SetMatrix(&matrix[0][0]);
			compoundShape.AddCollision(&boxShape);

		compoundShape.EndAddRemoveCollision();

		// create a Ogre mesh for this compound shape
		TexturePtr texture = Ogre::TextureManager::getSingleton().load("wood.tga", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		// make a material to use with this mesh
		MaterialPtr renderMaterial = MaterialManager::getSingleton().create("spawnMaterial", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		renderMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(true);
		renderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("wood.tga");
		renderMaterial->setAmbient(1.0f, 1.0f, 1.0f);

		OgreNewtonMesh mesh (&compoundShape);
		mesh.Triangulate();
		int materialId = mesh.AddMaterial(renderMaterial);
		mesh.ApplyBoxMapping(materialId, materialId, materialId);

		ManualObject* const object = mesh.CreateEntity(MakeName ("spawnMesh"));
		MeshPtr visualMesh (object->convertToMesh (MakeName ("spawnMesh")));
		delete object;

		// now use this shape a place few instances in the world
		for (int i = 0; i < count; i ++) {
			Matrix4 matrix;
			Vector3 posit (origin + Vector3 (i * 4.0f, 0.0f, 0.0f));

			// make a ogre node
			Entity* const ent = mSceneMgr->createEntity(MakeName ("spawnMesh"), visualMesh);
			SceneNode* const node = CreateNode (mSceneMgr, ent, posit, Quaternion::IDENTITY);
			matrix.makeTransform (posit, Vector3(1.0f, 1.0f, 1.0f), Quaternion::IDENTITY);

			// make a dynamic body
			new OgreNewtonDynamicBody (m_physicsWorld, 30.0f, &compoundShape, node, matrix);
		}
	}


	void SpawnAutomaticCompoundCollisionShapes (int count, const Vector3& origin, const char* const meshName)
	{
		Entity* const entity = mSceneMgr->createEntity(MakeName("compound"), meshName);		
		entity->setCastShadows (true);

		// convert the ogre entity to a newton mesh
		OgreNewtonMesh mesh (m_physicsWorld, entity);

		// make a convex approximation for the newton mesh 
		OgreNewtonMesh convexAproximation (m_physicsWorld);
		convexAproximation.CreateApproximateConvexDecomposition(mesh, 0.01f, 0.2f, 32, 100);

		// now make a compound collision form the convex approximation
		dNewtonCollisionCompound compoundShape (m_physicsWorld, convexAproximation, 0);

		// now use this shape a place few instances in the world
		MeshPtr visualMesh = entity->getMesh();
		for (int i = 0; i < count; i ++) {
			Matrix4 matrix;
			Vector3 posit (origin + Vector3 (i * 4.0f, 0.0f, 0.0f));

			// make a ogre node
			Entity* const ent = mSceneMgr->createEntity(MakeName ("spawnMesh"), visualMesh);
			SceneNode* const node = CreateNode (mSceneMgr, ent, posit, Quaternion::IDENTITY);
			matrix.makeTransform (posit, Vector3(1.0f, 1.0f, 1.0f), Quaternion::IDENTITY);

			// make a dynamic body
			new OgreNewtonDynamicBody (m_physicsWorld, 30.0f, &compoundShape, node, matrix);
		}
	}

	void LoadDynamicScene(const Vector3& origin)
	{
		const int compoundCount = 10;

		// add some compound collision shapes
		SpawnManualCompoundCollisionShapes    (10, origin + Vector3 (-20.0f, 1.0f, -40.0f));

		// some automatically generated convex decompositions
		SpawnAutomaticCompoundCollisionShapes (compoundCount, origin + Vector3 (-20.0f, 1.0f, -50.0f), "cow.mesh");
		SpawnAutomaticCompoundCollisionShapes (compoundCount, origin + Vector3 (-20.0f, 1.0f, -60.0f), "bessel.mesh");
		SpawnAutomaticCompoundCollisionShapes (compoundCount, origin + Vector3 (-20.0f, 1.0f, -70.0f), "torus.mesh");
		SpawnAutomaticCompoundCollisionShapes (compoundCount, origin + Vector3 (-20.0f, 1.0f, -80.0f), "torusKnot.mesh");


		// make a convex hull from 200 random points
		dNewtonScopeBuffer<Vector3> points(200);
		for (int i = 0; i < points.GetElementsCount(); i ++) {
			points[i] = Vector3 (Rand (0.5f), Rand (0.5f), Rand (0.5f));
		}
		
		// add samples the single solid primitive with non uniform scaling
		const int spawnCount = 20;
		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 (-16.0f, 4.0f, -10.0f), dNewtonCollisionSphere (m_physicsWorld, 0.5f, 0));
		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 (-12.0f, 4.0f, -10.0f), dNewtonCollisionBox (m_physicsWorld, 0.5f, 0.5f, 0.5f, 0));
		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 ( -8.0f, 4.0f, -10.0f), dNewtonCollisionCapsule (m_physicsWorld, 0.25f, 0.5f, 0));
		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 ( -4.0f, 4.0f, -10.0f), dNewtonCollisionTaperedCapsule (m_physicsWorld, 0.25f, 0.35f, 0.75f, 0));
		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 (  0.0f, 4.0f, -10.0f), dNewtonCollisionCone (m_physicsWorld, 0.25f, 0.75f, 0));
		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 (  4.0f, 4.0f, -10.0f), dNewtonCollisionCylinder (m_physicsWorld, 0.25f, 0.75f, 0));
		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 (  8.0f, 4.0f, -10.0f), dNewtonCollisionTaperedCylinder (m_physicsWorld, 0.25f, 0.35f, 0.75f, 0));
		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 ( 12.0f, 4.0f, -10.0f), dNewtonCollisionChamferedCylinder (m_physicsWorld, 0.25f, 0.75f, 0));
		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 ( 16.0f, 4.0f, -10.0f), dNewtonCollisionConvexHull (m_physicsWorld, points.GetElementsCount(), &points[0].x, sizeof (Vector3), 0.0f, 0));
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

		m_shootRigidBody->ShootRandomBody (this, mSceneMgr, timestepInSecunds);
	}

	virtual void destroyScene()
	{
		delete (m_shootRigidBody);
	}

	void createScene()
	{
		// create the physic world first
		DemoApplication::createScene();

		//make a light
		mSceneMgr->setAmbientLight(ColourValue(0.2f, 0.2f, 0.2f));

		Light* const light0 = mSceneMgr->createLight( "Light0" );
		Vector3 lightdir(-0.55f, -0.3f, -0.75f);
		light0->setType(Light::LT_DIRECTIONAL);
		light0->setDirection(lightdir);
		light0->setDiffuseColour(ColourValue(1.0f, 1.0f, 1.0f));
		light0->setSpecularColour(ColourValue(0.4f, 0.4f, 0.4f));

		// sky box.
		//mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");
		mSceneMgr->setSkyBox(true, "Examples/MorningSkyBox");

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
		//mCamera->setPolygonMode(Ogre::PM_WIREFRAME);

		// now load the dynamics Scene
		LoadDynamicScene(origin);

		// create shutting components
		m_shootRigidBody = new ShootRigidBody(m_physicsWorld);

		// initialize the Camera position after the scene was loaded
		ResetCamera (mCamera->getPosition(), mCamera->getOrientation());
	}

	ShootRigidBody* m_shootRigidBody;
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

