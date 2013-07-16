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
	OgreNewtonRayCast(dNewton* const world);
	void CastRay (const dFloat* const p0, const dFloat* const p1, int threadIndex = 0);
	dFloat OnRayHit (const dNewtonBody* const body, const dNewtonCollision* const shape, const dFloat* const contact, const dFloat* const normal, const int* const collisionID, dFloat intersectParam);

	Vector3 m_normal;
	Vector3 m_contact;
	dNewtonBody* m_bodyHit;
	Real m_param;
};


#endif