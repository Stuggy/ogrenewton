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


OgreNewtonBody::OgreNewtonBody (OgreNewtonWorld* const world, Real mass, const dNewtonCollision* const collision, SceneNode* const treeNode, const Matrix4& location)
	:dNewtonBody (world, mass, collision, treeNode, location.transpose()[0])
	,m_peekPosit (Vector3 (0.0f, 0.0f, 0.0f))
	,m_peekStep (Vector3 (0.0f, 0.0f, 0.0f))
{
}

OgreNewtonBody::~OgreNewtonBody()
{
}


void OgreNewtonBody::OnForceAndTorque (dFloat timestep, int threadIndex)
{
	const OgreNewtonWorld* const world = (OgreNewtonWorld*) GetNewton();
	SetForce (world->GetGravity() * GetMass());
	ApplyPeekImpulse(timestep);
}

void OgreNewtonBody::ApplyPeekImpulse (dFloat timestep)
{
	Vector3 peekPosit (0.0f, 0.0f, 0.0f);
	Vector3 peekStep (0.0f, 0.0f, 0.0f);
	{
		dNewton::ScopeLock scopelock (&m_lock);
		peekPosit = m_peekPosit;
		peekStep = m_peekStep;
		m_peekPosit = Vector3 (0.0f, 0.0f, 0.0f);
		m_peekStep = Vector3 (0.0f, 0.0f, 0.0f);
	}

	if ((peekPosit.dotProduct(peekPosit) != 0.0f) && (m_peekStep.dotProduct(m_peekStep) != 0.0f)) {

		//dVector com; 
		//dMatrix matrix; 
		//dVector omega0;
		//dVector veloc0;
		//dVector omega1;
		//dVector veloc1;
		//dVector pointVeloc;
		//Real Ixx;
		//Real Iyy;
		//Real Izz;
		//Real mass;

		Real invTimeStep = 1.0f / timestep;

		// calculate the desired impulse
		Matrix4 matrix (GetMatrix());

		Vector3 omega0 (GetOmega());
		Vector3 veloc0 (GetVeloc());
//		NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);

		Real stiffness = 0.3f;
		Vector3 pointVeloc (GetPointVeloc (peekPosit));
		Vector3 deltaVeloc (peekStep * (stiffness * invTimeStep) - pointVeloc);

		for (int i = 0; i < 3; i ++) {
			Vector3 veloc (0.0f, 0.0f, 0.0f);
			veloc[i] = deltaVeloc[i];
			ApplyImpulseToDesiredPointVeloc (peekPosit, veloc);
		}
/*
	NewtonBodyGetOmega (body, &omega1[0]);
	NewtonBodyGetVelocity (body, &veloc1[0]);
	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);

	dMatrix Inertia;
	Inertia[0] = matrix[0].Scale (Ixx);
	Inertia[1] = matrix[1].Scale (Iyy);
	Inertia[2] = matrix[2].Scale (Izz);
	Inertia[3] = dVector (0.0f, 0.0f, 0.0f, 1.0f);

	force = (veloc1 - veloc0).Scale (mass * invTimeStep);
	torque = Inertia.RotateVector (matrix.UnrotateVector(omega1 - omega0)).Scale (invTimeStep);

	// restore the body linear and angular velocity 
	NewtonBodySetOmega (body, &omega0[0]);
	NewtonBodySetVelocity (body, &veloc0[0]);

	// reset the impulse accumulators for the next update
	m_extLinearImpulse = dVector (0.0f, 0.0f, 0.0f, 0.0f);
	m_extAngularImpulse = dVector (0.0f, 0.0f, 0.0f, 0.0f);

	// clamp the extern force to no more than 5 gravities
	//const float gravities = 5.0f;
	//float maxWeight = peelNewton->mGravityMag * mass * gravities;
	//dVector force (cookie->m_extLinearImpulse.Scale (invTimeStep));
	//if ((force % force) > (maxWeight * maxWeight)) {
	//	force = force.Scale (maxWeight / dSqrt (force % force));
	//}
	//force += dVector (peelNewton->mGlobalGravity.x, peelNewton->mGlobalGravity.y, peelNewton->mGlobalGravity.z, 0.0f).Scale (mass);

	//dFloat inertia = dMax (dMax (Ixx, Iyy), Izz);
	//float maxTorque = inertia * gravities;
	//dVector torque (cookie->m_extAngularImpulse.Scale (invTimeStep));
	//if ((torque % torque) > maxTorque) {
	//	torque = torque.Scale (maxTorque / dSqrt (torque % torque));
	//}
*/
	}

}

OgreNewtonBody* OgreNewtonBody::CreateBox(OgreNewtonWorld* const world, SceneNode* const sourceNode, Real mass, const Matrix4& matrix)
{

	MovableObject* const attachedObject = sourceNode->getAttachedObject(0);

	Vector3 position       = sourceNode->getPosition();
	Vector3 size           = Vector3::UNIT_SCALE;
	Vector3 centerOffset   = Vector3::ZERO;
	Vector3 scale          = sourceNode->_getDerivedScale();
	Vector3 dimensions     = scale;
	Quaternion orientation = sourceNode->_getDerivedOrientation();

	AxisAlignedBox boundingBox = attachedObject->getBoundingBox();
	Vector3 padding = (boundingBox.getMaximum() - boundingBox.getMinimum()) * MeshManager::getSingleton().getBoundsPaddingFactor() * 2;

	size = (boundingBox.getMaximum() - boundingBox.getMinimum()) - padding * 2;
	centerOffset = boundingBox.getMinimum() + padding + (size / 2.0f);
//		dimensions = sourceNode->_getDerivedScale() * size;

	Matrix4 shapeMatrix;
	shapeMatrix.makeTransform(centerOffset * scale, Vector3(1.0f, 1.0f, 1.0f), Quaternion::IDENTITY);
	shapeMatrix = shapeMatrix.transpose();

	dNewtonCollisionBox boxShape (world, size.x, size.y, size.z, 0);
	boxShape.SetScale(scale.x, scale.y, scale.z);

//		modifiedCollision->setScalarMatrix(modifierMatrix);
	boxShape.SetMatrix (shapeMatrix[0]);

	OgreNewtonBody* const body = new OgreNewtonBody (world, mass, &boxShape, sourceNode, matrix);
/*
	if (materialId != NULL)
	{
		body->setMaterialGroupID(materialId);
	}

	Vector3 inertia, offset;
	modifiedCollision->calculateInertialMatrix(inertia, offset);

	body->setMassMatrix(mass, inertia * mass);
	body->setCenterOfMass(offset);
	body->setStandardForceCallback();
	body->attachNode(sourceNode);
	body->setPositionOrientation(position, orientation);

	boxes.insert(std::make_pair(sourceNode->getName(), body));
*/
	return body;
}

