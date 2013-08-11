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


#ifndef _OGRE_NEWTON_WORLD_H_
#define _OGRE_NEWTON_WORLD_H_

#include "OgreNewtonStdAfx.h"

using namespace Ogre;

class OgreNewtonInputManager;
class OgreNewtonPlayerManager;
class OgreNewtonRayPickManager;
class OgreNewtonTriggerManager;
class OgreNewtonArticulatedTransformManager;


class OGRE_NEWTON_API OgreNewtonWorld: public dNewton 
{
	public:
	OgreNewtonWorld (SceneManager* const manager, int updateFramerate);
	virtual ~OgreNewtonWorld();

	virtual void Update ();
	virtual void OnBeginUpdate (dFloat timestepInSecunds) = 0; 
	virtual void OnEndUpdate (dFloat timestepInSecunds) = 0; 
	virtual bool OnNodesTransformBegin(Real inteplationParam) = 0;
	virtual bool OnNodesTransformEnd(Real inteplationParam) = 0;

	const Vector3& GetGravity() const 
	{
		return m_gravity;
	}

	OgreNewtonInputManager* GetInputManager() const 
	{
		return m_inputManager;
	}

	OgreNewtonArticulatedTransformManager* GetHierarchyTransformManager() const 
	{
		return m_localTransformManager;
	}

	OgreNewtonTriggerManager* GetTriggerManager() const 
	{
		return m_triggerManager;
	}

	OgreNewtonPlayerManager* GetPlayerManager() const 
	{
		return m_playerManager;
	}

	OgreNewtonRayPickManager* GetRayPickManager() const 
	{
		return m_rayPickerManager;
	}


	void SetUpdateFPS(Real desiredFps, int maxUpdatesPerFrames = 3);
	void SetConcurrentUpdateMode (bool mode);
	bool GetConcurrentUpdateMode () const; 
	dNewtonBody* CreateBox(Ogre::SceneNode* const sourceNode);
	dLong GetPhysicsTimeInMicroSeconds() const;
	

	protected:
	SceneManager* m_sceneManager;
	OgreNewtonInputManager* m_inputManager;
	OgreNewtonPlayerManager* m_playerManager;
	OgreNewtonTriggerManager* m_triggerManager;
	OgreNewtonRayPickManager* m_rayPickerManager;
	OgreNewtonArticulatedTransformManager* m_localTransformManager;

	Vector3 m_gravity;
	Real m_timestep;
	bool m_concurrentUpdateMode;
	dLong m_lastPhysicTimeInMicroseconds;
	dLong m_physicUpdateTimestepInMocroseconds;
};

#endif
