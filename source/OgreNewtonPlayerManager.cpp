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
#include "OgreNewtonPlayerManager.h"

Vector3 OgreNewtonPlayerManager::OgreNetwonPlayer::m_upDir (0.0f, 1.0f, 0.0f);
Vector3 OgreNewtonPlayerManager::OgreNetwonPlayer::m_frontDir (0.0f, 0.0f, -1.0f);


OgreNewtonPlayerManager::OgreNewtonPlayerManager (OgreNewtonWorld* const world)
	:dNewtonPlayerManager (world)
{
}

OgreNewtonPlayerManager::~OgreNewtonPlayerManager ()
{
}


//void OgreNewtonPlayerManager::DestroyPlayer (OgreNewtonPlayerController* const player)
//{
//}



OgreNewtonPlayerManager::OgreNetwonPlayer::OgreNetwonPlayer (dNewtonPlayerManager* const manager, SceneNode* const node, dFloat mass, dFloat outerRadius, dFloat innerRadius, dFloat height, dFloat stairStep)
	:dNewtonPlayer (manager, node, mass, outerRadius, innerRadius, height, stairStep, &m_upDir.x, &m_frontDir.x)
{
}

void OgreNewtonPlayerManager::OgreNetwonPlayer::OnPlayerMove (Real timestep)
{

}
