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


#include "OgreNewtonExample.h"

/*
#include "OgreNewtonWorld.h"
#include "OgreNewtonDebugger.h"
#include "OgreNewtonExampleApplication.h"
#include "OgreNewtonArticulatedTransformManager.h"


OgreNewtonExampleApplication::OgreNewtonPhysicsListener::OgreNewtonPhysicsListener (OgreNewtonExampleApplication* const application, int updateFramerate)
	:FrameListener()
	,OgreNewtonWorld (application->mSceneMgr, updateFramerate)
	,m_application (application)
	,m_continueApplication(false)
{
}

OgreNewtonExampleApplication::OgreNewtonPhysicsListener::~OgreNewtonPhysicsListener()
{
}

bool OgreNewtonExampleApplication::OgreNewtonPhysicsListener::frameStarted(const FrameEvent &evt)
{
	Update();
	return m_continueApplication;
}


void OgreNewtonExampleApplication::OgreNewtonPhysicsListener::OnBeginUpdate (dFloat timestepInSecunds)
{
	m_application->OnPhysicUpdateBegin(timestepInSecunds);
}

void OgreNewtonExampleApplication::OgreNewtonPhysicsListener::OnEndUpdate (dFloat timestepInSecunds)
{
	m_application->OnPhysicUpdateEnd(timestepInSecunds);
}

void OgreNewtonExampleApplication::OgreNewtonPhysicsListener::OnNodesTransformBegin(Real interpolationParam)
{
	m_continueApplication = m_application->OnRenderUpdateBegin(interpolationParam);
}

void OgreNewtonExampleApplication::OgreNewtonPhysicsListener::OnNodesTransformEnd(Real interpolationParam)
{
	m_application->OnRenderUpdateEnd(interpolationParam);
}

bool OgreNewtonExampleApplication::OgreNewtonPhysicsListener::OnBodiesAABBOverlap (const dNewtonBody* const body0, const dNewtonBody* const body1, int threadIndex) const
{
	dNewtonCollision* const collision0 = body0->GetCollision();
	dNewtonCollision* const collision1 = body1->GetCollision();

	// check if these twp collision shape are part of a hierarchical model
	void* const node0 = collision0->GetUserData();
	void* const node1 = collision1->GetUserData();
	if (node0 && node1) {
		const OgreNewtonPhysicsListener* const world = this;
		//both collision are child nodes, check if there are self colliding
		return world->GetHierarchyTransformManager()->SelfCollisionTest (node0, node1);
	}
	
	// check all other collision using the bitfield mask, 
	//for now simple return true
	return true;
}

bool OgreNewtonExampleApplication::OgreNewtonPhysicsListener::OnCompoundSubCollisionAABBOverlap (const dNewtonBody* const body0, const dNewtonCollision* const subShape0, const dNewtonBody* const body1, const dNewtonCollision* const subShape1, int threadIndex) const
{
	return true;
}

void OgreNewtonExampleApplication::OgreNewtonPhysicsListener::OnContactProcess (dNewtonContactMaterial* const contactMaterial, dFloat timestep, int threadIndex) const
{
	OgreNewtonWorld::OnContactProcess (contactMaterial, timestep, threadIndex);
}

OgreNewtonExampleApplication::OgreNewtonExampleApplication()
	:ExampleApplication()
	,m_debugRender(NULL)
	,m_physicsWorld(NULL)
	,m_cameraYawAngle(0.0f)
	,m_cameraPitchAngle(0.0f)
	,m_cameraTranslation(0.0f, 0.0f, 0.0f)
	,m_interpolatedCameraPosition(0.0f, 0.0f, 0.0f)
	,m_interpolatedCameraRotation (Quaternion::IDENTITY)
	,m_cameraTransform()
{
}

OgreNewtonExampleApplication::~OgreNewtonExampleApplication()
{
	delete m_debugRender;
	delete m_physicsWorld;
}


void OgreNewtonExampleApplication::createScene()
{
	// create the physic world first
	m_physicsWorld = new OgreNewtonPhysicsListener (this);
	mRoot->addFrameListener(m_physicsWorld);

	// create a debug Renderer for showing physics data visually
	m_debugRender = new OgreNewtonDebugger (mSceneMgr, m_physicsWorld);
	mRoot->addFrameListener(m_debugRender);
}

void OgreNewtonExampleApplication::destroyScene(void)
{
	// make sure that no physics updated is in progress before shutting down all systems
	m_physicsWorld->WaitForUpdateToFinish ();

	// destroy the debugger
	m_debugRender->HideDebugInformation();
	
	// destroy the rest of the Ogre world
	ExampleApplication::destroyScene();
}


void OgreNewtonExampleApplication::ResetCamera (const Vector3& posit, const Quaternion& rotation)
{
	Matrix4 matrix;
	matrix.makeTransform (posit, Vector3 (1.0f, 1.0f, 1.0f), rotation);
	matrix = matrix.transpose();

	Matrix3 rot;
	matrix.extract3x3Matrix(rot);

	Radian rotX;
	Radian rotY;
	Radian rotZ;
	rot.ToEulerAnglesZYX (rotZ, rotY, rotX);

	m_cameraYawAngle = rotY;
	m_cameraPitchAngle = rotX;
	m_cameraTranslation = posit;
	m_cameraTransform.ResetMatrix (&matrix[0][0]);

	Matrix4 cameraMatrix;
	m_cameraTransform.InterplateMatrix (0.0f, cameraMatrix[0]);
	cameraMatrix = cameraMatrix.transpose();
	m_interpolatedCameraPosition = cameraMatrix.getTrans();
	m_interpolatedCameraRotation = cameraMatrix.extractQuaternion();
}

void OgreNewtonExampleApplication::MoveCamera (Real deltaTranslation, Real deltaStrafe, Radian pitchAngleStep, Radian yawAngleStep)
{
	// here we update the camera movement at simulation rate
	m_cameraYawAngle = fmodf (m_cameraYawAngle.valueRadians() + yawAngleStep.valueRadians(), 3.141592f * 2.0f);
	m_cameraPitchAngle = Math::Clamp (m_cameraPitchAngle.valueRadians() + pitchAngleStep.valueRadians(), - 80.0f * 3.141592f / 180.0f, 80.0f * 3.141592f / 180.0f);

	Matrix3 rot; 
	rot.FromEulerAnglesZYX (Radian (0.0f), m_cameraYawAngle, m_cameraPitchAngle);
	Matrix4 matrix (rot);
	m_cameraTranslation += Vector3 (matrix[0][2], matrix[1][2], matrix[2][2]) * deltaTranslation;   
	m_cameraTranslation += Vector3 (matrix[0][0], matrix[1][0], matrix[2][0]) * deltaStrafe;   

	matrix.setTrans(m_cameraTranslation);
	matrix = matrix.transpose();
	m_cameraTransform.Update (matrix[0]);
}

Real OgreNewtonExampleApplication::GetCameraYawAngle() const 
{
	return m_cameraYawAngle.valueRadians();  
}

void OgreNewtonExampleApplication::GetInterpolatedCameraMatrix (Vector3& cameraPosit, Quaternion& cameraRotation)
{
	cameraPosit = m_interpolatedCameraPosition;
	cameraRotation = m_interpolatedCameraRotation;
}

bool OgreNewtonExampleApplication::OnRenderUpdateBegin(dFloat updateParam) 
{
	Matrix4 cameraMatrix;
	m_cameraTransform.InterplateMatrix (updateParam, cameraMatrix[0]);
	cameraMatrix = cameraMatrix.transpose();

	m_interpolatedCameraPosition = cameraMatrix.getTrans();
	m_interpolatedCameraRotation = cameraMatrix.extractQuaternion();
	return true;
}


bool OgreNewtonExampleApplication::OnRenderUpdateEnd(dFloat updateParam) 
{
	return true;
}

*/


