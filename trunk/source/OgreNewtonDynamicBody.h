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


#ifndef _OGRE_NEWTON_BODY_H_
#define _OGRE_NEWTON_BODY_H_

#include "OgreNewtonStdAfx.h"

using namespace Ogre;
class OgreNewtonWorld;

class OGRE_NEWTON_API OgreNewtonDynamicBody: public dNewtonDynamicBody
{
	public:
	OgreNewtonDynamicBody (OgreNewtonWorld* const world, Real mass, const dNewtonCollision* const collision, SceneNode* const node, const Matrix4& location);
	~OgreNewtonDynamicBody();
	virtual void OnForceAndTorque (dFloat timestep, int threadIndex);
	
	Matrix4 GetMatrix() const;
	void SetMatrix (const Matrix4& matrix);

	Real GetMass() const; 
	Vector3 GetInertia() const ; 

	void SetVeloc (const Vector3& veloc);
	Vector3 GetVeloc () const;

	void SetOmega (const Vector3& omega);
	Vector3 GetOmega () const;

	void SetForce (const Vector3& force);
	void SetTorque (const Vector3& torque);

	void AddForce (const Vector3& force);
	void AddTorque (const Vector3& torque);

	Vector3 GetPointVeloc (const Vector3& point) const;
	void ApplyImpulseToDesiredPointVeloc (const Vector3& point, const Vector3& desiredveloc);

	protected:
	OgreNewtonDynamicBody();
};


inline Real OgreNewtonDynamicBody::GetMass() const
{
	Real mass;
	Real Ixx;
	Real Iyy;
	Real Izz;	

	GetMassAndInertia (mass, Ixx, Iyy, Izz);
	return mass;
}

inline Vector3 OgreNewtonDynamicBody::GetInertia() const
{
	Real mass;
	Real Ixx;
	Real Iyy;
	Real Izz;	

	GetMassAndInertia (mass, Ixx, Iyy, Izz);
	return Vector3 (Ixx, Iyy, Izz);
}

inline Matrix4 OgreNewtonDynamicBody::GetMatrix() const
{
	Matrix4 matrix;
	dNewtonDynamicBody::GetMatrix(matrix[0]);
	return matrix.transpose();
}


inline void OgreNewtonDynamicBody::SetMatrix (const Matrix4& matrix)
{
	Matrix4 matrixTrans(matrix.transpose());
	dNewtonDynamicBody::SetMatrix(matrixTrans[0]);
}

inline void OgreNewtonDynamicBody::SetVeloc (const Vector3& veloc)
{
	dNewtonDynamicBody::SetVeloc(&veloc.x);
}

inline Vector3 OgreNewtonDynamicBody::GetVeloc () const
{
	Vector3 veloc;
	dNewtonDynamicBody::GetOmega(&veloc.x);
	return veloc;
}

inline void OgreNewtonDynamicBody::SetOmega (const Vector3& omega)
{
	dNewtonDynamicBody::SetOmega(&omega.x);
}

inline Vector3 OgreNewtonDynamicBody::GetOmega () const
{
	Vector3 omega;
	dNewtonDynamicBody::GetOmega(&omega.x);
	return omega;
}

inline void OgreNewtonDynamicBody::SetForce (const Vector3& force)
{
	dNewtonDynamicBody::SetForce (&force.x);
}

inline void OgreNewtonDynamicBody::SetTorque (const Vector3& torque)
{
	dNewtonDynamicBody::SetTorque (&torque.x);
}

inline void OgreNewtonDynamicBody::AddForce (const Vector3& force)
{
	dNewtonDynamicBody::AddForce (&force.x);
}

inline void OgreNewtonDynamicBody::AddTorque (const Vector3& torque)
{
	dNewtonDynamicBody::AddTorque (&torque.x);
}


inline Vector3 OgreNewtonDynamicBody::GetPointVeloc (const Vector3& point) const
{
	Vector3 veloc;
	dNewtonDynamicBody::GetPointVeloc (&point.x, &veloc.x);
	return veloc;
}

inline void OgreNewtonDynamicBody::ApplyImpulseToDesiredPointVeloc (const Vector3& point, const Vector3& desiredveloc)
{
	dNewtonDynamicBody::ApplyImpulseToDesiredPointVeloc (&point.x, &desiredveloc.x);
}


#endif
