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


#ifndef _OGRE_NEWTON_EXAMPLE_APPLICATION_H_
#define _OGRE_NEWTON_EXAMPLE_APPLICATION_H_

#include "OgreNewtonStdAfx.h"
#include "OgreNewtonWorld.h"
#include <ExampleApplication.h>
#include <ExampleFrameListener.h>

using namespace Ogre;

class OgreNewtonDebugger;
class OgreNewtonPlayerManager;
class OgreNewtonRayPickManager;
class OgreNewtonTriggerManager;
class OgreNewtonArticulatedTransformManager;


class OGRE_NEWTON_API OgreNewtonExampleApplication: public ExampleApplication
{
	public:
	class OGRE_NEWTON_API OgreNewtonPhysicsListener: public FrameListener, public OgreNewtonWorld
	{
		public:
		OgreNewtonPhysicsListener (OgreNewtonExampleApplication* const application, int updateFramerate = 120);
		~OgreNewtonPhysicsListener();
		bool frameStarted(const FrameEvent &evt);
		virtual void OnBeginUpdate (dFloat timestepInSecunds);
		virtual void OnEndUpdate (dFloat timestepInSecunds);

		virtual void OnNodesTransformBegin(Real interpolationParam);
		virtual void OnNodesTransformEnd(Real interpolationParam);

		virtual bool OnBodiesAABBOverlap (const dNewtonBody* const body0, const dNewtonBody* const body1, int threadIndex) const;
		virtual bool OnCompoundSubCollisionAABBOverlap (const dNewtonBody* const body0, const dNewtonCollision* const subShape0, const dNewtonBody* const body1, const dNewtonCollision* const subShape1, int threadIndex) const;
		virtual void OnContactProcess (dNewtonContactMaterial* const contacts, dFloat timestep, int threadIndex) const;

		protected:
		OgreNewtonExampleApplication* m_application;
		bool m_continueApplication;
	};


	OgreNewtonExampleApplication();
	virtual ~OgreNewtonExampleApplication();

	OgreNewtonWorld* GetPhysics() const 
	{ 
		return m_physicsWorld;
	}

	virtual void OnPhysicUpdateBegin(dFloat timestepInSecunds)
	{
	}

	virtual void OnPhysicUpdateEnd(dFloat timestepInSecunds)
	{
	}

	virtual bool OnRenderUpdateBegin(dFloat updateParam);
	virtual bool OnRenderUpdateEnd(dFloat updateParam);

	void ResetCamera (const Vector3& posit, const Quaternion& rot);
	void MoveCamera (Real deltaTranslation, Real deltaStrafe, Radian pitchAngleStep, Radian yawAngleStep);
	void GetInterpolatedCameraMatrix (Vector3& cameraPosit, Quaternion& cameraRotation);

	Real GetCameraYawAngle() const ;
	
	protected:
	virtual void createScene(void);
	virtual void destroyScene(void);

	protected:
	OgreNewtonDebugger* m_debugRender;
	OgreNewtonPhysicsListener* m_physicsWorld;

	Radian m_cameraYawAngle;
	Radian m_cameraPitchAngle;
	Vector3 m_cameraTranslation;
	Vector3 m_interpolatedCameraPosition;
	Quaternion m_interpolatedCameraRotation;
	dNewtonTransformLerp m_cameraTransform;
};



#endif
