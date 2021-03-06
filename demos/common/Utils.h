
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

#ifndef _UTILS_H_
#define _UTILS_H_


using namespace Ogre;


class KeyTrigger
{
	public:
	KeyTrigger (bool state = false)
		:m_state(state)
		,m_mem0(false)
		,m_mem1(false)
	{
	}

	bool TriggerUp() const
	{
		return !m_mem0 & m_mem1;
	}

	bool TriggerDown() const
	{
		return m_mem0 & !m_mem1;
	}

	void Update (bool input)
	{
		m_mem0 = m_mem1;
		m_mem1 = input;
		m_state = (!m_mem0 & m_mem1) ^ m_state;
	}

	bool m_state;
	private:
	bool m_mem0;
	bool m_mem1;
};


enum OgreDemoCollisionTypes
{
	m_rayCast = 1<<0,
	m_mousePick = 1<<1,
	m_gravidyBody = 1<<2,

	m_all = m_rayCast | m_gravidyBody | m_mousePick,
	m_allExcludingMousePick = m_all & ~ m_mousePick,
};


Real Rand (Real base);

String MakeName (const char* const name);
SceneNode* CreateNode (SceneManager* const sceneMgr, Entity* const entity, const Vector3& position, const Quaternion& orientation);


#endif