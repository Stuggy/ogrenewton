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

#include "MouseCursor.h"

#include <OgreNewtonMesh.h>
#include <OgreNewtonWorld.h>
#include <OgreNewtonRayCast.h>
#include <OgreNewtonDebugger.h>
#include <OgreNewtonSceneBody.h>
#include <OgreNewtonDynamicBody.h>
#include <OgreNewtonRayPickManager.h>
#include <OgreNewtonExampleApplication.h>

#include <OgreTextureManager.h>
#include <OgreTechnique.h>

#include "Utils.h"
#include "DemoApplication.h"
#include "ShootRigidBody.h"


ShootRigidBody::ShootRigidBody (OgreNewtonWorld* const physicsWorld)
	:m_shootingTimer(0.25f)
{
	// create a texture for using with this material
	TexturePtr texture = Ogre::TextureManager::getSingleton().load("smilli.tga", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	// make a material to use with this mesh
	MaterialPtr renderMaterial = MaterialManager::getSingleton().create("shootingMaterial", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	renderMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(true);
	renderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("smilli.tga");
	renderMaterial->setAmbient(1.0f, 1.0f, 1.0f);

	{
		// make sphere;
		m_shootingCollisions[0] = new dNewtonCollisionSphere (physicsWorld, 0.25f, m_all);

		OgreNewtonMesh mesh (m_shootingCollisions[0]);
		mesh.Triangulate();
		int materialId = mesh.AddMaterial(renderMaterial);
		mesh.ApplySphericalMapping (materialId);

		ManualObject* const object = mesh.CreateEntity(MakeName ("shootMesh"));
		m_shootingMesh[0] = object->convertToMesh (MakeName ("shootMesh"));
		delete object;
	}

	{
		// make capsule;
		m_shootingCollisions[1] = new dNewtonCollisionCapsule (physicsWorld, 0.25f, 1.0f, m_all);

		OgreNewtonMesh mesh (m_shootingCollisions[1]);
		mesh.Triangulate();
		int materialId = mesh.AddMaterial(renderMaterial);
		//mesh.ApplySphericalMapping (materialId);
		mesh.ApplyCylindricalMapping (materialId, materialId);

		ManualObject* const object = mesh.CreateEntity(MakeName ("shootMesh"));
		m_shootingMesh[1] = object->convertToMesh (MakeName ("shootMesh"));
		delete object;
	}
}


ShootRigidBody::~ShootRigidBody()
{
	for (int i = 0; i < int (sizeof (m_shootingCollisions) / sizeof (m_shootingCollisions[0])); i ++) {
		delete m_shootingCollisions[i];
	}
}

void ShootRigidBody::ShootRandomBody(DemoApplication* const application, SceneManager* const sceneMgr, Real timestep)
{
	m_shootingTimer -= timestep;
	if ((m_shootingTimer < 0.0f) && application->m_keyboard->isKeyDown(OIS::KC_SPACE)) {
		m_shootingTimer = 0.1f;

		Vector3 cameraPosit;
		Quaternion cameraRotation;

		OgreNewtonWorld* const physics = application->GetPhysics();
		application->GetInterpolatedCameraMatrix (cameraPosit, cameraRotation);

		int index = (rand() >> 3) % int (sizeof (m_shootingMesh) / sizeof (m_shootingMesh[0]));

		Entity* const ent = sceneMgr->createEntity(MakeName ("shootObject"), m_shootingMesh[index]);
		SceneNode* const node = CreateNode (sceneMgr, ent, cameraPosit, cameraRotation);
		Matrix4 matrix;
		matrix.makeTransform (cameraPosit, Vector3(1.0f, 1.0f, 1.0f), cameraRotation);
		OgreNewtonDynamicBody* const body = new OgreNewtonDynamicBody (physics, 30.0f, m_shootingCollisions[index], node, matrix);

		const Real speed = -40.0f;
		Vector3 veloc (Vector3 (matrix[0][2], matrix[1][2], matrix[2][2]) * speed);   
		body->SetVeloc(veloc);
	}
}
