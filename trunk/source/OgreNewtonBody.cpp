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
#include "OgreNewtonBody.h"
#include "OgreNewtonWorld.h"


OgreNewtonBody::OgreNewtonBody (OgreNewtonWorld* const world, Real mass, const dNewtonCollision* const collision, SceneNode* const treeNode, const Matrix4& location)
	:dNewtonBody (world, mass, collision, treeNode, location.transpose()[0])
{
}

OgreNewtonBody::~OgreNewtonBody()
{
}


void OgreNewtonBody::OnForceAndTorque (dFloat timestep, int threadIndex)
{
	dFloat mass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;	

	const OgreNewtonWorld* const world = (OgreNewtonWorld*) GetNewton();
	GetMassAndInertia (mass, Ixx, Iyy, Izz);

	Vector3 force (world->GetGravity() * mass);
	SetForce (&force.x);
}


OgreNewtonBody* OgreNewtonBody::CreateBox(OgreNewtonWorld* const world, SceneNode* const sourceNode, Real mass, const Matrix4& matrix)
{

	MovableObject* const attachedObject = sourceNode->getAttachedObject(0);

	Vector3 position       = sourceNode->getPosition();
	Vector3 size           = Vector3::UNIT_SCALE;
	Vector3 centerOffset   = Vector3::ZERO;
	Vector3 scale          = sourceNode->_getDerivedScale();
	Vector3 dimensions     = scale;
	Quaternion orientation = sourceNode->_getDerivedOrientation();

	AxisAlignedBox boundingBox = attachedObject->getBoundingBox();
	Vector3 padding = (boundingBox.getMaximum() - boundingBox.getMinimum()) * MeshManager::getSingleton().getBoundsPaddingFactor() * 2;

	size = (boundingBox.getMaximum() - boundingBox.getMinimum()) - padding * 2;
	centerOffset = boundingBox.getMinimum() + padding + (size / 2.0f);
//		dimensions = sourceNode->_getDerivedScale() * size;

	Matrix4 modifierMatrix;
	modifierMatrix.makeTransform(centerOffset * scale, Vector3(1.0f, 1.0f, 1.0f), Quaternion::IDENTITY);
	modifierMatrix = modifierMatrix.transpose();

	dNewtonCollisionBox boxShape (world, size.x, size.y, size.z, 0);
	boxShape.SetScale(scale.x, scale.y, scale.z);

//		modifiedCollision->setScalarMatrix(modifierMatrix);
	boxShape.SetMatrix (modifierMatrix[0]);

	OgreNewtonBody* const body = new OgreNewtonBody (world, mass, &boxShape, sourceNode, matrix);
/*
	if (materialId != NULL)
	{
		body->setMaterialGroupID(materialId);
	}

	Vector3 inertia, offset;
	modifiedCollision->calculateInertialMatrix(inertia, offset);

	body->setMassMatrix(mass, inertia * mass);
	body->setCenterOfMass(offset);
	body->setStandardForceCallback();
	body->attachNode(sourceNode);
	body->setPositionOrientation(position, orientation);

	boxes.insert(std::make_pair(sourceNode->getName(), body));
*/
	return body;
}

