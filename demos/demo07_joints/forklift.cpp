
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
#include <OgreNewtonMesh.h>
#include <OgreNewtonWorld.h>
#include <OgreNewtonRayCast.h>
#include <OgreNewtonDebugger.h>
#include <OgreNewtonSceneBody.h>
#include <OgreNewtonDynamicBody.h>
#include <OgreNewtonRayPickManager.h>
#include <OgreNewtonExampleApplication.h>
#include <OgreNewtonHierarchyTransformManager.h>

#include "Utils.h"
#include "ScreenWriter.h"
#include "OgreDotScene.h"
#include "DemoApplication.h"
#include "forklift.h"


using namespace Ogre;


class ForkliftSliderActuator: public dNewtonSliderActuator
{
	ForkliftSliderActuator (const dFloat* const pinAndPivotFrame, dNewtonDynamicBody* const child, dNewtonDynamicBody* const parent, DemoApplication* const application, Real minPosit, Real maxPosit)
		:dNewtonSliderActuator (pinAndPivotFrame, minPosit, maxPosit, child, parent)
		,m_base(parent)
		,m_movingPart(child)
		,m_application(application)
	{
	}

	public:
	CNEWTON_API virtual void OnSubmitConstraint (dFloat timestep, int threadIndex)
	{
		if (m_isTeeth) {
			const Real rate = 10.0f;
			if (m_application->m_keyboard->isKeyDown(OIS::KC_F)) {
				SetTargetPosit (GetActuatorPosit() + rate * timestep);
			} else if (m_application->m_keyboard->isKeyDown(OIS::KC_G)) {
				SetTargetPosit (GetActuatorPosit() - rate * timestep);
			} else {
				SetTargetPosit (GetActuatorPosit());
			}
		} else {
			const Real rate = 25.0f;
			if (m_application->m_keyboard->isKeyDown(OIS::KC_Q)) {
				SetTargetPosit (GetActuatorPosit() + rate * timestep);
			} else if (m_application->m_keyboard->isKeyDown(OIS::KC_E)) {
				SetTargetPosit (GetActuatorPosit() - rate * timestep);
			} else {
				SetTargetPosit (GetActuatorPosit());
			}
		}
		dNewtonSliderActuator::OnSubmitConstraint (timestep, threadIndex);
	}

	static void ConnectBase (OgreNewtonDynamicBody* const parent, OgreNewtonDynamicBody* const child, DemoApplication* const application)  
	{
		Matrix4 aligmentMatrix (Quaternion (Radian (3.141592f * 0.5f), Vector3 (0.0f, 0.0f, 1.0f)));
		Matrix4 baseMatrix((child->GetMatrix() * aligmentMatrix).transpose());
		ForkliftSliderActuator* const joint = new ForkliftSliderActuator (&baseMatrix[0][0], child, parent, application, -0.25f, 1.5f);
		joint->m_isTeeth = false;
	}

	static void ConnectTeeth (OgreNewtonDynamicBody* const parent, OgreNewtonDynamicBody* const child, DemoApplication* const application, Real dir)  
	{
		Matrix4 aligmentMatrix (Quaternion (Radian (dir * 3.141592f * 0.5f), Vector3 (0.0f, 1.0f, 0.0f)));
		Matrix4 baseMatrix((child->GetMatrix() * aligmentMatrix).transpose());
		ForkliftSliderActuator* const joint = new ForkliftSliderActuator (&baseMatrix[0][0], child, parent, application, -0.25, 0.25f);
		joint->m_isTeeth = true;
	}

	bool m_isTeeth;
	dNewtonDynamicBody* const m_base;
	dNewtonDynamicBody* const m_movingPart;
	DemoApplication* m_application;
};

class ForkliftHingeActuator: public dNewtonHingeActuator
{
	ForkliftHingeActuator (const dFloat* const pinAndPivotFrame, dNewtonDynamicBody* const child, dNewtonDynamicBody* const parent, DemoApplication* const application)
		:dNewtonHingeActuator (pinAndPivotFrame, -20.0f * 3.141592f / 180.0f, 20.0f * 3.141592f / 180.0f, child, parent)
		,m_base(parent)
		,m_movingPart(child)
		,m_application(application)
	{
	}

	public:
	CNEWTON_API virtual void OnSubmitConstraint (dFloat timestep, int threadIndex)
	{
		const Real rate = 25.0f;
		if (m_application->m_keyboard->isKeyDown(OIS::KC_Z)) {
			SetTargetAngle (GetActuatorAngle() + rate * timestep);
		} else if (m_application->m_keyboard->isKeyDown(OIS::KC_C)) {
			SetTargetAngle (GetActuatorAngle() - rate * timestep);
		} else {
			SetTargetAngle (GetActuatorAngle());
		}
		dNewtonHingeActuator::OnSubmitConstraint (timestep, threadIndex);
	}


	static void ConnectBase (OgreNewtonDynamicBody* const parent, OgreNewtonDynamicBody* const child, DemoApplication* const application)  
	{
		Matrix4 aligmentMatrix (Quaternion (Radian (3.141592f * 0.5f), Vector3 (0.0f, 1.0f, 0.0f)));
		Matrix4 baseMatrix((child->GetMatrix() * aligmentMatrix).transpose());
		new ForkliftHingeActuator (&baseMatrix[0][0], child, parent, application);
	}

	dNewtonDynamicBody* const m_base;
	dNewtonDynamicBody* const m_movingPart;
	DemoApplication* m_application;
};

class ForkliftFrontTireJoint: public dNewtonHingeJoint
{
	ForkliftFrontTireJoint (const dFloat* const pinAndPivotFrame, dNewtonDynamicBody* const tire, dNewtonDynamicBody* const mainBody, DemoApplication* const application)
		:dNewtonHingeJoint (pinAndPivotFrame, tire, mainBody)
		,m_tire(tire)
		,m_mainBody(mainBody)
		,m_application(application)
	{
		// calculate the maximum torque that the engine will produce
		dNewtonCollision* const tireShape = tire->GetCollision();
		dAssert (tireShape->GetType() == dNewtonCollision::m_chamferedCylinder);

		Vector3 p0;
		Vector3 p1;
		Matrix4 matrix (Matrix4::IDENTITY);
		tireShape->CalculateAABB (&matrix[0][0], &p0.x, &p1.x);

		Real Ixx;
		Real Iyy;
		Real Izz;
		Real mass;
		mainBody->GetMassAndInertia (mass, Ixx, Iyy, Izz);

		const Vector3& gravity = ((OgreNewtonWorld*) mainBody->GetNewton())->GetGravity();

		Real radius = (p1.y - p0.y) * 0.5f;

		// calculate a torque the will produce a 0.5f of the force of gravity
		m_maxEngineTorque = 0.25f * mass * radius * gravity.length();

		// calculate the coefficient of drag for top speed of 20 m/s
		Real maxOmega = 200.0f / radius;
		m_omegaResistance = 1.0f / maxOmega;
	}

	// apply the tractor torque to the tires here
	virtual void OnSubmitConstraint (dFloat timestep, int threadIndex) 
	{
		m_application->m_keyboard->capture();

		Real engineTorque = 0.0f;
		if (m_application->m_keyboard->isKeyDown(OIS::KC_W)) {
			engineTorque = -m_maxEngineTorque; 
		}

		if (m_application->m_keyboard->isKeyDown(OIS::KC_S)) {
			engineTorque = m_maxEngineTorque; 
		}

		Matrix4 matrix;
		m_mainBody->GetMatrix(&matrix[0][0]);
		matrix = matrix.transpose();
		matrix.setTrans(Vector3::ZERO);

		Vector3 tirePing (matrix * Vector3(1.0f, 0.0f, 0.0f));
		if (engineTorque != 0.0f) {
			Vector3 torque (tirePing * engineTorque);
			m_tire->AddTorque (&torque.x);
		}

		Vector3 omega;
		m_tire->GetOmega (&omega.x);
		Real omegaMag = omega.dotProduct(tirePing);
		Real sign = (omegaMag >= 0.0f) ? 1.0 : -1.0f;
		omega -= tirePing * (sign * omegaMag * omegaMag * m_omegaResistance);
		m_tire->SetOmega(&omega.x);

		dNewtonHingeJoint::OnSubmitConstraint (timestep, threadIndex);
	}

	public:
	static void ConnectTire (OgreNewtonDynamicBody* const body, OgreNewtonDynamicBody* const tire, DemoApplication* const application)  
	{
		Matrix4 tireMatrix(tire->GetMatrix());
		Matrix4 axisMatrix(body->GetMatrix());

		axisMatrix.setTrans(tireMatrix.getTrans());
		axisMatrix = axisMatrix.transpose();
		new ForkliftFrontTireJoint (&axisMatrix[0][0], tire, body, application);
	}

	dNewtonDynamicBody* const m_tire;
	dNewtonDynamicBody* const m_mainBody;
	DemoApplication* m_application;

	Real m_omegaResistance;
	Real m_maxEngineTorque;
};


class ForkliftRearTireJoint: public dNewtonUniversalActuator
{
	ForkliftRearTireJoint (const dFloat* const pinAndPivotFrame, dNewtonDynamicBody* const tire, dNewtonDynamicBody* const mainBody, DemoApplication* const application)
		:dNewtonUniversalActuator (pinAndPivotFrame, -30.0f * 3.141592f / 180.0f, 30.0f * 3.141592f / 180.0f, -30.0f * 3.141592f / 180.0f, 30.0f * 3.141592f / 180.0f, tire, mainBody)
		,m_tire(tire)
		,m_mainBody(mainBody)
		,m_application(application)
	{
		// disable the limits of the first row, so that it cna spinn free
		SetEnableFlag0 (false);
	}

	// control the steering angle here
	virtual void OnSubmitConstraint (dFloat timestep, int threadIndex) 
	{

		dNewtonUniversalActuator::OnSubmitConstraint (timestep, threadIndex);
	}

	public:
	static void ConnectTire (OgreNewtonDynamicBody* const body, OgreNewtonDynamicBody* const tire, DemoApplication* const application)  
	{
		Matrix4 tireMatrix;
		Matrix4 matrixOffset;

		tire->GetCollision()->GetMatrix(&matrixOffset[0][0]);
		tireMatrix = (tire->GetMatrix() * matrixOffset.transpose()).transpose();
		new ForkliftRearTireJoint (&tireMatrix[0][0], tire, body, application);
	}

	dNewtonDynamicBody* const m_tire;
	dNewtonDynamicBody* const m_mainBody;
	DemoApplication* m_application;
};



class LocalTransformCalculator: public OgreNewtonHierarchyTransformManager::OgreNewtonHierarchyTransformController
{
	public:
	LocalTransformCalculator (OgreNewtonHierarchyTransformManager* const manager)
		:OgreNewtonHierarchyTransformController(manager)
	{
	}

	~LocalTransformCalculator()
	{
	}

	virtual void* AddBone (dNewtonBody* const bone, const dFloat* const bindMatrix, void* const parentBodne)
	{	
		// add the bode to the controller
		void* const boneNode = OgreNewtonHierarchyTransformManager::OgreNewtonHierarchyTransformController::AddBone (bone, bindMatrix, parentBodne);

		// save the body handle as the used data pf the body collision for using in later in eh collision callback
		dNewtonCollision* const collision = bone->GetCollision();
		dAssert (!collision->GetUserData());
		collision->SetUserData (boneNode);

		return boneNode;
	}

	virtual void UpdateTransform (dNewtonBody* const bone, const dFloat* const localMatrix)
	{
		if (bone->GetSleepState()) {
			bone->Update (localMatrix);
		}
		bone->SetTargetMatrix (localMatrix);
	}
};


static OgreNewtonDynamicBody* ForkliftMakeMainBody (DemoApplication* const application, SceneNode* const node, const Vector3& origin)
{
	Entity* const ent = (Entity*) node->getAttachedObject (0);
	Vector3 scale (node->getScale());
	OgreNewtonMesh bodyMesh (application->GetPhysics(), ent);
	bodyMesh.ApplyTransform (Vector3::ZERO, scale, Quaternion::IDENTITY);
	dNewtonCollisionConvexHull bodyCollision (application->GetPhysics(), bodyMesh, m_allExcludingMousePick);
	Matrix4 bodyMatrix;
	bodyMatrix.makeTransform (node->_getDerivedPosition() + origin, Vector3 (1.0f, 1.0f, 1.0f), node->_getDerivedOrientation());
	return new OgreNewtonDynamicBody (application->GetPhysics(), 500.0f, &bodyCollision, node, bodyMatrix);
}


static OgreNewtonDynamicBody* ForkliftMakeBase (DemoApplication* const application, SceneNode* const baseNode, const Vector3& origin)
{
	Entity* const ent = (Entity*) baseNode->getAttachedObject (0);
	Vector3 scale (baseNode->getScale());
	OgreNewtonMesh bodyMesh (application->GetPhysics(), ent);
	bodyMesh.ApplyTransform (Vector3::ZERO, scale, Quaternion::IDENTITY);
	dNewtonCollisionConvexHull collision (application->GetPhysics(), bodyMesh, m_allExcludingMousePick);

	Matrix4 matrix;
	matrix.makeTransform(baseNode->_getDerivedPosition() + origin, Vector3 (1.0f, 1.0f, 1.0f), baseNode->_getDerivedOrientation());
	return new OgreNewtonDynamicBody (application->GetPhysics(), 50.0f, &collision, baseNode, matrix);
}

static OgreNewtonDynamicBody* ForkliftMakeTeeth (DemoApplication* const application, SceneNode* const baseNode, const Vector3& origin)
{
	Entity* const ent = (Entity*) baseNode->getAttachedObject (0);
	Vector3 scale (baseNode->getScale());
	OgreNewtonMesh mesh (application->GetPhysics(), ent);
	mesh.ApplyTransform (Vector3::ZERO, scale, Quaternion::IDENTITY);

	OgreNewtonMesh convexAproximation (application->GetPhysics());
	convexAproximation.CreateApproximateConvexDecomposition(mesh, 0.01f, 0.2f, 32, 100);
	dNewtonCollisionCompound compoundShape (application->GetPhysics(), convexAproximation, m_allExcludingMousePick);

	Matrix4 matrix;
	matrix.makeTransform(baseNode->_getDerivedPosition() + origin, Vector3 (1.0f, 1.0f, 1.0f), baseNode->_getDerivedOrientation());
	return new OgreNewtonDynamicBody (application->GetPhysics(), 50.0f, &compoundShape, baseNode, matrix);
}



static OgreNewtonDynamicBody* ForkliftMakeTire (DemoApplication* const application, SceneNode* const tireNode, const Vector3& origin)
{
	Entity* const ent = (Entity*) tireNode->getAttachedObject (0);
	Vector3 scale (tireNode->getScale());
	AxisAlignedBox box (ent->getBoundingBox());
	Real height = scale.z * (box.getMaximum().z - box.getMinimum().z);
	Real radius = scale.x * (box.getMaximum().x - box.getMinimum().x) * 0.5f - height * 0.5f;
	dNewtonCollisionChamferedCylinder shape (application->GetPhysics(), radius, height, m_allExcludingMousePick);

	Matrix4 aligmentMatrix (Quaternion (Radian (3.141592f * 0.5f), Vector3 (0.0f, 1.0f, 0.0f)));
	aligmentMatrix = aligmentMatrix.transpose();
	shape.SetMatrix (&aligmentMatrix[0][0]);

	Matrix4 matrix;
	matrix.makeTransform(tireNode->_getDerivedPosition() + origin, Vector3 (1.0f, 1.0f, 1.0f), tireNode->_getDerivedOrientation());
	return new OgreNewtonDynamicBody (application->GetPhysics(), 50.0f, &shape, tireNode, matrix);
}



OgreNewtonDynamicBody* LoadForklift (DemoApplication* const application, const Vector3& origin)
{
	SceneManager* const sceneMgr = application->GetSceneManager();
	// load the mode as Ogre node
	DotSceneLoader loader;
	SceneNode* const forkliftRoot = CreateNode (sceneMgr, NULL, Vector3::ZERO, Quaternion::IDENTITY);
	loader.parseDotScene ("forklift.scene", "Autodetect", sceneMgr, forkliftRoot);

	// find all vehicle components
	SceneNode* const bodyNode = (SceneNode*) forkliftRoot->getChild ("body");
	dAssert (bodyNode);

	SceneNode* const fl_tireNode = (SceneNode*) bodyNode->getChild ("fl_tire");
	SceneNode* const fr_tireNode = (SceneNode*) bodyNode->getChild ("fr_tire");
	SceneNode* const rl_tireNode = (SceneNode*) bodyNode->getChild ("rl_tire");
	SceneNode* const rr_tireNode = (SceneNode*) bodyNode->getChild ("rr_tire");
	dAssert (fl_tireNode);
	dAssert (fr_tireNode);
	dAssert (rl_tireNode);
	dAssert (rr_tireNode);

	SceneNode* const base1Node = (SceneNode*) bodyNode->getChild ("lift_1");
	SceneNode* const base2Node = (SceneNode*) base1Node->getChild ("lift_2");
	SceneNode* const base3Node = (SceneNode*) base2Node->getChild ("lift_3");
	SceneNode* const base4Node = (SceneNode*) base3Node->getChild ("lift_4");
	dAssert (base1Node);
	dAssert (base2Node);
	dAssert (base3Node);
	dAssert (base4Node);

	SceneNode* const leftTeethNode = (SceneNode*) base4Node->getChild ("left_teeth");
	SceneNode* const rightTeethNode = (SceneNode*) base4Node->getChild ("right_teeth");
	dAssert (leftTeethNode);
	dAssert (leftTeethNode);


	// make a local transform controller to control this body
	OgreNewtonHierarchyTransformManager* const localTransformManager = application->GetPhysics()->GetHierarchyTransformManager();
	LocalTransformCalculator* const transformCalculator = new LocalTransformCalculator(localTransformManager);

	//convert the body part to rigid bodies
	Matrix4 bindMatrix (Matrix4::IDENTITY);
	OgreNewtonDynamicBody* const mainBody = ForkliftMakeMainBody (application, bodyNode, origin);

	void* const parentBone = transformCalculator->AddBone (mainBody, &bindMatrix[0][0], NULL);

	// make the tires
	OgreNewtonDynamicBody* const frontLeftTireBody = ForkliftMakeTire (application, fl_tireNode, origin);
	OgreNewtonDynamicBody* const frontRightTireBody = ForkliftMakeTire (application, fr_tireNode, origin);
	OgreNewtonDynamicBody* const rearLeftTireBody = ForkliftMakeTire (application, rl_tireNode, origin);
	OgreNewtonDynamicBody* const rearRightTireBody = ForkliftMakeTire (application, rr_tireNode, origin);

	// make the lift base
	OgreNewtonDynamicBody* const base1 = ForkliftMakeBase (application, base1Node, origin);
	OgreNewtonDynamicBody* const base2 = ForkliftMakeBase (application, base2Node, origin);
	OgreNewtonDynamicBody* const base3 = ForkliftMakeBase (application, base3Node, origin);
	OgreNewtonDynamicBody* const base4 = ForkliftMakeBase (application, base4Node, origin);

	// make the left and right palette teeth
	OgreNewtonDynamicBody* const leftTeeth = ForkliftMakeTeeth (application, leftTeethNode, origin);
	OgreNewtonDynamicBody* const rightTeeth = ForkliftMakeTeeth (application, rightTeethNode, origin);

	// add the tire as children bodies
	transformCalculator->AddBone (frontLeftTireBody, &bindMatrix[0][0], parentBone);
	transformCalculator->AddBone (frontRightTireBody, &bindMatrix[0][0], parentBone);
	transformCalculator->AddBone (rearLeftTireBody, &bindMatrix[0][0], parentBone);
	transformCalculator->AddBone (rearRightTireBody, &bindMatrix[0][0], parentBone);

	// add the base bones
	void* const base1Bone = transformCalculator->AddBone (base1, &bindMatrix[0][0], parentBone);
	void* const base2Bone = transformCalculator->AddBone (base2, &bindMatrix[0][0], base1Bone);
	void* const base3Bone = transformCalculator->AddBone (base3, &bindMatrix[0][0], base2Bone);
	void* const base4Bone = transformCalculator->AddBone (base4, &bindMatrix[0][0], base3Bone);

	// add the teeth bode
	transformCalculator->AddBone (leftTeeth, &bindMatrix[0][0], base4Bone);
	transformCalculator->AddBone (rightTeeth, &bindMatrix[0][0], base4Bone);

	// connect the part with joints
	ForkliftFrontTireJoint::ConnectTire (mainBody, frontLeftTireBody, application);
	ForkliftFrontTireJoint::ConnectTire (mainBody, frontRightTireBody, application);
	ForkliftRearTireJoint::ConnectTire (mainBody, rearLeftTireBody, application);
	ForkliftRearTireJoint::ConnectTire (mainBody, rearRightTireBody, application);

	// connect the forklift base
	ForkliftHingeActuator::ConnectBase (mainBody, base1, application);
	ForkliftSliderActuator::ConnectBase (base1, base2, application);
	ForkliftSliderActuator::ConnectBase (base2, base3, application);
	ForkliftSliderActuator::ConnectBase (base3, base4, application);

	// connect the teeth
	ForkliftSliderActuator::ConnectTeeth (base4, leftTeeth, application, 1.0f);
	ForkliftSliderActuator::ConnectTeeth (base4, rightTeeth, application, -1.0f);

//ForkliftSliderActuator::ConnectTeeth (base2, leftTeeth, application, 1.0f);
//mainBody->SetSleepState(false);
//leftTeeth->SetSleepState(false);

	// disable self collision between all body parts
	transformCalculator->DisableAllSelfCollision();

	// save the main body as the player
	return mainBody;
}





