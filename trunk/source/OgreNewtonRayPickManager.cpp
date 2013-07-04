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
#include "OgreNewtonBody.h"
#include "OgreNewtonWorld.h"
#include "OgreNewtonRayPickManager.h"


OgreNewtonRayPickManager::OgreNewtonRayPickManager (OgreNewtonWorld* const world)
	:CustomControllerManager<OgreNewtonRayPickController>(world->GetNewton(), OGRE_RAY_PEEKER_PLUGIN_NAME)
	,m_globalTarget (0.0f, 0.0f, 0.0f) 
	,m_localpHandlePoint (0.0f, 0.0f, 0.0f) 
	,m_peekBody(NULL)
	,m_stiffness(0.25f)
	,m_lock(0)
{
}

OgreNewtonRayPickManager::~OgreNewtonRayPickManager()
{
}


void OgreNewtonRayPickManager::PostUpdate (const NewtonWorld* const world, void* const listenerUserData, dFloat timestep)
{
	//do nothing;
}

void OgreNewtonRayPickManager::GetAndClearPosition (Vector3& localPosit, Vector3& targetPosit)
{
	dNewton::ScopeLock scopelock (&m_lock);
	localPosit = m_localpHandlePoint;
	targetPosit = m_globalTarget;

	m_localpHandlePoint = Vector3 (0.0f, 0.0f, 0.0f);
	m_globalTarget = Vector3 (0.0f, 0.0f, 0.0f);
}

void OgreNewtonRayPickManager::SetPickedBody (OgreNewtonBody* const body, const Vector3& posit)
{
	dNewton::ScopeLock scopelock (&m_lock);

	m_peekBody = body;
	if (m_peekBody) {
		Matrix4 matrix (body->GetMatrix().inverseAffine());
		m_localpHandlePoint = matrix.transformAffine(posit);
		m_globalTarget = m_localpHandlePoint;
	}
}

void OgreNewtonRayPickManager::PreUpdate(dFloat timestep, int threadIndex)
{
	// all of the work will be done here;
	if (m_peekBody) {
		dAssert (0);
/*
		Vector3 peekTarget;
		Vector3 peekLocalPosit;
		GetAndClearPosition (peekLocalPosit, peekTarget);

		Real invTimeStep = 1.0f / timestep;
		Matrix4 matrix (m_peekBody->GetMatrix());
		Vector3 omega0 (m_peekBody->GetOmega());
		Vector3 veloc0 (m_peekBody->GetVeloc());

		Vector3 peekPosit (matrix.transformAffine(peekLocalPosit));
		Vector3 peekStep (peekTarget - peekPosit);

		Vector3 pointVeloc (m_peekBody->GetPointVeloc (peekPosit));
		Vector3 deltaVeloc (peekStep * (m_stiffness * invTimeStep) - pointVeloc);

		for (int i = 0; i < 3; i ++) {
			Vector3 veloc (0.0f, 0.0f, 0.0f);
			veloc[i] = deltaVeloc[i];
			m_peekBody->ApplyImpulseToDesiredPointVeloc (peekPosit, veloc);
		}

		Real mass = m_peekBody->GetMass ();
		Vector3 omega1 (m_peekBody->GetOmega());
		Vector3 veloc1 (m_peekBody->GetVeloc());
		Vector3 inerta (m_peekBody->GetInertia());

		m_peekBody->SetVeloc (veloc0);
		m_peekBody->SetOmega (omega0);

		dMatrix Inertia;
		Inertia[0] = matrix[0].Scale (Ixx);
		Inertia[1] = matrix[1].Scale (Iyy);
		Inertia[2] = matrix[2].Scale (Izz);
		Inertia[3] = dVector (0.0f, 0.0f, 0.0f, 1.0f);
		force = (veloc1 - veloc0).Scale (mass * invTimeStep);
		torque = Inertia.RotateVector (matrix.UnrotateVector(omega1 - omega0)).Scale (invTimeStep);
*/
	}
}

