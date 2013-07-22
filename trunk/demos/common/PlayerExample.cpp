
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



#include <OgreNewtonStdAfx.h>
#include "PlayerExample.h"
#include "DemoApplication.h"

using namespace Ogre;

String MyPlayerContyroller::m_animNames[] = {"IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed", "DrawSwords",  "SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"};

MyPlayerContyroller::MyPlayerContyroller (OgreNewtonPlayerManager* const manager, Entity* const playerMesh, SceneNode* const node, dFloat mass, dFloat outerRadius, dFloat innerRadius, dFloat playerHigh, dFloat stairStep, const Vector3& playerPivotOffset, const Quaternion& alignment)
	:OgreNewtonPlayerManager::OgreNetwonPlayer (manager, node, mass, outerRadius, innerRadius, playerHigh, stairStep)
	,m_localOffset(playerPivotOffset)
	,m_localRotation (alignment)
	,m_currentSpeed(0.0f)
	,m_currentHeadingAngle(0.0f)
	,m_walkSpeed(2.0f)
	,m_strafeSpeed(1.0f)
	,m_walkDirection(0.0f, 0.0f, 0.0f)
	,m_desiredStrafeSpeed(0.0f)
{
	// set the blend mode here
	playerMesh->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);

	// populate our animation list
	for (int i = 0; i < (sizeof (m_animNames) / sizeof (m_animNames[0])); i++) {
		m_anims.push_back (playerMesh->getAnimationState(m_animNames[i]));
//		m_anims[i] = playerMesh->getAnimationState(animNames[i]);
		m_anims[i]->setLoop(true);
		//mFadingIn[i] = false;
		//mFadingOut[i] = false;
	}

	// start off in the idle state (top and bottom together)
	setBaseAnimation(ANIM_IDLE_BASE);
	setTopAnimation(ANIM_IDLE_TOP);

	// relax the hands since we're not holding anything
	m_anims[ANIM_HANDS_RELAXED]->setEnabled(true);
	//mSwordsDrawn = false;
}

void MyPlayerContyroller::setBaseAnimation(AnimID id, bool reset)
{
//	if (mBaseAnimID >= 0 && mBaseAnimID < NUM_ANIMS)
//	{
//		// if we have an old animation, fade it out
//		mFadingIn[mBaseAnimID] = false;
//		mFadingOut[mBaseAnimID] = true;
//	}

	mBaseAnimID = id;
	m_anims[id]->setEnabled(true);
	m_anims[id]->setWeight(1.0f);

//	if (id != ANIM_NONE)
//	{
//		// if we have a new animation, enable it and fade it in
//		m_anims[id]->setEnabled(true);
//		m_anims[id]->setWeight(0);
//		mFadingOut[id] = false;
//		mFadingIn[id] = true;
//		if (reset) m_anims[id]->setTimePosition(0);
//	}
}


void MyPlayerContyroller::setTopAnimation(AnimID id, bool reset)
{
	mTopAnimID = id;
	m_anims[id]->setEnabled(true);
	m_anims[id]->setWeight(1.0f);
/*
	if (mTopAnimID >= 0 && mTopAnimID < NUM_ANIMS)
	{
		// if we have an old animation, fade it out
		mFadingIn[mTopAnimID] = false;
		mFadingOut[mTopAnimID] = true;
	}

	mTopAnimID = id;

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		m_anims[id]->setEnabled(true);
		m_anims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset) m_anims[id]->setTimePosition(0);
	}
*/
}

// called at rendering time, for game play stuff (we will update the player animations here
void MyPlayerContyroller::OnApplicationPostTransform (dFloat timestep) 
{
	// need to translate the root matrix by the offset;
	SceneNode* const node = (SceneNode*) GetUserData();

	const Quaternion& rotation (node->getOrientation());
	node->setPosition (node->getPosition() + rotation * m_localOffset);
	node->setOrientation (rotation * m_localRotation);

	Real baseAnimSpeed = 1.0f;
	Real topAnimSpeed = 1.0f;

	// increment the current base and top animation times
	//if (mBaseAnimID != ANIM_NONE) m_anims[mBaseAnimID]->addTime(deltaTime * baseAnimSpeed);
	//if (mTopAnimID != ANIM_NONE) m_anims[mTopAnimID]->addTime(deltaTime * topAnimSpeed);

	m_anims[mBaseAnimID]->addTime (timestep * baseAnimSpeed);
	m_anims[mTopAnimID]->addTime (timestep * topAnimSpeed);
}

// this is call from the Ogre main thread, you should not put too much work here
// just read the input and save so that function OnPlayerMove use these values later on
void MyPlayerContyroller::ApplyPlayerInputs (const DemoApplication* const application, Real timestepInSecunds) 
{
	// lock the body while modifying values 
	dNewton::ScopeLock scopelock (&m_lock);


	m_walkDirection = Vector3 (0.0f, 0.0f, 0.0f);
	if (application->m_keyboard->isKeyDown(OIS::KC_W)) {
		m_walkDirection.z = 1.0f;
	} 

	if (application->m_keyboard->isKeyDown(OIS::KC_S)) {
		m_walkDirection.z = -1.0f;
	} 

	if (application->m_keyboard->isKeyDown(OIS::KC_A)) {
		m_walkDirection.x = 1.0f;
	} 

	if (application->m_keyboard->isKeyDown(OIS::KC_D)) {
		m_walkDirection.x = -1.0f;
	}
}

// this is call from the Ogre main thread, you should not put too much work here
// just set the desired speed and heading as determine by the AI so that OnPlayerMove use these values later on
void MyPlayerContyroller::ApplyNPCInputs (const DemoApplication* const application, Real timestepInSecunds) 
{
	// lock the body while modifying values 
	dNewton::ScopeLock scopelock (&m_lock);

	//TODO
	// this is call from the Ogre main thread, you should not put too much work here
	// just set the desired speed and heading as determine by the AI so that OnPlayerMove use these values later on

}


// called at physics time form the Physic thread, we most update the player motion here, 
// do all of the player logic here, for now the basic play only apply the gravity, 
// to do anything interesting we must overload OnPlayerMove
void MyPlayerContyroller::OnPlayerMove (Real timestep)
{
	dNewton::ScopeLock scopelock (&m_lock);

	Real strafeSpeed = 0.0f;
	m_currentSpeed = m_walkSpeed * Math::Sqrt (m_walkDirection.dotProduct(m_walkDirection));

	if (m_walkDirection.dotProduct(m_walkDirection) > 0.0f) {
		m_currentHeadingAngle = Math::ATan2 (m_walkDirection.x, m_walkDirection.z).valueRadians();
	}

	const OgreNewtonWorld* const world = (OgreNewtonWorld*) GetNewton();
	const Vector3& gravity = world->GetGravity();
	SetPlayerVelocity (m_currentSpeed, strafeSpeed, 0.0f, m_currentHeadingAngle, &gravity.x, timestep);
}

