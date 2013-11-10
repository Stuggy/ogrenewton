
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


#include "HelpersStdAfx.h"
#include "Utils.h"
#include "ScreenWriter.h"
#include "OgreDotScene.h"
#include "DemoApplication.h"
#include "Forklift.h"


using namespace Ogre;

#define BODY_MASS	800.0f
#define TIRE_MASS	50.0f
#define RACK_MASS	50.0f
#define TOOTH_MASS	50.0f

class ForkliftTireBody: public OgreNewtonDynamicBody
{
	public:
	ForkliftTireBody (OgreNewtonWorld* const world, Real mass, const dNewtonCollision* const collision, SceneNode* const node, const Matrix4& location, OgreNewtonDynamicBody* const rootBody)
		:OgreNewtonDynamicBody (world, mass, collision, node, location)
		,m_rootBody(rootBody)
	{
	}

	virtual void OnContactProcess (dNewtonContactMaterial* const contactMaterial, dFloat timestep, int threadIndex) const
	{
		Matrix4 tireMatrix (GetMatrix());
		Matrix4 chassis (m_rootBody->GetMatrix());
		
		chassis.setTrans (Vector3 (0.0f, 0.0f, 0.0f));
		tireMatrix.setTrans (Vector3 (0.0f, 0.0f, 0.0f));
		Vector3 upPin (chassis * Vector3 (0.0f, 1.0f, 0.0f));
		Vector3 axisPin (chassis * Vector3 (1.0f, 0.0f, 0.0f));

		Vector3 dir (axisPin.crossProduct (upPin));
		for (void* contact = contactMaterial->GetFirstContact(); contact; contact = contactMaterial->GetNextContact(contact)) {
			contactMaterial->RotateTangentDirections (contact, &dir.x);
		}
	}
	OgreNewtonDynamicBody* m_rootBody;
};


ForkliftPhysicsModel::ForkliftPhysicsModel (DemoApplication* const application, const char* const fileName, const Vector3& origin, const String& rootName)
	:OgreNewtonArticulatedTransformController(application->GetPhysics()->GetHierarchyTransformManager(), true)
	,m_application(application)
	,m_liftPosit(0.0f)
	,m_openPosit(0.0f)
	,m_tiltAngle(0.0f)
{
	SceneManager* const sceneMgr = application->GetSceneManager();

	// load the Ogre model form and Ogre scene 
	DotSceneLoader loader;
	SceneNode* const forkliftRoot = CreateNode (sceneMgr, NULL, Vector3::ZERO, Quaternion::IDENTITY);
	loader.parseDotScene (fileName, "Autodetect", sceneMgr, forkliftRoot, rootName);

	// find all vehicle components
	SceneNode* const bodyNode = (SceneNode*) forkliftRoot->getChild (rootName + "body");
	dAssert (bodyNode);

	SceneNode* const fl_tireNode = (SceneNode*) bodyNode->getChild (rootName + "fl_tire");
	SceneNode* const fr_tireNode = (SceneNode*) bodyNode->getChild (rootName + "fr_tire");
	SceneNode* const rl_tireNode = (SceneNode*) bodyNode->getChild (rootName + "rl_tire");
	SceneNode* const rr_tireNode = (SceneNode*) bodyNode->getChild (rootName + "rr_tire");
	dAssert (fl_tireNode);
	dAssert (fr_tireNode);
	dAssert (rl_tireNode);
	dAssert (rr_tireNode);

	SceneNode* const base1Node = (SceneNode*) bodyNode->getChild (rootName + "lift_1");
	SceneNode* const base2Node = (SceneNode*) base1Node->getChild (rootName + "lift_2");
	SceneNode* const base3Node = (SceneNode*) base2Node->getChild (rootName + "lift_3");
	SceneNode* const base4Node = (SceneNode*) base3Node->getChild (rootName + "lift_4");
	dAssert (base1Node);
	dAssert (base2Node);
	dAssert (base3Node);
	dAssert (base4Node);

	SceneNode* const leftTeethNode = (SceneNode*) base4Node->getChild (rootName + "left_teeth");
	SceneNode* const rightTeethNode = (SceneNode*) base4Node->getChild (rootName + "right_teeth");
	dAssert (leftTeethNode);
	dAssert (leftTeethNode);

	//convert the body part to rigid bodies
	Matrix4 bindMatrix (Matrix4::IDENTITY);
	m_rootBody = CreateRootBody (bodyNode, origin);
	void* const parentBone = AddBone (m_rootBody, &bindMatrix[0][0], NULL);

	// make the tires
	OgreNewtonDynamicBody* const rearLeftTireBody = CreateTireBody (rl_tireNode, origin);
	OgreNewtonDynamicBody* const rearRightTireBody = CreateTireBody (rr_tireNode, origin);
	m_frontTireBody[0] = CreateTireBody (fl_tireNode, origin);
	m_frontTireBody[1] = CreateTireBody (fr_tireNode, origin);

	// make the lift base
	OgreNewtonDynamicBody* const base1 = CreateBasePlatform (base1Node, origin);
	OgreNewtonDynamicBody* const base2 = CreateBasePlatform (base2Node, origin);
	OgreNewtonDynamicBody* const base3 = CreateBasePlatform (base3Node, origin);
	OgreNewtonDynamicBody* const base4 = CreateBasePlatform (base4Node, origin);

	// make the left and right palette teeth
	OgreNewtonDynamicBody* const leftTooth = CreateTooth (leftTeethNode, origin);
	OgreNewtonDynamicBody* const rightTooth = CreateTooth (rightTeethNode, origin);

	// add the tire as children bodies
	AddBone (rearLeftTireBody, &bindMatrix[0][0], parentBone);
	AddBone (rearRightTireBody, &bindMatrix[0][0], parentBone);
	AddBone (m_frontTireBody[0], &bindMatrix[0][0], parentBone);
	AddBone (m_frontTireBody[1], &bindMatrix[0][0], parentBone);
	// connect the tire to the root body 
	m_rearTire[0] = LinkRearTire (rearLeftTireBody);
	m_rearTire[1] = LinkRearTire (rearRightTireBody);
	m_frontTire[0] = LinkFrontTire (m_frontTireBody[0]);
	m_frontTire[1] = LinkFrontTire (m_frontTireBody[1]);


	// add the lifter apparatus bones 
	void* const base1Bone = AddBone (base1, &bindMatrix[0][0], parentBone);
	void* const base2Bone = AddBone (base2, &bindMatrix[0][0], base1Bone);
	void* const base3Bone = AddBone (base3, &bindMatrix[0][0], base2Bone);
	void* const base4Bone = AddBone (base4, &bindMatrix[0][0], base3Bone);
	// connect the forklift base
	m_revolvePlatform = LinkBasePlatform (base1);
	m_slidePlaforms[0] = LinkBasePlatform (base1, base2);
	m_slidePlaforms[1] = LinkBasePlatform (base2, base3);
	m_slidePlaforms[2] = LinkBasePlatform (base3, base4);

	// add the teeth bode
	AddBone (leftTooth, &bindMatrix[0][0], base4Bone);
	AddBone (rightTooth, &bindMatrix[0][0], base4Bone);
	// connect the teeth
	m_slideTooth[0] = LinkTooth (base4, leftTooth, 1.0f);
	m_slideTooth[1] = LinkTooth (base4, rightTooth, -1.0f);

	// calculate a fake engine 
	CalculateEngine (m_frontTireBody[0]);

	// disable self collision between all body parts
	DisableAllSelfCollision();
}

ForkliftPhysicsModel::~ForkliftPhysicsModel()
{
}


void* ForkliftPhysicsModel::AddBone (dNewtonBody* const bone, const dFloat* const bindMatrix, void* const parentBodne)
{	
	// add the bode to the controller
	void* const boneNode = OgreNewtonArticulationManager::OgreNewtonArticulatedTransformController::AddBone (bone, bindMatrix, parentBodne);
	return boneNode;
}


void ForkliftPhysicsModel::OnUpdateBoneTransform (dNewtonBody* const bone, const dFloat* const localMatrix)
{
	if (bone->GetSleepState()) {
		bone->Update (localMatrix);
	}
	bone->SetTargetMatrix (localMatrix);
}


OgreNewtonDynamicBody* ForkliftPhysicsModel::CreateRootBody (SceneNode* const node, const Vector3& origin)
{
	Entity* const ent = (Entity*) node->getAttachedObject (0);
	Vector3 scale (node->getScale());
	OgreNewtonMesh bodyMesh (m_application->GetPhysics(), ent);
	bodyMesh.ApplyTransform (Vector3::ZERO, scale, Quaternion::IDENTITY);
	dNewtonCollisionConvexHull bodyCollision (m_application->GetPhysics(), bodyMesh, m_allExcludingMousePick);
	Matrix4 bodyMatrix;
	bodyMatrix.makeTransform (node->_getDerivedPosition() + origin, Vector3 (1.0f, 1.0f, 1.0f), node->_getDerivedOrientation());
	OgreNewtonDynamicBody* const body = new OgreNewtonDynamicBody (m_application->GetPhysics(), BODY_MASS, &bodyCollision, node, bodyMatrix);

	// move the center of mass a little to the back	of the chassis
	Vector3 com;
	body->GetCenterOfMass (&com.x);
	com.z += 0.5f;
	com.y -= 0.2f;
	body->SetCenterOfMass (&com.x);
	
	return body;
}


OgreNewtonDynamicBody* ForkliftPhysicsModel::CreateTireBody (SceneNode* const tireNode, const Vector3& origin)
{
	Entity* const ent = (Entity*) tireNode->getAttachedObject (0);
	Vector3 scale (tireNode->getScale());
	AxisAlignedBox box (ent->getBoundingBox());
	Real height = scale.z * (box.getMaximum().z - box.getMinimum().z);
	Real radius = scale.x * (box.getMaximum().x - box.getMinimum().x) * 0.5f - height * 0.5f;
	dNewtonCollisionChamferedCylinder shape (m_application->GetPhysics(), radius, height, m_allExcludingMousePick);

	Matrix4 aligmentMatrix (Quaternion (Radian (3.141592f * 0.5f), Vector3 (0.0f, 1.0f, 0.0f)));
	aligmentMatrix = aligmentMatrix.transpose();
	shape.SetMatrix (&aligmentMatrix[0][0]);

	Matrix4 matrix;
	matrix.makeTransform(tireNode->_getDerivedPosition() + origin, Vector3 (1.0f, 1.0f, 1.0f), tireNode->_getDerivedOrientation());
	return new ForkliftTireBody (m_application->GetPhysics(), TIRE_MASS, &shape, tireNode, matrix, m_rootBody);
}

OgreNewtonDynamicBody* ForkliftPhysicsModel::CreateBasePlatform (SceneNode* const baseNode, const Vector3& origin)
{
	Entity* const ent = (Entity*) baseNode->getAttachedObject (0);
	Vector3 scale (baseNode->getScale());
	OgreNewtonMesh bodyMesh (m_application->GetPhysics(), ent);
	bodyMesh.ApplyTransform (Vector3::ZERO, scale, Quaternion::IDENTITY);
	dNewtonCollisionConvexHull collision (m_application->GetPhysics(), bodyMesh, m_allExcludingMousePick);

	Matrix4 matrix;
	matrix.makeTransform(baseNode->_getDerivedPosition() + origin, Vector3 (1.0f, 1.0f, 1.0f), baseNode->_getDerivedOrientation());
	return new OgreNewtonDynamicBody (m_application->GetPhysics(), RACK_MASS, &collision, baseNode, matrix);
}


OgreNewtonDynamicBody* ForkliftPhysicsModel::CreateTooth (SceneNode* const baseNode, const Vector3& origin)
{
	Entity* const ent = (Entity*) baseNode->getAttachedObject (0);
	Vector3 scale (baseNode->getScale());
	OgreNewtonMesh mesh (m_application->GetPhysics(), ent);
	mesh.ApplyTransform (Vector3::ZERO, scale, Quaternion::IDENTITY);

	OgreNewtonMesh convexAproximation (m_application->GetPhysics());
	convexAproximation.CreateApproximateConvexDecomposition(mesh, 0.01f, 0.2f, 32, 100);
	dNewtonCollisionCompound compoundShape (m_application->GetPhysics(), convexAproximation, m_allExcludingMousePick);

	Matrix4 matrix;
	matrix.makeTransform(baseNode->_getDerivedPosition() + origin, Vector3 (1.0f, 1.0f, 1.0f), baseNode->_getDerivedOrientation());
	return new OgreNewtonDynamicBody (m_application->GetPhysics(), TOOTH_MASS, &compoundShape, baseNode, matrix);
}

dNewtonHingeJoint* ForkliftPhysicsModel::LinkFrontTire (OgreNewtonDynamicBody* const tire)  
{
	Matrix4 tireMatrix(tire->GetMatrix());
	Matrix4 axisMatrix(m_rootBody->GetMatrix());

	axisMatrix.setTrans(tireMatrix.getTrans());
	axisMatrix = axisMatrix.transpose();
	dNewtonHingeJoint* const hinge = new dNewtonHingeJoint (&axisMatrix[0][0], tire, m_rootBody);
	return hinge;
}


dNewtonUniversalActuator* ForkliftPhysicsModel::LinkRearTire (OgreNewtonDynamicBody* const tire)  
{
	Matrix4 tireMatrix;
	Matrix4 matrixOffset;

	tire->GetCollision()->GetMatrix(&matrixOffset[0][0]);
	tireMatrix = (tire->GetMatrix() * matrixOffset.transpose()).transpose();

	dFloat angleLimit = 30.0f * 3.141592f / 180.0f;
	dFloat angularRate = 60.0f * 3.141592f / 180.0f;
	dNewtonUniversalActuator* const joint = new dNewtonUniversalActuator (&tireMatrix[0][0], angularRate, -angleLimit, angleLimit, angularRate, -angleLimit, angleLimit, tire, m_rootBody);

	// disable the limits of the first row, so that it can spin free
	joint->SetEnableFlag0 (false);
	return joint;
}


dNewtonHingeActuator* ForkliftPhysicsModel::LinkBasePlatform (OgreNewtonDynamicBody* const platform)  
{
	Matrix4 aligmentMatrix (Quaternion (Radian (3.141592f * 0.5f), Vector3 (0.0f, 1.0f, 0.0f)));
	Matrix4 baseMatrix((platform->GetMatrix() * aligmentMatrix).transpose());

	dFloat minAngleLimit = -30.0f * 3.141592f / 180.0f;
	dFloat maxAngleLimit =  20.0f * 3.141592f / 180.0f;
	dFloat angularRate = 10.0f * 3.141592f / 180.0f;
	return new dNewtonHingeActuator (&baseMatrix[0][0], angularRate, minAngleLimit, maxAngleLimit, platform, m_rootBody);
}

dNewtonSliderActuator* ForkliftPhysicsModel::LinkBasePlatform (OgreNewtonDynamicBody* const parent, OgreNewtonDynamicBody* const platform)
{
	Matrix4 aligmentMatrix (Quaternion (Radian (3.141592f * 0.5f), Vector3 (0.0f, 0.0f, 1.0f)));
	Matrix4 baseMatrix((platform->GetMatrix() * aligmentMatrix).transpose());
	return new dNewtonSliderActuator (&baseMatrix[0][0], 0.1f, -0.25f, 1.5f, platform, parent);
}


dNewtonSliderActuator* ForkliftPhysicsModel::LinkTooth(OgreNewtonDynamicBody* const parent, OgreNewtonDynamicBody* const child, Real dir)
{
	Matrix4 aligmentMatrix (Quaternion (Radian (dir * 3.141592f * 0.5f), Vector3 (0.0f, 1.0f, 0.0f)));
	Matrix4 baseMatrix((child->GetMatrix() * aligmentMatrix).transpose());
	return new dNewtonSliderActuator (&baseMatrix[0][0], 0.25f, -0.25f, 0.25f, child, parent);
}

void ForkliftPhysicsModel::CalculateEngine(OgreNewtonDynamicBody* const tire)
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
	m_rootBody->GetMassAndInertia (mass, Ixx, Iyy, Izz);

	const Vector3& gravity = m_application->GetPhysics()->GetGravity();

	Real radius = (p1.y - p0.y) * 0.5f;

	// calculate a torque the will produce a 0.5f of the force of gravity
	m_maxEngineTorque = 0.25f * mass * radius * gravity.length();

	// calculate the coefficient of drag for top speed of 20 m/s
	Real maxOmega = 200.0f / radius;
	m_omegaResistance = 1.0f / maxOmega;
}


void ForkliftPhysicsModel::ApplyInputs(const InputRecored& inputs)
{
	m_inputRecored = inputs;
}

void ForkliftPhysicsModel::OnPreUpdate (dFloat timestep)
{

	// apply steering control
	Real steeringAngle = m_rearTire[0]->GetActuatorAngle1();
	if (m_inputRecored.m_steering < 0) {
		steeringAngle = m_rearTire[0]->GetMinAngularLimit1(); 
	} else if (m_inputRecored.m_steering > 0) {
		steeringAngle = m_rearTire[0]->GetMaxAngularLimit1(); 
	}
	m_rearTire[0]->SetTargetAngle1(steeringAngle);
	m_rearTire[1]->SetTargetAngle1(steeringAngle);

	// apply engine torque
	Real brakeTorque = 0.0f;
	Real engineTorque = 0.0f;
	if (m_inputRecored.m_throtler > 0) {
		engineTorque = -m_maxEngineTorque; 
	} else if (m_inputRecored.m_throtler < 0) {
		engineTorque = m_maxEngineTorque; 
	} else {
		brakeTorque = 1000.0f;
	}
	m_frontTire[0]->SetFriction(brakeTorque);
	m_frontTire[1]->SetFriction(brakeTorque);

	Matrix4 matrix(m_rootBody->GetMatrix());
	matrix.setTrans(Vector3::ZERO);

	Vector3 tirePing (matrix * Vector3(1.0f, 0.0f, 0.0f));
	if (engineTorque != 0.0f) {
		Vector3 torque (tirePing * engineTorque);
		m_frontTireBody[0]->AddTorque (torque);
		m_frontTireBody[1]->AddTorque (torque);
	}

	for (int i = 0; i < 2; i ++) {
		Vector3 omega(m_frontTireBody[i]->GetOmega());
		Real omegaMag = omega.dotProduct(tirePing);
		Real sign = (omegaMag >= 0.0f) ? 1.0 : -1.0f;
		omega -= tirePing * (sign * omegaMag * omegaMag * m_omegaResistance);
		m_frontTireBody[i]->SetOmega(omega);
	}


	// control tilt angle
	Real tiltAngle = m_tiltAngle;
	if (m_inputRecored.m_tilt > 0) {
		tiltAngle = m_revolvePlatform->GetMinAngularLimit();
		m_tiltAngle = m_revolvePlatform->GetActuatorAngle();
	} else if (m_inputRecored.m_tilt < 0) {
		tiltAngle = m_revolvePlatform->GetMaxAngularLimit();
		m_tiltAngle = m_revolvePlatform->GetActuatorAngle();
	}
	m_revolvePlatform->SetTargetAngle (tiltAngle);


	// control lift position
	Real liftPosit= m_liftPosit;
	if (m_inputRecored.m_lift > 0) {
		liftPosit = m_slidePlaforms[0]->GetMinPositLimit();
		m_liftPosit = m_slidePlaforms[0]->GetActuatorPosit();
	} else if (m_inputRecored.m_lift < 0) {
		liftPosit = m_slidePlaforms[0]->GetMaxPositLimit();
		m_liftPosit = m_slidePlaforms[0]->GetActuatorPosit();
	}
	for (int i = 0; i < sizeof (m_slidePlaforms) / sizeof (m_slidePlaforms[0]); i ++) {
		m_slidePlaforms[i]->SetTargetPosit(liftPosit);
	}

	// control teeth position
	Real toothPosit = m_slideTooth[0]->GetActuatorPosit();
	if (m_inputRecored.m_palette > 0) {
		toothPosit = m_slideTooth[0]->GetMinPositLimit();
		m_openPosit = m_slideTooth[0]->GetActuatorPosit();
	} else if (m_inputRecored.m_palette < 0) {
		toothPosit = m_slideTooth[0]->GetMaxPositLimit();
		m_openPosit = m_slideTooth[0]->GetActuatorPosit();
	}
	for (int i = 0; i < sizeof (m_slideTooth) / sizeof (m_slideTooth[0]); i ++) {
		m_slideTooth[i]->SetTargetPosit(toothPosit);
	}
}
