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
#include "OgreNewtonRayCast.h"
#include "OgreNewtonRayPickManager.h"

class OgreNewtonRayPickManager::OgreNewtonRayPicker: public OgreNewtonRayCast
{
	public:
	OgreNewtonRayPicker(dNewton* const world)
		:OgreNewtonRayCast(world)
	{
	}

	dFloat OnRayHit (const dNewtonBody* const body, const dNewtonCollision* const shape, const dFloat* const contact, const dFloat* const normal, const int* const collisionID, dFloat intersectParam)
	{
		if (intersectParam < m_param) {
			// skip static bodies
			OgreNewtonBody* const ogreBody = (OgreNewtonBody*) body;
			Real mass = ogreBody->GetMass();
			if (mass > 0.0f) {
				m_bodyHit = ogreBody;
				m_param = intersectParam;
				m_normal = Vector3 (normal[0], normal[1], normal[2]); 
				m_contact = Vector3 (contact[0], contact[1], contact[2]); 
			}
		}
		return intersectParam;
	}
};




OgreNewtonRayPickManager::OgreNewtonRayPickManager (OgreNewtonWorld* const world)
	:CustomControllerManager<OgreNewtonRayPickController>(world->GetNewton(), OGRE_RAY_PEEKER_PLUGIN_NAME)
	,m_globalTarget (0.0f, 0.0f, 0.0f) 
	,m_localpHandlePoint (0.0f, 0.0f, 0.0f) 
	,m_world(world)
	,m_pickedBody(NULL)
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

OgreNewtonBody* OgreNewtonRayPickManager::PickBody (const Vector3& lineP0, const Vector3& lineP1, Vector3& hitPoint, Vector3& hitNormal) const
{
	OgreNewtonRayPicker rayPicker (m_world);

	rayPicker.CastRay(&lineP0.x, &lineP1.x, 0);

	hitPoint = rayPicker.m_contact;
	hitNormal = rayPicker.m_normal;
	return (OgreNewtonBody*) rayPicker.m_bodyHit;
}	

void OgreNewtonRayPickManager::SetPickedBody (OgreNewtonBody* const body, const Vector3& posit)
{
	dNewton::ScopeLock scopelock (&m_lock);

	m_pickedBody = body;
	if (m_pickedBody) {
		Matrix4 matrix (body->GetMatrix().inverseAffine());
		m_localpHandlePoint = matrix.transformAffine(posit);
		m_globalTarget = m_localpHandlePoint;
	}
}

void OgreNewtonRayPickManager::PreUpdate(dFloat timestep, int threadIndex)
{
	// all of the work will be done here;
	if (m_pickedBody) {
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

