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


#ifndef _OGRE_NEWTON_SCENE_BODY_H_
#define _OGRE_NEWTON_SCENE_BODY_H_

#include "OgreNewtonStdAfx.h"
#include "OgreNewtonBody.h"

using namespace Ogre;


class OgreNewtonSceneBody: public OgreNewtonBody
{
	public:
	enum FaceWinding
	{
		FW_DEFAULT,
		FW_REVERSE
	};

	class OgreNewtonCollisionTree;
	

	OgreNewtonSceneBody (OgreNewtonWorld* const world);
	~OgreNewtonSceneBody();


	virtual void OnForceAndTorque (dFloat timestep, int threadIndex) {};

	virtual void BeginAddRemoveCollision();
	virtual void* AddCollision(const dNewtonCollision* const collision);
	virtual void* AddCollisionTree (SceneNode* const treeNode, FaceWinding faceWind = FW_DEFAULT);
	
	virtual void RemoveCollision (void* const handle);
	virtual void EndAddRemoveCollision();
	
};


#endif