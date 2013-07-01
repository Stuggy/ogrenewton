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


OgreNewtonRayCast::OgreNewtonRayCast(dNewton* const world)
	:dNewtonRayCast(world)
	,m_param(1.0f)
{
}

void OgreNewtonRayCast::CastRay (const dFloat* const p0, const dFloat* const p1, int threadIndex)
{
	m_param = 1.2f;
	m_bodyHit = NULL;
	dNewtonRayCast::CastRay(p0, p1);
}

dFloat OgreNewtonRayCast::OnRayHit (const dNewtonBody* const body, const dNewtonCollision* const shape, const dFloat* const contact, const dFloat* const normal, const int* const collisionID, dFloat intersectParam)
{
	if (intersectParam < m_param) {
		m_param = intersectParam;
		m_normal = Vector3 (normal[0], normal[1], normal[2]); 
		m_contact = Vector3 (contact[0], contact[1], contact[2]); 
	}
	return intersectParam;
}
