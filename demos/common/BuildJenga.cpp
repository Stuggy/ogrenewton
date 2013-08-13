
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

#include "Utils.h"
#include "BuildJenga.h"
#include "DemoApplication.h"


void BuildJenga(SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& location, int high)
{
	Vector3 blockBoxSize (0.4f, 0.2f, 0.4f * 3.0f);

	// find the floor position
	Vector3 start(location + Vector3 (0.0f, 10.0f, 0.0f));
	Vector3 end (start - Vector3 (0.0f, 20.0f, 0.0f));
	OgreNewtonRayCast raycaster(world, m_rayCast); 
	raycaster.CastRay (&start.x, &end.x);
	Vector3 position (raycaster.m_contact + Vector3 (0.0f, blockBoxSize.y * 0.5f, 0.0f));

	Matrix4 baseMatrix (Matrix4::IDENTITY);
	baseMatrix.setTrans (position);

	// set realistic mass and inertia matrix for each block
	Real mass = 5.0f;

	// create a 90 degree rotation matrix
	Matrix4 rotMatrix (Quaternion (Degree(90.0f), Vector3 (0.0f, 1.0f, 0.0f)));

	Real collisionPenetration = 1.0f / 256.0f;

	// make a box collision shape
	dNewtonCollisionBox boxShape (world, blockBoxSize.x, blockBoxSize.y, blockBoxSize.z, m_all);

	// create a texture for using with this material
	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().load("crate.tga", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	// make a material to use with this mesh
	MaterialPtr renderMaterial = MaterialManager::getSingleton().create("jengaMaterial", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	renderMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(true);
	renderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("crate.tga");
	renderMaterial->setAmbient(0.2f, 0.2f, 0.2f);

	// create a visual for visual representation
	OgreNewtonMesh boxMesh (&boxShape);
	boxMesh.Triangulate();
	int materialId = boxMesh.AddMaterial(renderMaterial);
	boxMesh.ApplyBoxMapping (materialId, materialId, materialId);

	// create a manual object for rendering 
	ManualObject* const object = boxMesh.CreateEntity(MakeName ("jengaBox"));
	MeshPtr mesh (object->convertToMesh (MakeName ("jengaBox")));

	for (int i = 0; i < high; i ++) { 
		Matrix4 matrix(baseMatrix);
		Vector3 step_x (matrix[0][0], matrix[0][1], matrix[0][2]); 

		step_x = step_x * blockBoxSize.x;
		matrix.setTrans (matrix.getTrans() - step_x);

		for (int j = 0; j < 3; j ++) { 
			Entity* const ent = sceneMgr->createEntity(MakeName ("jengaBox"), mesh);
			SceneNode* const node = CreateNode (sceneMgr, ent, matrix.getTrans(), matrix.extractQuaternion());
			new OgreNewtonDynamicBody (world, mass, &boxShape, node, matrix);
			matrix.setTrans (matrix.getTrans() + step_x);
		}

		baseMatrix = baseMatrix * rotMatrix;			
		Vector3 step_y (matrix[1][0], matrix[1][1], matrix[1][2]); 
		step_y = step_y * (blockBoxSize.y - collisionPenetration);
		baseMatrix.setTrans (baseMatrix.getTrans() + step_y);
	}
	delete object;
}	



