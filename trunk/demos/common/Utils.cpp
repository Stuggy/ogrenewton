
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

Real Rand (Real base)
{
	return 2.0f * base * (Real(rand()) / RAND_MAX - 0.5f); 
}

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


