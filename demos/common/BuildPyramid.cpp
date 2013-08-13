
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
#include "BuildPyramid.h"
#include "DemoApplication.h"



void BuildPyramid (SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& location, Real mass, int base, int high)
{
	// please do not ask why, I just like golden ratios dimensions, 
	Vector3 blockBoxSize (1.62f/2.0f, 0.25f, 0.5f);

	// find the floor position
	Vector3 start(location + Vector3 (0.0f, 10.0f, 0.0f));
	Vector3 end (start - Vector3 (0.0f, 20.0f, 0.0f));
	OgreNewtonRayCast raycaster(world, m_rayCast); 
	raycaster.CastRay (&start.x, &end.x);
	Vector3 position (raycaster.m_contact + Vector3 (0.0f, blockBoxSize.y * 0.5f, 0.0f));

	// build the visual mesh out of a collision box
	// make a box collision shape
	dNewtonCollisionBox boxShape (world, blockBoxSize.x, blockBoxSize.y, blockBoxSize.z, m_all);

	// create a texture for using with this material
	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().load("crate.tga", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	// make a material to use with this mesh
	MaterialPtr renderMaterial = MaterialManager::getSingleton().create("pyramidMaterial", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
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


	// get the initial matrix
	Matrix4 matrix (Matrix4::IDENTITY);
	matrix.setTrans (position);

	float y0 = matrix.getTrans().x + blockBoxSize.y / 2.0f;
	float x0 = matrix.getTrans().x - (blockBoxSize.x + 0.01f) * base / 2;

	Real collisionPenetration = 1.0f / 256.0f;

	matrix.setTrans(matrix.getTrans() + Vector3 (0.0f, y0, 0.0f));
	for (int j = 0; j < high; j ++) {
		matrix[0][3] = x0;
		for (int i = 0; i < (base - j) ; i ++) {
			Entity* const ent = sceneMgr->createEntity(MakeName ("pyramidBox"), mesh);
			SceneNode* const node = CreateNode (sceneMgr, ent, matrix.getTrans(), matrix.extractQuaternion());
			new OgreNewtonDynamicBody (world, mass, &boxShape, node, matrix);
			matrix[0][3] += blockBoxSize.x;
		}
		x0 += (blockBoxSize.x + 0.01f) * 0.5f;
		matrix[1][3] += (blockBoxSize.y - collisionPenetration);
	}

	delete object;
}	



