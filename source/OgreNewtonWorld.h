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
class OgreNewtonArticulationManager;


class OGRE_NEWTON_API OgreNewtonWorld: public dNewton 
{
	public:
	OgreNewtonWorld (int updateFramerate = 120.0f);
	virtual ~OgreNewtonWorld();

	// call each frame update to advance the physical world
	virtual void Update ();

	// called asynchronous at the beginning of a physics update. 
	virtual void OnBeginUpdate (dFloat timestepInSecunds){}

	// called asynchronous at the beginning end a physics update. 
	virtual void OnEndUpdate (dFloat timestepInSecunds){}

	// called synchronous with ogre update loop, at the beginning of setting all node transform after a physics update  
	virtual void OnNodesTransformBegin(Real interpolationParam){}

	// called synchronous with ogre update loop, at the end of setting all node transform after a physics update  
	virtual void OnNodesTransformEnd(Real interpolationParam){}

	const Vector3& GetGravity() const;
	OgreNewtonInputManager* GetInputManager() const ;
	OgreNewtonArticulationManager* GetHierarchyTransformManager() const; 
	OgreNewtonTriggerManager* GetTriggerManager() const; 
	OgreNewtonPlayerManager* GetPlayerManager() const; 
	OgreNewtonRayPickManager* GetRayPickManager() const; 

	void SetUpdateFPS(Real desiredFps, int maxUpdatesPerFrames = 3);
	void SetConcurrentUpdateMode (bool mode);
	bool GetConcurrentUpdateMode () const; 
	dLong GetPhysicsTimeInMicroSeconds() const;

	dMaterialPairManager::dMaterialPair* GetDefualtMaterialPair ();
	void AddMaterialPair (int materialId0, int materialId1, const dMaterialPairManager::dMaterialPair& pair);
	const dMaterialPairManager::dMaterialPair* GetMaterialPair (int materialId0, int materialId1, int threadIndex) const;

	protected:

	// broad phase AABB overlap
	virtual bool OnBodiesAABBOverlap (const dNewtonBody* const body0, const dNewtonBody* const body1, int threadIndex) const;
	virtual bool OnCompoundSubCollisionAABBOverlap (const dNewtonBody* const body0, const dNewtonCollision* const subShape0, const dNewtonBody* const body1, const dNewtonCollision* const subShape1, int threadIndex) const;
	virtual void OnContactProcess (dNewtonContactMaterial* const contacts, dFloat timestep, int threadIndex) const;

	dMaterialPairManager m_materialMap;
	OgreNewtonInputManager* m_inputManager;
	OgreNewtonPlayerManager* m_playerManager;
	OgreNewtonTriggerManager* m_triggerManager;
	OgreNewtonRayPickManager* m_rayPickerManager;
	OgreNewtonArticulationManager* m_localTransformManager;

	Vector3 m_gravity;
	Real m_timestep;
	bool m_concurrentUpdateMode;
	dLong m_lastPhysicTimeInMicroseconds;
	dLong m_physicUpdateTimestepInMocroseconds;
};

#endif
