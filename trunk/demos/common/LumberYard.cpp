
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
#include "LumberYard.h"
#include "OgreDotScene.h"
#include "DemoApplication.h"


void LumberYard (SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& origin, int count_x, int count_z, int high)
{
	DotSceneLoader loader;
	SceneNode* const lumberYardRoot = CreateNode (sceneMgr, NULL, Vector3::ZERO, Quaternion::IDENTITY);
	loader.parseDotScene ("lumberyard.scene", "Autodetect", sceneMgr, lumberYardRoot);

SceneNode* const viper = CreateNode (sceneMgr, NULL, Vector3::ZERO, Quaternion::IDENTITY);
viper->setPosition(origin + Vector3(5, 0, 0)) ;
//loader.parseDotScene ("viper.scene", "Autodetect", sceneMgr, viper);
loader.parseDotScene ("aventador.scene", "Autodetect", sceneMgr, viper);

	Real mass = 5.0f;

	int count = lumberYardRoot->numChildren();
	for (int i = 0; i < count; i ++) {
		SceneNode* const node = (SceneNode*)lumberYardRoot->getChild(i);

		Entity* const ent = (Entity*) node->getAttachedObject (0);
		Vector3 scale (node->getScale());

		OgreNewtonMesh bodyMesh (world, ent);
		bodyMesh.ApplyTransform (Vector3::ZERO, scale, Quaternion::IDENTITY);
		dNewtonCollisionConvexHull bodyCollision (world, bodyMesh, m_all);
		Matrix4 bodyMatrix;
		bodyMatrix.makeTransform (node->_getDerivedPosition() + origin, Vector3 (1.0f, 1.0f, 1.0f), node->_getDerivedOrientation());
		new OgreNewtonDynamicBody (world, mass, &bodyCollision, node, bodyMatrix);
	}
}



