
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


String MakeName (const char* const name)
{
	static int enumeration = 0;
	char text[256];
	sprintf (text, "%s_%d", name, enumeration);
	enumeration ++;
	return String (text);
}


SceneNode* CreateNode (SceneManager* const sceneMgr, Entity* const entity, const Vector3& position, const Quaternion& orientation)
{
	SceneNode* const node = sceneMgr->getRootSceneNode()->createChildSceneNode();
	node->attachObject(entity);
	node->setScale(Vector3 (1.0f, 1.0f, 1.0f));
	node->setPosition(position);
	node->setOrientation(orientation);
	return node;
}


void CreateComponentsForShutting(int count, OgreNewtonWorld* const physicsWorld, dNewtonCollision** shapes, MeshPtr* const visualMeshes)
{
	// create a texture for using with this material
	TexturePtr texture = Ogre::TextureManager::getSingleton().load("smilli.tga", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	// make a material to use with this mesh
	MaterialPtr renderMaterial = MaterialManager::getSingleton().create("shootingMaterial", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	renderMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(true);
	renderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("smilli.tga");
	renderMaterial->setAmbient(1.0f, 1.0f, 1.0f);

	if (count >= 1) {
		// make sphere;
		shapes[0] = new dNewtonCollisionSphere (physicsWorld, 0.25f, 0);

		OgreNewtonMesh mesh (shapes[0]);
		mesh.Triangulate();
		int materialId = mesh.AddMaterial(renderMaterial);
		mesh.ApplySphericalMapping (materialId);

		ManualObject* const object = mesh.CreateEntity(MakeName ("shootMesh"));
		visualMeshes[0] = object->convertToMesh (MakeName ("shootMesh"));
		delete object;
	}

	if (count >= 2) {
		// make capsule;
		shapes[1] = new dNewtonCollisionCapsule (physicsWorld, 0.25f, 1.0f, 0);

		OgreNewtonMesh mesh (shapes[1]);
		mesh.Triangulate();
		int materialId = mesh.AddMaterial(renderMaterial);
		//mesh.ApplySphericalMapping (materialId);
		mesh.ApplyCylindricalMapping (materialId, materialId);

		ManualObject* const object = mesh.CreateEntity(MakeName ("shootMesh"));
		visualMeshes[1] = object->convertToMesh (MakeName ("shootMesh"));
		delete object;
	}

}

