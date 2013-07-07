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
	,m_cameraLock(0)
	,m_yawStep(0.0f)
	,m_pitchStep(0.0f)
	,m_yawAngle(0.0f)
	,m_pitchAngle(0.0f)
	,m_translation(0.0f, 0.0f, 0.0f)
	,m_translationStep(0.0f, 0.0f, 0.0f)
	,m_interpolatedCameraPosition(0.0f, 0.0f, 0.0f)
	,m_interpolatedCameraRotation (Quaternion::IDENTITY)
	,m_cameraTransform()
{
}

OgreNewtonExampleApplication::~OgreNewtonExampleApplication()
{
	if (m_debugRender) {
		mRoot->removeFrameListener(m_debugRender);
		delete m_debugRender;
	}
}


void OgreNewtonExampleApplication::createScene()
{
	OgreNewtonApplication::createScene();

	// create a debug Renderer for showing physics data visually
	m_debugRender = new OgreNewtonDebugger (mSceneMgr, m_physicsWorld);
	mRoot->addFrameListener(m_debugRender);
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
	rot.ToEulerAnglesZYX (rotY, rotX, rotZ);

	OgreNewtonWorld::ScopeLock lock (&m_cameraLock);
	m_yawAngle = rotY;
	m_pitchAngle = rotX;
	m_translation = posit;
	m_cameraTransform.ResetMatrix (&matrix[0][0]);
}

void OgreNewtonExampleApplication::MoveCamera (Real deltaTranslation, Real deltaStrafe, Radian pitchAngleStep, Radian yawAngleStep)
{
	OgreNewtonWorld::ScopeLock lock (&m_cameraLock);
	m_yawStep = yawAngleStep;
	m_pitchStep = pitchAngleStep;
	m_translationStep = Vector3 (deltaStrafe, 0.0f, deltaTranslation);
}


void OgreNewtonExampleApplication::GetInterpolatedCameraMatrix (Vector3& cameraPosit, Quaternion& cameraRotation)
{
	OgreNewtonWorld::ScopeLock lock (&m_cameraLock);
	cameraPosit = m_interpolatedCameraPosition;
	cameraRotation = m_interpolatedCameraRotation;
}


void OgreNewtonExampleApplication::OnPhysicUpdateBegin(dFloat timestepInSecunds) 
{
	// here we update the camera movement at simulation rate
	OgreNewtonWorld::ScopeLock lock (&m_cameraLock);

	m_yawAngle = fmodf (m_yawAngle.valueRadians() + m_yawStep.valueRadians(), 3.141592f * 2.0f);
	m_pitchAngle = Math::Clamp (m_pitchAngle.valueRadians() + m_pitchStep.valueRadians(), - 80.0f * 3.141592f / 180.0f, 80.0f * 3.141592f / 180.0f);

	Matrix3 rot; 
	rot.FromEulerAnglesZYX (m_yawAngle, m_pitchAngle, Radian (0.0f));
	Matrix4 matrix (rot);
	matrix.setTrans(m_translation + m_translationStep);
	matrix = matrix.transpose();
	m_cameraTransform.Update (matrix[0]);
}

void OgreNewtonExampleApplication::OnPhysicUpdateEnd(dFloat timestepInSecunds) 
{
}

void OgreNewtonExampleApplication::OnRenderUpdateBegin(dFloat updateParam) 
{
	Matrix4 cameraMatrix;
	OgreNewtonWorld::ScopeLock lock (&m_cameraLock);
	m_cameraTransform.InterplateMatrix (updateParam, cameraMatrix[0]);
	cameraMatrix = cameraMatrix.transpose();
	m_interpolatedCameraPosition = cameraMatrix.getTrans();
	m_interpolatedCameraRotation = cameraMatrix.extractQuaternion();
}


void OgreNewtonExampleApplication::OnRenderUpdateEnd(dFloat updateParam) 
{
}

