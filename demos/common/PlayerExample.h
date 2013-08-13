
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
#include <OgreNewtonPlayerManager.h>

using namespace Ogre;

class DemoApplication;

// subclass the basic player 
class MyPlayerContyroller: public OgreNewtonPlayerManager::OgreNetwonPlayer
{
	public:
	enum AnimID
	{
		ANIM_IDLE_BASE,
		ANIM_IDLE_TOP,
		ANIM_RUN_BASE,
		ANIM_RUN_TOP,
		ANIM_HANDS_CLOSED,
		ANIM_HANDS_RELAXED,
		ANIM_DRAW_SWORDS,
		ANIM_SLICE_VERTICAL,
		ANIM_SLICE_HORIZONTAL,
		ANIM_DANCE,
		ANIM_JUMP_START,
		ANIM_JUMP_LOOP,
		ANIM_JUMP_END,
		ANIM_NONE
	};

	MyPlayerContyroller (OgreNewtonPlayerManager* const manager, Entity* const playerMesh, SceneNode* const node, dFloat mass, dFloat outerRadius, dFloat innerRadius, dFloat playerHigh, dFloat stairStep, const Vector3& playerPivotOffset, const Quaternion& alignment, Real maxSpeed);

	void setBaseAnimation(AnimID id, bool reset = false);
	void setTopAnimation(AnimID id, bool reset = false);

	// called at rendering time, for game play stuff (we will update the player animations here
	void OnApplicationPostTransform (dFloat timestep); 

	// this is call from the Ogre main thread, you should not put too much work here
	// just read te input and save so that function OnPlayerMove use these values later on
	void ApplyPlayerInputs (const DemoApplication* const application, Real timestepInSecunds);

	// this is call from the Ogre main thread, you should not put too much work here
	// just set the desired speed and heading as determien by the AI so that OnPlayerMove use these values later on
	void ApplyNPCInputs (const DemoApplication* const application, Real timestepInSecunds);

	// called at physics time form the Physic thread, we most update the player motion here, 
	// do all of the player logic here, for now the basic play only apply the gravity, 
	// to do anything interesting we must overload OnPlayerMove
	void OnPlayerMove (Real timestep);


	public:	
	vector<AnimationState*>::type m_anims;
	//AnimationState* m_anims[NUM_ANIMS];    // master animation list
	//bool mFadingIn[NUM_ANIMS];          // which animations are fading in
	//bool mFadingOut[NUM_ANIMS];         // which animations are fading out

	AnimID mBaseAnimID;                   // current base (full- or lower-body) animation
	AnimID mTopAnimID;                    // current top (upper-body) animation
	Vector3 m_localOffset;
	Quaternion m_localRotation;

	//Real m_currentSpeed;
	Real m_currentHeadingAngle;
	Real m_currentCameraHeadingAngle;
	
	Real m_walkSpeed;
	Real m_strafeSpeed;
	Vector3 m_walkDirection;
	Real m_desiredStrafeSpeed;



	static String m_animNames[];
};


