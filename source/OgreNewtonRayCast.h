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


#ifndef _OGRE_NEWTON_RAYCAST_H_
#define _OGRE_NEWTON_RAYCAST_H_

#include "OgreNewtonStdAfx.h"

using namespace Ogre;


class OGRE_NEWTON_API OgreNewtonRayCast: public dNewtonRayCast
{
	public:
	OgreNewtonRayCast(dNewton* const world, dLong collisionMask);
	void CastRay (const dFloat* const p0, const dFloat* const p1, int threadIndex = 0);
	dFloat OnRayHit (const dNewtonBody* const body, const dNewtonCollision* const shape, const dFloat* const contact, const dFloat* const normal, dLong collisionID, dFloat intersectParam);

	Vector3 m_normal;
	Vector3 m_contact;
	dNewtonBody* m_bodyHit;
	dNewtonCollision* m_shapeHit;
	dLong m_shapeId;
	Real m_param;
};

class OgreNewtonRayHitBody
{
	public:
	OgreNewtonRayHitBody(){}
	OgreNewtonRayHitBody (const dNewtonBody* const bodyHit, const dNewtonCollision* const shapeHit, const Vector3& normal, const Vector3& contact, dLong shapeId, Real param)
		:m_normal(normal)
		,m_contact(contact)
		,m_bodyHit(bodyHit)
		,m_shapeHit(shapeHit)
		,m_shapeId(shapeId)
		,m_param(param)
	{
	}

	Vector3 m_normal;
	Vector3 m_contact;
	const dNewtonBody* m_bodyHit;
	const dNewtonCollision* m_shapeHit;
	dLong m_shapeId;
	Real m_param;
};


class OGRE_NEWTON_API OgreNewtonAllHitRayCast: public dNewtonRayCast, public std::vector<OgreNewtonRayHitBody>
{
	public:
	OgreNewtonAllHitRayCast(dNewton* const world, dLong collisionMask);
	void CastRay (const dFloat* const p0, const dFloat* const p1, int threadIndex = 0);
	dFloat OnRayHit (const dNewtonBody* const body, const dNewtonCollision* const shape, const dFloat* const contact, const dFloat* const normal, dLong collisionID, dFloat intersectParam);
};


#endif
