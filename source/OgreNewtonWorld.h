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


#ifndef _OGRE_NEWTON_WORLD_H_
#define _OGRE_NEWTON_WORLD_H_

#include "OgreNewtonStdAfx.h"

using namespace Ogre;

class OgreNewtonWorld: public FrameListener, public dNewton 
{
	public:
	OgreNewtonWorld (int updateFramerate = 120);
	virtual ~OgreNewtonWorld();

	bool frameStarted(const FrameEvent &evt);

	void SetUpdateFPS(Real desiredFps, int maxUpdatesPerFrames = 3);
	void SetConcurrentUpdateMode (bool mode);
	bool GetConcurrentUpdateMode () const; 
	dNewtonBody* CreateBox(Ogre::SceneNode* const sourceNode);
	dLong GetPhysicsTimeInMicroSeconds() const;
	const Vector3& GetGravity() const {return m_gravity;}

	protected:
	Vector3 m_gravity;
	Real m_timestep;
	bool m_concurrentUpdateMode;
	dLong m_lastPhysicTimeInMicroseconds;
};

#endif
