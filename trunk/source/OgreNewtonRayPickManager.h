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


#ifndef _OGRE_NEWTON_RAY_PICKING_MANAGER_H_
#define _OGRE_NEWTON_RAY_PICKING_MANAGER_H_

#include "OgreNewtonStdAfx.h"

using namespace Ogre;

#define OGRE_NEWTON_RAY_PICKER_PLUGIN_NAME	"__rayPickManager__"

class OgreNewtonWorld;

class OGRE_NEWTON_API OgreNewtonRayPickController: public CustomControllerBase
{
	public:

	bool operator == (const OgreNewtonRayPickController& other ) const
	{
		dAssert (0);
		return false;
	}

	virtual void PreUpdate(dFloat timestep, int threadIndex)
	{
		//do nothing;
	}

	virtual void PostUpdate(dFloat timestep, int threadIndex)
	{
		//do nothing;
	}
};


class OGRE_NEWTON_API OgreNewtonRayPickManager: public CustomControllerManager<OgreNewtonRayPickController> 
{
	public:
	class OgreNewtonRayPicker;

	OgreNewtonRayPickManager (OgreNewtonWorld* const world, dLong collisionMask);
	virtual ~OgreNewtonRayPickManager();

	void PreUpdate(dFloat timestep);
	void PostUpdate (dFloat timestep);

	void SetCollisionMask(dLong mask);
	dLong GetCollisionMask() const;

	dNewtonBody* RayCast (const Vector3& lineP0, const Vector3& lineP1, Real& pickParam) const;

	void SetTarget (const Vector3& targetPoint);
	void SetPickedBody (dNewtonBody* const body, const Vector3& handle = Vector3 (0.0f, 0.0f, 0.0f));

	virtual void Debug () const {};

	protected:
	Vector3 m_globalTarget;
	Vector3 m_localpHandlePoint;
	OgreNewtonWorld* m_world;
	dNewtonBody* m_pickedBody;
	dLong m_collisionMask;
	Real m_stiffness;
	unsigned m_lock;
};

#endif
