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

class OgreNewtonBody: public dNewtonBody
{
	public:
	OgreNewtonBody (OgreNewtonWorld* const dWorld, Real mass, const dNewtonCollision* const collision, SceneNode* const node, const Matrix4& location);
	~OgreNewtonBody();
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

	Vector3 GetPointVeloc (const Vector3& point) const;
	void ApplyImpulseToDesiredPointVeloc (const Vector3& point, const Vector3& desiredveloc);

	static OgreNewtonBody* CreateBox(OgreNewtonWorld* const world, SceneNode* const sourceNode, Real mass, const Matrix4& matrix);
};


inline Real OgreNewtonBody::GetMass() const
{
	Real mass;
	Real Ixx;
	Real Iyy;
	Real Izz;	

	GetMassAndInertia (mass, Ixx, Iyy, Izz);
	return mass;
}

inline Vector3 OgreNewtonBody::GetInertia() const
{
	Real mass;
	Real Ixx;
	Real Iyy;
	Real Izz;	

	GetMassAndInertia (mass, Ixx, Iyy, Izz);
	return Vector3 (Ixx, Iyy, Izz);
}

inline Matrix4 OgreNewtonBody::GetMatrix() const
{
	Matrix4 matrix;
	dNewtonBody::GetMatrix(matrix[0]);
	return matrix.transpose();
}


inline void OgreNewtonBody::SetMatrix (const Matrix4& matrix)
{
	Matrix4 matrixTrans(matrix.transpose());
	dNewtonBody::SetMatrix(matrixTrans[0]);
}

inline void OgreNewtonBody::SetVeloc (const Vector3& veloc)
{
	dNewtonBody::SetVeloc(&veloc.x);
}

inline Vector3 OgreNewtonBody::GetVeloc () const
{
	Vector3 veloc;
	dNewtonBody::GetOmega(&veloc.x);
	return veloc;
}

inline void OgreNewtonBody::SetOmega (const Vector3& omega)
{
	dNewtonBody::SetOmega(&omega.x);
}

inline Vector3 OgreNewtonBody::GetOmega () const
{
	Vector3 omega;
	dNewtonBody::GetOmega(&omega.x);
	return omega;
}

inline void OgreNewtonBody::SetForce (const Vector3& force)
{
	dNewtonBody::SetForce (&force.x);
}

inline void OgreNewtonBody::SetTorque (const Vector3& torque)
{
	dNewtonBody::SetTorque (&torque.x);
}

inline Vector3 OgreNewtonBody::GetPointVeloc (const Vector3& point) const
{
	Vector3 veloc;
	dNewtonBody::GetPointVeloc (&point.x, &veloc.x);
	return veloc;
}

inline void OgreNewtonBody::ApplyImpulseToDesiredPointVeloc (const Vector3& point, const Vector3& desiredveloc)
{
	dNewtonBody::ApplyImpulseToDesiredPointVeloc (&point.x, &desiredveloc.x);
}


#endif
