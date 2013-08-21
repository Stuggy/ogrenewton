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


#include "OgreNewtonStdAfx.h"
#include "OgreNewtonWorld.h"
#include "OgreNewtonRayCast.h"
#include "OgreNewtonDynamicBody.h"
#include "OgreNewtonInputManager.h"


OgreNewtonInputManager::OgreNewtonInputManager (OgreNewtonWorld* const world)
	:dNewtonInputManager(world)
	,m_world(world)
{
}

OgreNewtonInputManager::~OgreNewtonInputManager()
{
}


void OgreNewtonInputManager::OnBeginUpdate (dFloat timestepInSecunds)
{
	m_world->OnBeginUpdate(timestepInSecunds);
}

void OgreNewtonInputManager::OnEndUpdate (dFloat timestepInSecunds)
{
	m_world->OnEndUpdate(timestepInSecunds);
}

