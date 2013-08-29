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


#ifndef _OGRE_NEWTON_RAY_PEEKING_H_
#define _OGRE_NEWTON_RAY_PEEKING_H_

#include "OgreNewtonStdAfx.h"

using namespace Ogre;

class OgreNewtonWorld;

class OGRE_NEWTON_API OgreNewtonInputManager: public dNewtonInputManager
{
	public:
	class OgrePlayerUserDataPair
	{
		public:
		OgrePlayerUserDataPair()
			:m_userData(NULL)
			,m_player(NULL)
		{
		}
		
		void* m_userData; 
		dNewtonDynamicBody* m_player;
	};

	OgreNewtonInputManager (OgreNewtonWorld* const world);
	virtual ~OgreNewtonInputManager();

	void SetPlayer(dNewtonDynamicBody* const player, void* const userData)
	{
		m_playerInfo.m_player = player;
		m_playerInfo.m_userData = userData;
	}

	const OgrePlayerUserDataPair& GetPlayer() const
	{
		return m_playerInfo;
	}

	virtual void OnBeginUpdate (dFloat timestepInSecunds);
	virtual void OnEndUpdate (dFloat timestepInSecunds);

	OgreNewtonWorld* m_world;
	OgrePlayerUserDataPair m_playerInfo;
};

#endif
