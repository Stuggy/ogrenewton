
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
#include "OgreDotScene.h"
#include "ShootRigidBody.h"
#include "DemoApplication.h"


#define PLAYER_CAMERA_DISTANCE			10.0f
#define PLAYER_CAMERA_HIGHT_ABOVE_HEAD	2.5f

using namespace Ogre;

class OgreNewtonDemoApplication: public DemoApplication
{
	public:
	class ForkliftBaseActuator: public dNewtonHingeActuator
	{
		public:
		ForkliftBaseActuator (const dFloat* const pinAndPivotFrame, dNewtonDynamicBody* const base, dNewtonDynamicBody* const mainBody, OgreNewtonDemoApplication* const application)
			:dNewtonHingeActuator (pinAndPivotFrame, -30.0f * 3.141592f / 180.0f, -30.0f * 3.141592f / 180.0f, base, mainBody)
			,m_base(base)
			,m_mainBody(mainBody)
			,m_application(application)
		{
		}

		dNewtonDynamicBody* const m_base;
		dNewtonDynamicBody* const m_mainBody;
		OgreNewtonDemoApplication* m_application;
	};
	
	class ForkliftFrontTireJoint: public dNewtonHingeJoint
	{
		ForkliftFrontTireJoint (const dFloat* const pinAndPivotFrame, dNewtonDynamicBody* const tire, dNewtonDynamicBody* const mainBody, OgreNewtonDemoApplication* const application)
			:dNewtonHingeJoint (pinAndPivotFrame, tire, mainBody)
			,m_tire(tire)
			,m_mainBody(mainBody)
			,m_application(application)
		{
			// calculate the maximum torque that the engine will produce
			dNewtonCollision* const tireShape = tire->GetCollision();
			dAssert (tireShape->GetType() == dNewtonCollision::m_chamferedCylinder);

			Vector3 p0;
			Vector3 p1;
			Matrix4 matrix (Matrix4::IDENTITY);
			tireShape->CalculateAABB (&matrix[0][0], &p0.x, &p1.x);

			Real Ixx;
			Real Iyy;
			Real Izz;
			Real mass;
			mainBody->GetMassAndInertia (mass, Ixx, Iyy, Izz);

			const Vector3& gravity = ((OgreNewtonWorld*) mainBody->GetNewton())->GetGravity();

			Real radius = (p1.y - p0.y) * 0.5f;

			// calculate a torque the will produce a 0.5f of the force of gravity
			m_maxEngineTorque = 0.25f * mass * radius * gravity.length();

			// calculate the coefficient of drag for top speed of 20 m/s
			Real maxOmega = 200.0f / radius;
			m_omegaResistance = 1.0f / maxOmega;
		}

		// apply the tractor torque to the tires here
		virtual void OnSubmitConstraint (dFloat timestep, int threadIndex) 
		{
			m_application->m_keyboard->capture();

			Real engineTorque = 0.0f;
			if (m_application->m_keyboard->isKeyDown(OIS::KC_W)) {
				engineTorque = -m_maxEngineTorque; 
			}

			if (m_application->m_keyboard->isKeyDown(OIS::KC_S)) {
				engineTorque = m_maxEngineTorque; 
			}

			Matrix4 matrix;
			m_mainBody->GetMatrix(&matrix[0][0]);
			matrix = matrix.transpose();
			matrix.setTrans(Vector3::ZERO);
			
			Vector3 tirePing (matrix * Vector3(0.0f, 0.0f, 1.0f));
			if (engineTorque != 0.0f) {
				Vector3 torque (tirePing * engineTorque);
				m_tire->AddTorque (&torque.x);
			}

			Vector3 omega;
			m_tire->GetOmega (&omega.x);
			Real omegaMag = omega.dotProduct(tirePing);
			Real sign = (omegaMag >= 0.0f) ? 1.0 : -1.0f;
			omega -= tirePing * (sign * omegaMag * omegaMag * m_omegaResistance);
			m_tire->SetOmega(&omega.x);
		}

		public:
		static void ConnectTire (OgreNewtonDynamicBody* const body, OgreNewtonDynamicBody* const tire, OgreNewtonDemoApplication* const application)  
		{
			Matrix4 tireMatrix;
			Matrix4 matrixOffset;

			tire->GetCollision()->GetMatrix(&matrixOffset[0][0]);
			tireMatrix = (tire->GetMatrix() * matrixOffset.transpose()).transpose();
			new ForkliftFrontTireJoint (&tireMatrix[0][0], tire, body, application);
		}

		dNewtonDynamicBody* const m_tire;
		dNewtonDynamicBody* const m_mainBody;
		OgreNewtonDemoApplication* m_application;

		Real m_omegaResistance;
		Real m_maxEngineTorque;
	};


	class ForkliftRearTireJoint: public dNewtonHingeJoint
	{
		ForkliftRearTireJoint (const dFloat* const pinAndPivotFrame, dNewtonDynamicBody* const tire, dNewtonDynamicBody* const mainBody, OgreNewtonDemoApplication* const application)
			:dNewtonHingeJoint (pinAndPivotFrame, tire, mainBody)
			,m_tire(tire)
			,m_mainBody(mainBody)
			,m_application(application)
		{
		}

		// control the steering angle here
		virtual void OnSubmitConstraint (dFloat timestep, int threadIndex) 
		{

		}

		public:
		static void ConnectTire (OgreNewtonDynamicBody* const body, OgreNewtonDynamicBody* const tire, OgreNewtonDemoApplication* const application)  
		{
			Matrix4 tireMatrix;
			Matrix4 matrixOffset;

			tire->GetCollision()->GetMatrix(&matrixOffset[0][0]);
			tireMatrix = (tire->GetMatrix() * matrixOffset.transpose()).transpose();
			new ForkliftRearTireJoint (&tireMatrix[0][0], tire, body, application);
		}

		dNewtonDynamicBody* const m_tire;
		dNewtonDynamicBody* const m_mainBody;
		OgreNewtonDemoApplication* m_application;
	};



	class LocalTransformCalculator: public OgreNewtonHierarchyTransformManager::OgreNewtonHierarchyTransformController
	{
		public:
		LocalTransformCalculator (OgreNewtonHierarchyTransformManager* const manager)
			:OgreNewtonHierarchyTransformController(manager)
		{
		}

		~LocalTransformCalculator()
		{
		}

		virtual void UpdateTransform (dNewtonBody* const bone, const dFloat* const localMatrix)
		{
			if (bone->GetSleepState()) {
				bone->Update (localMatrix);
			}
			bone->SetTargetMatrix (localMatrix);
		}
	};

	OgreNewtonDemoApplication()
		:DemoApplication()
		,m_player(NULL)
		,m_shootRigidBody(NULL)
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
		dNewtonCollisionCompound compoundShape (m_physicsWorld, convexAproximation, 0);


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
		OgreNewtonRayCast raycaster(m_physicsWorld); 
		raycaster.CastRay (&start.x, &end.x);
		LoadWoodPallete (compoundCount, raycaster.m_contact + Vector3 (-10.0f, 0.25f, -50.0f));

/*
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
*/
	}									  

	OgreNewtonDynamicBody* ForkliftMakeMainBody (SceneNode* const node, const Vector3& origin)
	{
		Entity* const ent = (Entity*) node->getAttachedObject (0);
		Vector3 scale (node->getScale());
		OgreNewtonMesh bodyMesh (m_physicsWorld, ent);
		bodyMesh.ApplyTransform (Vector3::ZERO, scale, Quaternion::IDENTITY);
		dNewtonCollisionConvexHull bodyCollision (m_physicsWorld, bodyMesh, 0);
		Matrix4 bodyMatrix;
		bodyMatrix.makeTransform (node->_getDerivedPosition() + origin, Vector3 (1.0f, 1.0f, 1.0f), node->_getDerivedOrientation());
		return new OgreNewtonDynamicBody (m_physicsWorld, 500.0f, &bodyCollision, node, bodyMatrix);
	}

	OgreNewtonDynamicBody* ForkliftMakeBase (SceneNode* const baseNode, const Vector3& origin)
	{
		Entity* const ent = (Entity*) baseNode->getAttachedObject (0);
		Vector3 scale (baseNode->getScale());
		OgreNewtonMesh bodyMesh (m_physicsWorld, ent);
		bodyMesh.ApplyTransform (Vector3::ZERO, scale, Quaternion::IDENTITY);
		dNewtonCollisionConvexHull collision (m_physicsWorld, bodyMesh, 0);

		Matrix4 matrix;
		matrix.makeTransform(baseNode->_getDerivedPosition() + origin, Vector3 (1.0f, 1.0f, 1.0f), baseNode->_getDerivedOrientation());
		return new OgreNewtonDynamicBody (m_physicsWorld, 200.0f, &collision, baseNode, matrix);
	}



	OgreNewtonDynamicBody* ForkliftMakeTire (SceneNode* const tireNode, const Vector3& origin)
	{
		Entity* const ent = (Entity*) tireNode->getAttachedObject (0);
		Vector3 scale (tireNode->getScale());
		AxisAlignedBox box (ent->getBoundingBox());
		Real height = scale.z * (box.getMaximum().z - box.getMinimum().z);
		Real radius = scale.x * (box.getMaximum().x - box.getMinimum().x) * 0.5f - height * 0.5f;
		dNewtonCollisionChamferedCylinder shape (m_physicsWorld, radius, height, 0);

		Matrix4 aligmentMatrix (Quaternion (Radian (3.141592f * 0.5f), Vector3 (0.0f, 1.0f, 0.0f)));
		aligmentMatrix = aligmentMatrix.transpose();
		shape.SetMatrix (&aligmentMatrix[0][0]);

		Matrix4 matrix;
		matrix.makeTransform(tireNode->_getDerivedPosition() + origin, Vector3 (1.0f, 1.0f, 1.0f), tireNode->_getDerivedOrientation());
		return new OgreNewtonDynamicBody (m_physicsWorld, 80.0f, &shape, tireNode, matrix);
	}




	void LoadForklift(const Vector3& origin)
	{
		// load the mode as Ogre node
		DotSceneLoader loader;
		SceneNode* const forkliftRoot = CreateNode (mSceneMgr, NULL, Vector3::ZERO, Quaternion::IDENTITY);
		loader.parseDotScene ("forklift.scene", "Autodetect", mSceneMgr, forkliftRoot);

		// find all vehicle components
		SceneNode* const bodyNode = (SceneNode*) forkliftRoot->getChild ("body");
		dAssert (bodyNode);
		
		SceneNode* const fl_tireNode = (SceneNode*) bodyNode->getChild ("fl_tire");
		SceneNode* const fr_tireNode = (SceneNode*) bodyNode->getChild ("fr_tire");
		SceneNode* const rl_tireNode = (SceneNode*) bodyNode->getChild ("rl_tire");
		SceneNode* const rr_tireNode = (SceneNode*) bodyNode->getChild ("rr_tire");
		dAssert (fl_tireNode);
		dAssert (fr_tireNode);
		dAssert (rl_tireNode);
		dAssert (rr_tireNode);

		SceneNode* const base1Node = (SceneNode*) bodyNode->getChild ("lift");
		SceneNode* const base2Node = (SceneNode*) base1Node->getChild ("lift2");
		dAssert (base1Node);
		dAssert (base2Node);

		// make a local transform controller to control this body
		LocalTransformCalculator* const transformCalculator = new LocalTransformCalculator(m_localTransformManager);

		//convert the body part to rigid bodies
		Matrix4 bindMatrix (Matrix4::IDENTITY);
		OgreNewtonDynamicBody* const mainBody = ForkliftMakeMainBody (bodyNode, origin);
		
		void* const parentBone = transformCalculator->AddBone (mainBody, &bindMatrix[0][0], NULL);

		// make the tires
		OgreNewtonDynamicBody* const frontLeftTireBody = ForkliftMakeTire (fl_tireNode, origin);
		OgreNewtonDynamicBody* const frontRightTireBody = ForkliftMakeTire (fr_tireNode, origin);
		OgreNewtonDynamicBody* const rearLeftTireBody = ForkliftMakeTire (rl_tireNode, origin);
		OgreNewtonDynamicBody* const rearRightTireBody = ForkliftMakeTire (rr_tireNode, origin);

		// make the lift base
		OgreNewtonDynamicBody* const base1 = ForkliftMakeBase (base1Node, origin);
		OgreNewtonDynamicBody* const base2 = ForkliftMakeBase (base2Node, origin);

		// add the tire as children bodies
		transformCalculator->AddBone (frontLeftTireBody, &bindMatrix[0][0], parentBone);
		transformCalculator->AddBone (frontRightTireBody, &bindMatrix[0][0], parentBone);
		transformCalculator->AddBone (rearLeftTireBody, &bindMatrix[0][0], parentBone);
		transformCalculator->AddBone (rearRightTireBody, &bindMatrix[0][0], parentBone);

		// add the base bones
		void* const base1Bone = transformCalculator->AddBone (base1, &bindMatrix[0][0], parentBone);
		void* const base2Bone = transformCalculator->AddBone (base2, &bindMatrix[0][0], base1Bone);

		// connect the part with joints
		ForkliftFrontTireJoint::ConnectTire (mainBody, frontLeftTireBody, this);
		ForkliftFrontTireJoint::ConnectTire (mainBody, frontRightTireBody, this);
		ForkliftRearTireJoint::ConnectTire (mainBody, rearLeftTireBody, this);
		ForkliftRearTireJoint::ConnectTire (mainBody, rearRightTireBody, this);

		// connect the forklift base

		// save the main body as the player
		m_player = mainBody;
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


		// check if there are some vehicle input, if there is, then wakeup the vehicle
		m_keyboard->capture();
		if (m_keyboard->isKeyDown(OIS::KC_W) || m_keyboard->isKeyDown(OIS::KC_S) || m_keyboard->isKeyDown(OIS::KC_A) || m_keyboard->isKeyDown(OIS::KC_D)) {
			m_player->SetSleepState(false);
		}
	}


	void OnPhysicUpdateEnd(dFloat timestepInSecunds)
	{
		OgreNewtonExampleApplication::OnPhysicUpdateEnd (timestepInSecunds);

		// reposition the camera origin to point to the player
		Matrix4 camMatrix;
		m_cameraTransform.GetTargetMatrix (&camMatrix[0][0]);
		camMatrix = camMatrix.transpose();

		Matrix4 playerMatrix(m_player->GetMatrix());
		//playerMatrix = playerMatrix.transpose();

		Vector3 frontDir (camMatrix[0][2], camMatrix[1][2], camMatrix[2][2]);
		Vector3 camOrigin (playerMatrix.transformAffine(Vector3(0.0f, PLAYER_CAMERA_HIGHT_ABOVE_HEAD, 0.0f)));
		camOrigin += frontDir * PLAYER_CAMERA_DISTANCE;
		camMatrix.setTrans(camOrigin); 

		camMatrix = camMatrix.transpose();
		m_cameraTransform.SetTargetMatrix (&camMatrix[0][0]);
	}


	virtual void destroyScene()
	{
		delete (m_shootRigidBody);
	}



	void createScene()
	{
		// create the physic world first
		DemoApplication::createScene();

		// create a local transform manager for calculate local matrices of child sceneNodes attached to rigid bodies
		m_localTransformManager = new OgreNewtonHierarchyTransformManager (m_physicsWorld);


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

		// load articulated ForkLift
		LoadForklift(raycaster.m_contact + Vector3 (0.0f, 0.5f, -10.0f));

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
	OgreNewtonHierarchyTransformManager* m_localTransformManager;
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

