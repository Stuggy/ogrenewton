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


#ifndef _OGRE_NEWTON_PLAYER_MANAGER_H_
#define _OGRE_NEWTON_PLAYER_MANAGER_H_

#include "OgreNewtonStdAfx.h"

using namespace Ogre;


class OgreNewtonWorld;
class OgreNewtonPlayerController;


class OGRE_NEWTON_API OgreNewtonPlayerManager: public CustomPlayerControllerManager
{
	public:
	OgreNewtonPlayerManager (OgreNewtonWorld* const world);
	~OgreNewtonPlayerManager ();

	virtual OgreNewtonPlayerController* CreatePlayer (Real mass, Real outerRadius, Real innerRadius, Real height, Real stairStep);
	virtual void DestroyPlayer (OgreNewtonPlayerController* const player);

	virtual void ApplyPlayerMove (CustomPlayerController* const controller, Real timestep);
};


class OGRE_NEWTON_API OgreNewtonPlayerController: public dNewtonAlloc
{
	public:
	~OgreNewtonPlayerController();

	private:
	OgreNewtonPlayerController (CustomPlayerControllerManager::CustomController* const controller);
	CustomPlayerControllerManager::CustomController* m_controller;

	friend class OgreNewtonPlayerManager;
};



#endif
