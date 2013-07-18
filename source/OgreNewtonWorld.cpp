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
#include "OgreNewtonDynamicBody.h"
#include "OgreNewtonApplication.h"



OgreNewtonWorld::OgreNewtonWorld (OgreNewtonApplication* const application, int updateFramerate)
	:FrameListener()
	,dNewton()
	,m_application(application)
	,m_gravity (0.0f, -9.8f, 0.0f)
	,m_concurrentUpdateMode(true)
	,m_lastPhysicTimeInMicroseconds(GetTimeInMicrosenconds ())
	,m_physicUpdateTimestepInMocroseconds(0)
{
	SetUpdateFPS (Real (updateFramerate), 3);
}

OgreNewtonWorld::~OgreNewtonWorld()
{
}


void OgreNewtonWorld::SetUpdateFPS(Real desiredFps, int maxUpdatesPerFrames)
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
	return m_physicUpdateTimestepInMocroseconds;
}

void OgreNewtonWorld::OnBeginUpdate (dFloat timestepInSecunds)
{
	m_application->OnPhysicUpdateBegin(timestepInSecunds);
}

void OgreNewtonWorld::OnEndUpdate (dFloat timestepInSecunds)
{
	m_application->OnPhysicUpdateEnd(timestepInSecunds);
}



bool OgreNewtonWorld::frameStarted(const FrameEvent &evt)
{
//NewtonSerializeToFile (GetNewton(), "xxxx.bin");

	dLong simulationTime = GetTimeInMicrosenconds ();
	if (m_concurrentUpdateMode) {
		UpdateAsync (m_timestep);
	} else {
		Update (m_timestep);
	}

	dFloat param = GetInteplationParam(m_timestep);
	bool ret = m_application->OnRenderUpdateBegin (param);

	Real applicationTime = Real (simulationTime - m_lastPhysicTimeInMicroseconds) * 1.0e-6f;

	// iterate over all physics bodies and get the tranformtaion matrix;
	for (dNewtonBody* body = GetFirstBody(); body; body = GetNextBody(body)) {
		SceneNode* const node = (SceneNode*) body->GetUserData();
		if (node && !body->GetSleepState()) {
			dMatrix matrix;
			body->GetVisualMatrix (param, &matrix[0][0]);
			dQuaternion rotation (matrix);
			Vector3 posit (matrix.m_posit.m_x, matrix.m_posit.m_y, matrix.m_posit.m_z);
			Quaternion nodeRotation (rotation.m_q0, rotation.m_q1, rotation.m_q2, rotation.m_q3);

			Node* const nodeParent = node->getParent();
			const Vector3& derivedScale = nodeParent->_getDerivedScale();
			const Vector3& derivedPosition = nodeParent->_getDerivedPosition();
			const Quaternion derivedRotationInv (nodeParent->_getDerivedOrientation().Inverse());

			node->setPosition (derivedRotationInv * (posit - derivedPosition) / derivedScale);
			node->setOrientation (derivedRotationInv * nodeRotation);

			// update the application user data (that need to be update at rendering time, ex animations, particles emmitions, etc)
			//body->OnApplicationPostTransform (evt.timeSinceLastFrame);
			body->OnApplicationPostTransform (applicationTime);
		}
	}
	m_application->OnRenderUpdateEnd (param);

	m_lastPhysicTimeInMicroseconds = simulationTime;
	m_physicUpdateTimestepInMocroseconds = GetTimeInMicrosenconds () - simulationTime;
	return ret;
}