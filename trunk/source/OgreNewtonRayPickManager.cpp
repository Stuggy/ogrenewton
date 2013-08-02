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
#include "OgreNewtonRayPickManager.h"

class OgreNewtonRayPickManager::OgreNewtonRayPicker: public OgreNewtonRayCast
{
	public:
	OgreNewtonRayPicker(dNewton* const world)
		:OgreNewtonRayCast(world)
	{
	}

	dFloat OnRayHit (const dNewtonBody* const body, const dNewtonCollision* const shape, const dFloat* const contact, const dFloat* const normal, dLong collisionID, dFloat intersectParam)
	{
		if (intersectParam < m_param) {
			// skip static bodies
			Real mass = 0.0f;
			if (body->GetType() == dNewtonBody::m_dynamic) {
				mass = ((OgreNewtonDynamicBody*) body)->GetMass();
			} else {
				dAssert (0);
			}
			
			if (mass > 0.0f) {
				m_bodyHit = (dNewtonBody*) body;
				m_param = intersectParam;
				m_normal = Vector3 (normal[0], normal[1], normal[2]); 
				m_contact = Vector3 (contact[0], contact[1], contact[2]); 
			}
		}
		return intersectParam;
	}
};




OgreNewtonRayPickManager::OgreNewtonRayPickManager (OgreNewtonWorld* const world)
	:CustomControllerManager<OgreNewtonRayPickController>(world->GetNewton(), OGRE_NEWTON_RAY_PICKER_PLUGIN_NAME)
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


void OgreNewtonRayPickManager::PostUpdate (dFloat timestep)
{
	//do nothing;
}


dNewtonBody* OgreNewtonRayPickManager::RayCast (const Vector3& lineP0, const Vector3& lineP1, Real& pickParam) const
{
	OgreNewtonRayPicker rayPicker (m_world);

	rayPicker.CastRay(&lineP0.x, &lineP1.x, 0);
	pickParam = rayPicker.m_param;
	return rayPicker.m_bodyHit;
}	

void OgreNewtonRayPickManager::SetPickedBody (dNewtonBody* const body, const Vector3& handle)
{
	dNewton::ScopeLock scopelock (&m_lock);

	m_pickedBody = body;
	if (m_pickedBody) {
		Matrix4 matrix;
		if (m_pickedBody->GetType() == dNewtonBody::m_dynamic) {
			matrix = ((OgreNewtonDynamicBody*) body)->GetMatrix().inverseAffine();
		} else {
			dAssert (0);
		}
		m_localpHandlePoint = matrix.transformAffine(handle);
		m_globalTarget = handle;
	}
}

void OgreNewtonRayPickManager::SetTarget (const Vector3& targetPoint)
{
	dNewton::ScopeLock scopelock (&m_lock);
	m_globalTarget = targetPoint;
}


void OgreNewtonRayPickManager::PreUpdate(dFloat timestep)
{
	// all of the work will be done here;
	dNewton::ScopeLock scopelock (&m_lock);
	if (m_pickedBody) {
		if (m_pickedBody->GetType() == dNewtonBody::m_dynamic) {
			OgreNewtonDynamicBody* const body = (OgreNewtonDynamicBody*)m_pickedBody;
			const Vector3 peekTarget(m_globalTarget);
			const Vector3 peekLocalPosit (m_localpHandlePoint);

			Real invTimeStep = 1.0f / timestep;
			Matrix4 matrix (body->GetMatrix());
			Vector3 omega0 (body->GetOmega());
			Vector3 veloc0 (body->GetVeloc());

			Vector3 peekPosit (matrix.transformAffine(peekLocalPosit));
			Vector3 peekStep (peekTarget - peekPosit);

			Vector3 pointVeloc (body->GetPointVeloc (peekPosit));
			Vector3 deltaVeloc (peekStep * (m_stiffness * invTimeStep) - pointVeloc);

			for (int i = 0; i < 3; i ++) {
				Vector3 veloc (0.0f, 0.0f, 0.0f);
				veloc[i] = deltaVeloc[i];
				body->ApplyImpulseToDesiredPointVeloc (peekPosit, veloc);
			}
	//		Vector3 veloc1 (m_pickedBody->GetVeloc());
	//		Vector3 omega1 (m_pickedBody->GetOmega() * 0.9f);

			// restore body velocity and angular velocity
	//		m_pickedBody->SetOmega(omega0);
	//		m_pickedBody->SetVeloc(veloc0);

			// convert the delta velocity change to a external force and torque
	//		Vector3 inertia (m_pickedBody->GetInertia());
		
	//		matrix.setTrans(Vector3(0.0f, 0.0f, 0.0f));
	//		Matrix4 invMatrix (matrix.transpose());
	//		Vector3 angularMomentum (matrix.transformAffine(invMatrix.transformAffine(omega1 - omega0) * inertia));

	//		m_pickedBody->AddForce ((veloc1 - veloc0) * (m_pickedBody->GetMass() * invTimeStep));
	//		m_pickedBody->AddTorque(angularMomentum * invTimeStep);
		} else {
			dAssert (0);
		}
	}
}

