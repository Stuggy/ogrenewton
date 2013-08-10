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
#include "OgreNewtonInputManager.h"
#include "OgreNewtonPlayerManager.h"

#include "OgreNewtonRayPickManager.h"
#include "OgreNewtonTriggerManager.h"
#include "OgreNewtonArticulatedTransformManager.h"


OgreNewtonWorld::OgreNewtonWorld (SceneManager* const manager, int updateFramerate)
	:dNewton()
	,m_sceneManager(manager)
	,m_gravity (0.0f, -9.8f, 0.0f)
	,m_concurrentUpdateMode(true)
	,m_lastPhysicTimeInMicroseconds(GetTimeInMicrosenconds ())
	,m_physicUpdateTimestepInMocroseconds(0)
{
	SetUpdateFPS (Real (updateFramerate), 3);

	// add some of the essential managers, in order of execution
	m_triggerManager = new OgreNewtonTriggerManager(this);
	m_localTransformManager = new OgreNewtonArticulatedTransformManager (this);
	m_playerManager = new OgreNewtonPlayerManager (this);
	m_rayPickerManager = new OgreNewtonRayPickManager (this, 0);
	m_inputManager = new OgreNewtonInputManager(this);
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

void OgreNewtonWorld::Update ()
{
//NewtonSerializeToFile (GetNewton(), "xxxx.bin");
	dLong lastTime = m_lastPhysicTimeInMicroseconds;
	m_lastPhysicTimeInMicroseconds = GetTimeInMicrosenconds ();
	Real applicationTime = Real (m_lastPhysicTimeInMicroseconds - lastTime) * 1.0e-6f;
	if (m_concurrentUpdateMode) {
		dNewton::UpdateAsync (m_timestep);
	} else {
		dNewton::Update (m_timestep);
	}

	unsigned* const lockHandle = m_inputManager->GetLockHandle();
	dFloat param = GetInteplationParam(m_timestep);
	dAssert (applicationTime > 0.0f);
	{
		dNewton::ScopeLock lock (lockHandle);
		OnNodesTransformBegin (param);
	}
	
	// iterate over all physics bodies and get the tranformtaion matrix;
	for (dNewtonBody* body = GetFirstBody(); body; body = GetNextBody(body)) {
		SceneNode* const node = (SceneNode*) body->GetUserData();
		//if (node && !body->GetSleepState()) {
		if (node) {
			dMatrix matrix;
			body->GetVisualMatrix (param, &matrix[0][0]);
			dQuaternion rot (matrix);

			Vector3 posit (matrix.m_posit.m_x, matrix.m_posit.m_y, matrix.m_posit.m_z);
			Quaternion rotation (rot.m_q0, rot.m_q1, rot.m_q2, rot.m_q3);

			//Node* const nodeParent = node->getParent();
			//const Vector3& derivedScale = nodeParent->_getDerivedScale();
			//const Vector3& derivedPosition = nodeParent->_getDerivedPosition();
			//const Quaternion derivedRotationInv (nodeParent->_getDerivedOrientation().Inverse());
			//node->setPosition (derivedRotationInv * (posit - derivedPosition) / derivedScale);
			//node->setOrientation (derivedRotationInv * nodeRotation);

			node->setPosition (posit);
			node->setOrientation (rotation);

			// update the application user data (that need to be update at rendering time, ex animations, particles emmitions, etc)
			body->OnApplicationPostTransform (applicationTime);
		}
	}
	dNewton::ScopeLock lock (lockHandle);
	OnNodesTransformEnd (param);

	m_physicUpdateTimestepInMocroseconds = GetTimeInMicrosenconds () - m_lastPhysicTimeInMicroseconds;
}
