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

OgreNewtonWorld::OgreNewtonWorld (RenderWindow* const win, int updateFramerate)
	:FrameListener()
	,dNewton()
	,m_concurrentUpdateMode(true)
{
	setUpdateFPS (Real (updateFramerate), 3);
}

OgreNewtonWorld::~OgreNewtonWorld()
{
}


void OgreNewtonWorld::setUpdateFPS(Real desiredFps, int maxUpdatesPerFrames)
{
	m_timestep = 1.0f / desiredFps;
	SetMaxUpdatesPerIterations (maxUpdatesPerFrames);
}

void OgreNewtonWorld::SetConcurrentUpdateMode (bool mode)
{
	m_concurrentUpdateMode = mode;
}

bool OgreNewtonWorld::GetConcurrentUpdateMode () const
{
	return m_concurrentUpdateMode;
}


dLong OgreNewtonWorld::GetPhysicsTimeInMicroSeconds() const
{
	return m_lastPhysicTimeInMicroseconds;
}


bool OgreNewtonWorld::frameStarted(const FrameEvent &evt)
{
	dLong currentTime = GetTimeInMicrosenconds ();

	if (m_concurrentUpdateMode) {
		UpdateAsync (m_timestep);
	} else {
		Update (m_timestep);
	}

	// iterate over all physics bodies and get the tranformtaion matrix;
	for (dNewtonBody* body = GetFirstBody(); body; body = GetNextBody(body)) {
		SceneNode* const node = (SceneNode*) body->GetUserData();
		dAssert (node);

	}


	m_lastPhysicTimeInMicroseconds = GetTimeInMicrosenconds () - currentTime;
	return true;
}