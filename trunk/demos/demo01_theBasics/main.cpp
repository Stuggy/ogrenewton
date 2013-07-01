
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
#include <OgreNewtonSceneBody.h>


using namespace Ogre;

#include <OgreOverlay.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <ExampleApplication.h>
#include <ExampleFrameListener.h>


class OgreNewtonApplication: public ExampleApplication
{
	public:

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

		SceneNode* const node = mSceneMgr->getRootSceneNode()->createChildSceneNode();

		node->attachObject(entity);
		node->setScale(scale);
		node->setPosition(position);
		node->setOrientation(orientation);
		return node;
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

	void CreateBox(SceneNode* const sourceNode, Real mass, const Matrix4& matrix)
	{

		MovableObject* const attachedObject = sourceNode->getAttachedObject(0);

		Vector3 position       = sourceNode->getPosition();
		Vector3 size           = Vector3::UNIT_SCALE;
		Vector3 centerOffset   = Vector3::ZERO;
		Vector3 scale          = sourceNode->_getDerivedScale();
		Vector3 dimensions     = scale;
		Quaternion orientation = sourceNode->_getDerivedOrientation();

		AxisAlignedBox boundingBox = attachedObject->getBoundingBox();
		Vector3 padding = (boundingBox.getMaximum() - boundingBox.getMinimum()) * MeshManager::getSingleton().getBoundsPaddingFactor() * 2;

		size = (boundingBox.getMaximum() - boundingBox.getMinimum()) - padding * 2;
		centerOffset = boundingBox.getMinimum() + padding + (size / 2.0f);
//		dimensions = sourceNode->_getDerivedScale() * size;

		Matrix4 modifierMatrix;
		modifierMatrix.makeTransform(centerOffset * scale, Vector3(1.0f, 1.0f, 1.0f), Quaternion::IDENTITY);
		modifierMatrix = modifierMatrix.transpose();

		dNewtonCollisionBox boxShape (m_physicsWorld, size.x, size.y, size.z, 0);
		boxShape.SetScale(scale.x, scale.y, scale.z);

//		modifiedCollision->setScalarMatrix(modifierMatrix);
		boxShape.SetMatrix (modifierMatrix[0]);

		//OgreNewtonWorld* const dWorld, const dNewtonCollision* const collision, SceneNode* const node, const Matrix4& location
		OgreNewtonBody* const body = new OgreNewtonBody (m_physicsWorld, &boxShape, sourceNode, matrix);
/*
		if (materialId != NULL)
		{
			body->setMaterialGroupID(materialId);
		}

		Vector3 inertia, offset;
		modifiedCollision->calculateInertialMatrix(inertia, offset);

		body->setMassMatrix(mass, inertia * mass);
		body->setCenterOfMass(offset);
		body->setStandardForceCallback();
		body->attachNode(sourceNode);
		body->setPositionOrientation(position, orientation);

		boxes.insert(std::make_pair(sourceNode->getName(), body));

		return body;
*/
	}

	void LoadDynamicScene(const Vector3& origin)
	{
		Vector3 posit (origin + Vector3(0.0f, 0.0f, -5.0f)); 

		Vector3 pos(0.5f, 0.25f, 0.25f);
		Quaternion rot (Degree(0), Vector3::UNIT_Y);

		Matrix4	matrix;	
		matrix.makeTransform (pos, Vector3 (1.0f, 1.0f, 1.0f), rot);

		SceneNode* const node = CreateNode("box.mesh", "box", posit, Vector3(0.5f, 0.25f, 0.25f), Quaternion (Degree(0), Vector3::UNIT_Y));
		CreateBox (node, 10.0f, matrix);

		//SceneNode* const node1 = CreateNode("box.mesh", "box1", posit + Vector3(1.0f, 1.0f, 1.0f), Vector3(1.0f, 1.0f, 1.0f), Quaternion (Degree(0), Vector3::UNIT_Y));
	}

	void createScene()
	{
		// create the physic world first
		m_physicsWorld = new OgreNewtonWorld (mWindow);
		mRoot->addFrameListener(m_physicsWorld);


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

		Vector3 origin (raycaster.m_contact + Vector3 (0.0f, 5.0f, 0.0f));
		mCamera->setPosition(origin);

		// set the near and far clip plane
		mCamera->setNearClipDistance(0.1f);
		mCamera->setFarClipDistance(10000.0f);

		// now load the dynamics Scene
		LoadDynamicScene(origin);
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
	catch(Exception &e)
	{
		MessageBox(NULL, e.getFullDescription().c_str(), "Well, this is embarrassing.. an Ogre exception has occurred.", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}

	return 0;
}

