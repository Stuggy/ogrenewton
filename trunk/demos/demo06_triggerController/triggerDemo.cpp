
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
#include "ShootRigidBody.h"
#include "DemoApplication.h"


using namespace Ogre;

#define WORLD_SCALE 0.05f 

class OgreNewtonDemoApplication: public DemoApplication, public RenderTargetListener
{
	public:


	class WaterVolumeController: public dNewtonTriggerManager::dNewtonTrigger
	{
		public:
		WaterVolumeController (OgreNewtonTriggerManager* const manager, const dNewtonCollision& convexShape, SceneNode* const waterNode, const Matrix4& matrix, const Plane& waterPlane)
			:dNewtonTrigger (manager, convexShape, waterNode, &matrix[0][0])
			,m_waterViscosity (0.1f)
			,solidToFluidVolumeRatio(0.9f)
		{
			m_waterPlane = waterPlane.normal;
			m_waterPlane.w = waterPlane.d;
		}

		virtual void OnEnter(dNewtonBody* const guess)
		{
		}

		virtual void OnExit(dNewtonBody* const guess)
		{
		}

		virtual void OnInside(dNewtonBody* const guess)
		{
			Real Ixx;
			Real Iyy;
			Real Izz;
			Real mass;
			guess->GetMassAndInertia (mass, Ixx, Iyy, Izz);

			if (mass > 0.0f) {
				Matrix4 matrix;
				Vector3 cog;
				Vector3 accelPerUnitMass;
				Vector3 torquePerUnitMass;
				const Vector3 gravity = ((OgreNewtonWorld*)guess->GetNewton())->GetGravity();

				guess->GetMatrix(&matrix[0][0]);
				guess->GetCenterOfMass (&cog.x);
				cog = matrix.transpose() * cog;
				dNewtonCollision* const collision = guess->GetCollision();

				Real shapeVolume = collision->GetVolume();
				Real fluidDentity = 1.0f / (shapeVolume * solidToFluidVolumeRatio);
				collision->CalculateBuoyancyAcceleration (&matrix[0][0], &cog[0], &gravity.x, &m_waterPlane.x, fluidDentity, m_waterViscosity, &accelPerUnitMass.x, &torquePerUnitMass.x);

				Vector3 force (accelPerUnitMass * mass);
				Vector3 torque (torquePerUnitMass * mass);

				guess->AddForce(&force.x);
				guess->AddTorque(&torque.x);
			}
		}

		Vector4 m_waterPlane;
		Real m_waterViscosity;
		Real solidToFluidVolumeRatio;
	};



	OgreNewtonDemoApplication()
		:DemoApplication()
	{
	}

	virtual ~OgreNewtonDemoApplication()
	{
	}

	protected:

	void setupWater()
	{
		// create our reflection & refraction render textures, and setup their render targets
		for (unsigned int i = 0; i < 2; i++)
		{
			TexturePtr tex = TextureManager::getSingleton().createManual(i == 0 ? "refraction" : "reflection", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 512, 512, 0, PF_R8G8B8, TU_RENDERTARGET);

			RenderTarget* rtt = tex->getBuffer()->getRenderTarget();
			rtt->addViewport(mCamera)->setOverlaysEnabled(false);
			rtt->addListener(this);

			if (i == 0) mRefractionTarget = rtt;
			else mReflectionTarget = rtt;
		}

		// create our water plane mesh
		mWaterPlane = Plane(Vector3::UNIT_Y, 0);
		MeshManager::getSingleton().createPlane("water", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,	mWaterPlane, 700 * WORLD_SCALE, 1300 * WORLD_SCALE, 10, 10, true, 1, 3, 5, Vector3::UNIT_Z);

		// create a water entity using our mesh, give it the shader material, and attach it to the origin
		SceneNode* const waterNode = mSceneMgr->getRootSceneNode()->createChildSceneNode ("waterNode");

		mWater = mSceneMgr->createEntity("Water", "water");
       // some how I cannot get this material to work
		//mWater->setMaterialName("Examples/FresnelReflectionRefraction");
		mWater->setMaterialName("Simple/Translucent");
		waterNode->attachObject(mWater);


		// Get the AABB of the water plane and use that for placing a trigger volume around it
		// some how the AABB on the node is no ready, using the entity AABB 
		//AxisAlignedBox bBox (waterNode->_getWorldAABB());
		AxisAlignedBox bBox (mWater->getBoundingBox());
		Real box_x =  bBox.getMaximum().x - bBox.getMinimum().x;
		Real box_z =  bBox.getMaximum().z - bBox.getMinimum().z;
		Real box_y =  5.0f;

		Matrix4 waterMatrix (Matrix4::IDENTITY);
		waterMatrix.setTrans(Vector3 (0, (bBox.getMaximum().y + bBox.getMinimum().y) * 0.5f - box_y * 0.5f, 0.0f));
		waterMatrix = waterMatrix.transpose();

		dNewtonCollisionBox waterVolume (m_physicsWorld, box_x, box_y, box_z, 0);
		new WaterVolumeController (m_physicsWorld->GetTriggerManager(), waterVolume, waterNode, waterMatrix, mWaterPlane);
	}



	void loadStaticScene ()
	{
		// create a scene body to add all static collidable meshes in the world 
		OgreNewtonSceneBody* const sceneBody = new OgreNewtonSceneBody (m_physicsWorld, m_allExcludingMousePick);

		// start adding collision shape to the scene body
		sceneBody->BeginAddRemoveCollision();

		Entity* ent;
		SceneNode* const headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		headNode->setScale (WORLD_SCALE, WORLD_SCALE, WORLD_SCALE);
		headNode->setPosition(-350.0f * WORLD_SCALE, 55.0f * WORLD_SCALE, 130.0f * WORLD_SCALE);
		headNode->yaw(Degree(90));
		ent = mSceneMgr->createEntity("Head", "ogrehead.mesh");
		ent->setMaterialName("RomanBath/OgreStone");
		//mSurfaceEnts.push_back(ent);
		headNode->attachObject(ent);
		sceneBody->AddCollisionTree (headNode);

		SceneNode* const sceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode ("SceneNode");
		sceneNode->setScale (WORLD_SCALE, WORLD_SCALE, WORLD_SCALE);

		ent = mSceneMgr->createEntity("UpperBath", "RomanBathUpper.mesh" );
		sceneNode->attachObject(ent);        
		//mSurfaceEnts.push_back(ent);

		ent = mSceneMgr->createEntity("Columns", "columns.mesh");
		sceneNode->attachObject(ent);        
		//mSurfaceEnts.push_back(ent);

		ent = mSceneMgr->createEntity("LowerBath", "RomanBathLower.mesh");
		sceneNode->attachObject(ent);

		sceneBody->AddCollisionTree (sceneNode);

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
		dNewtonCollisionCompound compoundShape (m_physicsWorld, m_all);
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
		dNewtonCollisionCompound compoundShape (m_physicsWorld, convexAproximation, m_all);

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
		const int compoundCount = 8;

		// add some compound collision shapes
		SpawnManualCompoundCollisionShapes (compoundCount, origin + Vector3 (-14.0f, 1.0f, 10.0f));

		// some automatically generated convex decompositions
		SpawnAutomaticCompoundCollisionShapes (compoundCount, origin + Vector3 (-14.0f, 1.0f, 0.0f), "cow.mesh");
		SpawnAutomaticCompoundCollisionShapes (compoundCount, origin + Vector3 (-14.0f, 1.0f, -10.0f), "bessel.mesh");
		SpawnAutomaticCompoundCollisionShapes (compoundCount, origin + Vector3 (-14.0f, 1.0f, -20.0f), "torus.mesh");
		SpawnAutomaticCompoundCollisionShapes (compoundCount, origin + Vector3 (-14.0f, 1.0f, -30.0f), "torusKnot.mesh");

		// make a convex hull from 200 random points
		dNewtonScopeBuffer<Vector3> points(200);
		for (int i = 0; i < points.GetElementsCount(); i ++) {
			points[i] = Vector3 (Rand (0.5f), Rand (0.5f), Rand (0.5f));
		}

		// add samples the single solid primitive with non uniform scaling
		const int spawnCount = 20;
//		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 (-8.0f, 4.0f, 20.0f), dNewtonCollisionSphere (m_physicsWorld, 0.5f, m_all));
		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 (-6.0f, 4.0f, 20.0f), dNewtonCollisionBox (m_physicsWorld, 0.5f, 0.5f, 0.5f, m_all));
//		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 (-4.0f, 4.0f, 20.0f), dNewtonCollisionCapsule (m_physicsWorld, 0.25f, 0.5f, m_all));
//		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 (-2.0f, 4.0f, 20.0f), dNewtonCollisionTaperedCapsule (m_physicsWorld, 0.25f, 0.35f, 0.75f, m_all));
		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 ( 0.0f, 4.0f, 20.0f), dNewtonCollisionCone (m_physicsWorld, 0.25f, 0.75f, m_all));
		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 ( 2.0f, 4.0f, 20.0f), dNewtonCollisionCylinder (m_physicsWorld, 0.25f, 0.75f, 0));
		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 ( 4.0f, 4.0f, 20.0f), dNewtonCollisionTaperedCylinder (m_physicsWorld, 0.25f, 0.35f, 0.75f, m_all));
		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 ( 6.0f, 4.0f, 20.0f), dNewtonCollisionChamferedCylinder (m_physicsWorld, 0.25f, 0.75f, m_all));
		SpawnRegularScaledCollisionShape (spawnCount, origin + Vector3 ( 8.0f, 4.0f, 20.0f), dNewtonCollisionConvexHull (m_physicsWorld, points.GetElementsCount(), &points[0].x, sizeof (Vector3), 0.0f, m_all));
	}									  

	void createFrameListener()
	{
		// this is our custom frame listener for this app, that lets us shoot cylinders with the space bar, move the camera, etc.
		//		m_listener = new ApplicationFrameListener (mRoot, mWindow, mCamera, mSceneMgr, this, m_debugRender);
		//		mRoot->addFrameListener(m_listener);
	}

/*
	void preRenderTargetUpdate(const RenderTargetEvent& evt)
	{
		mWater->setVisible(false);  // hide the water

		if (evt.source == mReflectionTarget)  // for reflection, turn on camera reflection and hide submerged entities
		{
			mCamera->enableReflection(mWaterPlane);
			for (std::vector<Entity*>::iterator i = mSubmergedEnts.begin(); i != mSubmergedEnts.end(); i++)
				(*i)->setVisible(false);
		}
		else  // for refraction, hide surface entities
		{
			for (std::vector<Entity*>::iterator i = mSurfaceEnts.begin(); i != mSurfaceEnts.end(); i++)
				(*i)->setVisible(false);
		}
	}

	void postRenderTargetUpdate(const RenderTargetEvent& evt)
	{
		mWater->setVisible(true);  // unhide the water

		if (evt.source == mReflectionTarget)  // for reflection, turn off camera reflection and unhide submerged entities
		{
			mCamera->disableReflection();
			for (std::vector<Entity*>::iterator i = mSubmergedEnts.begin(); i != mSubmergedEnts.end(); i++)
				(*i)->setVisible(true);
		}
		else  // for refraction, unhide surface entities
		{
			for (std::vector<Entity*>::iterator i = mSurfaceEnts.begin(); i != mSurfaceEnts.end(); i++)
				(*i)->setVisible(true);
		}
	}
*/

	void OnPhysicUpdateBegin(dFloat timestepInSecunds)
	{
		DemoApplication::OnPhysicUpdateBegin(timestepInSecunds);
	}

	virtual void destroyScene()
	{
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

		// load a water plane and place an water trigger volume around it
		setupWater();

		// position camera using the ray cast functionality
		Vector3 start(0.0f, 1000.0f, 42.0f);
		Vector3 end(0.0f, -1000.0f, 42.0f);
		OgreNewtonRayCast raycaster(m_physicsWorld, m_rayCast); 
		raycaster.CastRay (&start.x, &end.x);

		Vector3 origin (raycaster.m_contact + Vector3 (0.0f, 3.0f, 0.0f));
		mCamera->setPosition(origin);

		// set the near and far clip plane
		mCamera->setNearClipDistance(0.1f);
		mCamera->setFarClipDistance(10000.0f);
		//mCamera->setPolygonMode(Ogre::PM_WIREFRAME);

		// now load the dynamics Scene
		LoadDynamicScene(Vector3 (0.0f, 4.0f, 0.0f));

		// initialize the Camera position after the scene was loaded
		ResetCamera (mCamera->getPosition(), mCamera->getOrientation());
	}

	Entity* mWater;
	Plane mWaterPlane;
	RenderTarget* mRefractionTarget;
	RenderTarget* mReflectionTarget;
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



 
 

