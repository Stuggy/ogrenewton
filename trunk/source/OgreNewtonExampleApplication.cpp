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
#include "OgreNewtonDebugger.h"
#include "OgreNewtonExampleApplication.h"



OgreNewtonExampleApplication::OgreNewtonExampleApplication()
	:OgreNewtonApplication()
	,m_debugRender(NULL)
//	,m_cameraLock(0)
	,m_yawAngle(0.0f)
	,m_pitchAngle(0.0f)
	,m_translation(0.0f, 0.0f, 0.0f)
	,m_interpolatedCameraPosition(0.0f, 0.0f, 0.0f)
	,m_interpolatedCameraRotation (Quaternion::IDENTITY)
	,m_cameraTransform()
{
}

OgreNewtonExampleApplication::~OgreNewtonExampleApplication()
{
	delete m_debugRender;
}


void OgreNewtonExampleApplication::createScene()
{
	OgreNewtonApplication::createScene();

	// create a debug Renderer for showing physics data visually
	m_debugRender = new OgreNewtonDebugger (mSceneMgr, m_physicsWorld);
	mRoot->addFrameListener(m_debugRender);
}

void OgreNewtonExampleApplication::destroyScene(void)
{
	// make sure no update is in progress, before shutting down all systems
	GetPhysics()->WaitForUpdateToFinish ();

	// destroy the debugger
	m_debugRender->HideDebugInformation();
	
	// destroy the physics world		
	OgreNewtonApplication::destroyScene();
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

	m_yawAngle = rotY;
	m_pitchAngle = rotX;
	m_translation = posit;
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
//	OgreNewtonWorld::ScopeLock lock (&m_cameraLock);

	m_yawAngle = fmodf (m_yawAngle.valueRadians() + yawAngleStep.valueRadians(), 3.141592f * 2.0f);
	m_pitchAngle = Math::Clamp (m_pitchAngle.valueRadians() + pitchAngleStep.valueRadians(), - 80.0f * 3.141592f / 180.0f, 80.0f * 3.141592f / 180.0f);

	Matrix3 rot; 
	//rot.FromEulerAnglesZYX (m_yawAngle, m_pitchAngle, Radian (0.0f));
	rot.FromEulerAnglesZYX (Radian (0.0f), m_yawAngle, m_pitchAngle);
	Matrix4 matrix (rot);
	m_translation += Vector3 (matrix[0][2], matrix[1][2], matrix[2][2]) * deltaTranslation;   
	m_translation += Vector3 (matrix[0][0], matrix[1][0], matrix[2][0]) * deltaStrafe;   

	matrix.setTrans(m_translation);
	matrix = matrix.transpose();
	m_cameraTransform.Update (matrix[0]);
}


void OgreNewtonExampleApplication::GetInterpolatedCameraMatrix (Vector3& cameraPosit, Quaternion& cameraRotation)
{
//	OgreNewtonWorld::ScopeLock lock (&m_cameraLock);
	cameraPosit = m_interpolatedCameraPosition;
	cameraRotation = m_interpolatedCameraRotation;
}


void OgreNewtonExampleApplication::OnPhysicUpdateBegin(dFloat timestepInSecunds) 
{
	OgreNewtonApplication::OnPhysicUpdateBegin(timestepInSecunds);
}

void OgreNewtonExampleApplication::OnPhysicUpdateEnd(dFloat timestepInSecunds) 
{
	OgreNewtonApplication::OnPhysicUpdateEnd(timestepInSecunds);
}

bool OgreNewtonExampleApplication::OnRenderUpdateBegin(dFloat updateParam) 
{
	Matrix4 cameraMatrix;
//	OgreNewtonWorld::ScopeLock lock (&m_cameraLock);
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

