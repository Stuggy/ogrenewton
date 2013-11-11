
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
#include "JointsScene.h"
#include "DemoApplication.h"


static OgreNewtonDynamicBody* CreateBox (SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& location, const Vector3& size)
{
	dNewtonCollisionBox shape (world, size.x, size.y, size.z, m_all);

	// create a texture for using with this material
	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().load("sand1b.jpg", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	// make a material to use with this mesh
	MaterialPtr renderMaterial = MaterialManager::getSingleton().create("ramp", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	renderMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(true);
	renderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("sand1b.jpg");
	renderMaterial->setAmbient(0.2f, 0.2f, 0.2f);

	OgreNewtonMesh boxMesh (&shape);
	boxMesh.Triangulate();
	int materialId = boxMesh.AddMaterial(renderMaterial);
	boxMesh.ApplyBoxMapping (materialId, materialId, materialId);

	// create a manual object for rendering 
	ManualObject* const object = boxMesh.CreateEntity(MakeName ("ramp"));
	MeshPtr mesh (object->convertToMesh (MakeName ("ramp")));

	//Matrix4 matrix (Matrix4::IDENTITY);
	Matrix4 matrix (Matrix4::IDENTITY);
	matrix.setTrans (Vector3 (location.x, location.y, location.z));

	Entity* const ent = sceneMgr->createEntity(MakeName ("ramp"), mesh);
	SceneNode* const node = CreateNode (sceneMgr, ent, matrix.getTrans(), matrix.extractQuaternion());
	OgreNewtonDynamicBody* const body = new OgreNewtonDynamicBody (world, 10.0f, &shape, node, matrix);

	delete object;
	return body;
}


static OgreNewtonDynamicBody* CreateCylinder (SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& location, dFloat radius, dFloat height)
{
	dNewtonCollisionCylinder shape (world, radius, height, m_all);

	// create a texture for using with this material
	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().load("sand1b.jpg", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	// make a material to use with this mesh
	MaterialPtr renderMaterial = MaterialManager::getSingleton().create("cylinder", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	renderMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(true);
	renderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("sand1b.jpg");
	renderMaterial->setAmbient(0.2f, 0.2f, 0.2f);

	OgreNewtonMesh boxMesh (&shape);
	boxMesh.Triangulate();
	int materialId = boxMesh.AddMaterial(renderMaterial);
	boxMesh.ApplyCylindricalMapping (materialId, materialId);

	// create a manual object for rendering 
	ManualObject* const object = boxMesh.CreateEntity(MakeName ("ramp"));
	MeshPtr mesh (object->convertToMesh (MakeName ("ramp")));

	//Matrix4 matrix (Matrix4::IDENTITY);
	Matrix4 matrix (Matrix4::IDENTITY);
	matrix.setTrans (Vector3 (location.x, location.y, location.z));

	Entity* const ent = sceneMgr->createEntity(MakeName ("ramp"), mesh);
	SceneNode* const node = CreateNode (sceneMgr, ent, matrix.getTrans(), matrix.extractQuaternion());
	OgreNewtonDynamicBody* const body = new OgreNewtonDynamicBody (world, 10.0f, &shape, node, matrix);

	delete object;
	return body;
}


static OgreNewtonDynamicBody* CreateWheel (SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& location, dFloat radius, dFloat height)
{
	dNewtonCollisionChamferedCylinder shape (world, radius, height, m_all);

	// create a texture for using with this material
	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().load("smilli.tga", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	// make a material to use with this mesh
	MaterialPtr renderMaterial = MaterialManager::getSingleton().create("wheel", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	renderMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(true);
	renderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("smilli.tga");
	renderMaterial->setAmbient(0.2f, 0.2f, 0.2f);

	OgreNewtonMesh boxMesh (&shape);
	boxMesh.Triangulate();
	int materialId = boxMesh.AddMaterial(renderMaterial);
	boxMesh.ApplyCylindricalMapping (materialId, materialId);

	// create a manual object for rendering 
	ManualObject* const object = boxMesh.CreateEntity(MakeName ("ramp"));
	MeshPtr mesh (object->convertToMesh (MakeName ("ramp")));

	//Matrix4 matrix (Matrix4::IDENTITY);
	Matrix4 matrix (Matrix4::IDENTITY);
	matrix.setTrans (Vector3 (location.x, location.y, location.z));

	Entity* const ent = sceneMgr->createEntity(MakeName ("ramp"), mesh);
	SceneNode* const node = CreateNode (sceneMgr, ent, matrix.getTrans(), matrix.extractQuaternion());
	OgreNewtonDynamicBody* const body = new OgreNewtonDynamicBody (world, 10.0f, &shape, node, matrix);

	delete object;
	return body;
}

static dNewtonSliderJoint* AddSliderWheel (SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& origin, dFloat radius, dFloat height, OgreNewtonDynamicBody* const parent)
{
	OgreNewtonDynamicBody* const wheel = CreateWheel (sceneMgr, world, origin, height, radius);

	// the joint pin is the first row of the matrix
	Matrix4 localPin (Quaternion (Degree(0.0f), Vector3 (0.0f, 0.0f, 1.0f)));

	// connect first box to the world
	Matrix4 matrix (wheel->GetMatrix() * localPin);
	return new dNewtonSliderJoint (&matrix.transpose()[0][0], wheel, parent);
}

static dNewtonHingeJoint* AddHingeWheel (SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& origin, dFloat radius, dFloat height, OgreNewtonDynamicBody* const parent)
{
    OgreNewtonDynamicBody* const wheel = CreateWheel (sceneMgr, world, origin, height, radius);

    // the joint pin is the first row of the matrix
	Matrix4 localPin (Quaternion (Degree(0.0f), Vector3 (0.0f, 0.0f, 1.0f)));

    // connect first box to the world
    Matrix4 matrix (wheel->GetMatrix() * localPin);
    return new dNewtonHingeJoint (&matrix.transpose()[0][0], wheel, parent);
}

static dNewtonCylindricalJoint* AddCylindricalWheel (SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& origin, dFloat radius, dFloat height, OgreNewtonDynamicBody* const parent)
{
    OgreNewtonDynamicBody* const wheel = CreateWheel (sceneMgr, world, origin, height, radius);

    // the joint pin is the first row of the matrix
	Matrix4 localPin (Quaternion (Degree(0.0f), Vector3 (0.0f, 0.0f, 1.0f)));

    // connect first box to the world
    Matrix4 matrix (localPin * wheel->GetMatrix());
    return new dNewtonCylindricalJoint (&matrix.transpose()[0][0], wheel, parent);
}

void AddBallAndSockect (SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& origin)
{
	Vector3 size (1.0f, 1.0f, 1.0f);
	OgreNewtonDynamicBody* const box0 = CreateBox (sceneMgr, world, origin + Vector3 (0.0f, 5.0f, 0.0f), size);
	OgreNewtonDynamicBody* const box1 = CreateBox (sceneMgr, world, origin + Vector3 (size.x, 5.0 - size.y, size.z), size);

	// connect first box to the world
	Matrix4 matrix (box0->GetMatrix());
	matrix.setTrans (matrix.getTrans() + Vector3 (-size.x * 0.5f, size.y * 0.5f, -size.z * 0.5f));
	new dNewtonBallAndSocketJoint (&matrix.transpose()[0][0], box0);

	// link the two boxes
	matrix = box1->GetMatrix();
	matrix.setTrans (matrix.getTrans() + Vector3 (-size.x * 0.5f, size.y * 0.5f, -size.z * 0.5f));
	new dNewtonBallAndSocketJoint (&matrix.transpose()[0][0], box0, box1);
}

void AddCylindrical (SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& origin)
{
	// make a reel static
	OgreNewtonDynamicBody* const reel = CreateCylinder(sceneMgr, world, origin + Vector3 (0.0f, 4.0f, 0.0f), 0.25f, 6.0f);
	reel->SetMassAndInertia (0.0f, 0.0f, 0.0f, 0.0f);

	OgreNewtonDynamicBody* const wheel = CreateWheel (sceneMgr, world, origin + Vector3 (0.0f, 4.0f, 0.0f), 1.0f, 0.5f);

	Matrix4 matrix (wheel->GetMatrix());
	dNewtonCylindricalJoint* const slider = new dNewtonCylindricalJoint (&matrix.transpose()[0][0], wheel, reel);

	// enable limit of first axis
	slider->EnableLimit_0(true);

	// set limit on second axis
	slider->SetLimis_0 (-3.0f, 3.0f);
}

void AddHinge (SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& origin)
{
	Vector3 size (1.5f, 4.0f, 0.125f);
	OgreNewtonDynamicBody* const box0 = CreateBox (sceneMgr, world, origin + Vector3 (0.0f, 4.0f, 0.0f), size);
	OgreNewtonDynamicBody* const box1 = CreateBox (sceneMgr, world, origin + Vector3 (1.5f, 4.0f, 0.0f), size);
	OgreNewtonDynamicBody* const box2 = CreateBox (sceneMgr, world, origin + Vector3 (3.0f, 4.0f, 0.0f), size);

	// the joint pin is the first row of the matrix, to make a upright pin we
	// take the x axis and rotate by 90 degree around the y axis
	Matrix4 localPin (Quaternion (Degree(90.0f), Vector3 (0.0f, 0.0f, 1.0f)));

	// connect first box to the world
	Matrix4 matrix (box0->GetMatrix() * localPin);

	matrix.setTrans (matrix.getTrans() + Vector3 (-size.x * 0.5f, 0.0f, 0.0f));
	dNewtonHingeJoint* const hinge0 = new dNewtonHingeJoint (&matrix.transpose()[0][0], box0);
	hinge0->EnableLimits (true);
	hinge0->SetLimis(-45.0f * 3.141592f / 180.0f, 45.0f * 3.141592f / 180.0f);

	// link the two boxes
	matrix = box1->GetMatrix() * localPin;
	matrix.setTrans (matrix.getTrans() + Vector3 (-size.x * 0.5f, 0.0f, 0.0f));
	dNewtonHingeJoint* const hinge1 = new dNewtonHingeJoint (&matrix.transpose()[0][0], box0, box1);
	hinge1->EnableLimits (true);
	hinge1->SetLimis (-45.0f * 3.141592f / 180.0f, 45.0f * 3.141592f / 180.0f);

	// link the two boxes
	matrix = box2->GetMatrix() * localPin;
	matrix.setTrans (matrix.getTrans() + Vector3 (-size.x * 0.5f, 0.0f, 0.0f));
	dNewtonHingeJoint* const hinge2 = new dNewtonHingeJoint (&matrix.transpose()[0][0], box1, box2);
	hinge2->EnableLimits (true);
	hinge2->SetLimis (-45.0f * 3.141592f / 180.0f, 45.0f * 3.141592f / 180.0f);
}


void AddUniversal (SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& origin)
{
	OgreNewtonDynamicBody* const wheel = CreateWheel (sceneMgr, world, origin + Vector3 (0.0f, 4.0f, 0.0f), 1.0f, 0.5f);

	Matrix4 localPin (Quaternion (Degree(0.0f), Vector3 (0.0f, 0.0f, 1.0f)));
	Matrix4 matrix (wheel->GetMatrix() * localPin);
	dNewtonUniversalJoint* const universal = new dNewtonUniversalJoint (&matrix.transpose()[0][0], wheel);

	// disable limit of first axis
	universal->EnableLimit_0(false);

	// set limit on second axis
	universal->SetLimis_1 (-500.0f * 3.141592f / 180.0f, 500.0f * 3.141592f / 180.0f);
}


void AddSlider (SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& origin)
{
	// make a reel static
	OgreNewtonDynamicBody* const reel = CreateBox (sceneMgr, world, origin + Vector3 (0.0f, 4.0f, 0.0f), Vector3 (8.0f, 0.25f, 0.25f));
	reel->SetMassAndInertia (0.0f, 0.0f, 0.0f, 0.0f);

	OgreNewtonDynamicBody* const wheel = CreateWheel (sceneMgr, world, origin + Vector3 (0.0f, 4.0f, 0.0f), 1.0f, 0.5f);

	Matrix4 matrix (wheel->GetMatrix());
	dNewtonSliderJoint* const slider = new dNewtonSliderJoint (&matrix.transpose()[0][0], wheel, reel);

	// enable limit of first axis
	slider->EnableLimits(true);

	// set limit on second axis
	slider->SetLimis (-4.0f, 4.0f);
}

void AddGear (SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& origin)
{
	OgreNewtonDynamicBody* const reel = CreateCylinder(sceneMgr, world, origin + Vector3 (0.0f, 4.0f, 0.0f), 0.25f, 4.0f);
	reel->SetMassAndInertia (0.0f, 0.0f, 0.0f, 0.0f);

	dNewtonHingeJoint* const hinge0 = AddHingeWheel (sceneMgr, world, origin + Vector3 (-1.0f, 4.0f, 0.0f), 0.5f, 1.0f, reel);
	dNewtonHingeJoint* const hinge1 = AddHingeWheel (sceneMgr, world, origin + Vector3 ( 1.0f, 4.0f, 0.0f), 0.5f, 1.0f, reel);

	OgreNewtonDynamicBody* const body0 = (OgreNewtonDynamicBody*)hinge0->GetBody0();
	OgreNewtonDynamicBody* const body1 = (OgreNewtonDynamicBody*)hinge1->GetBody0();

	Matrix4 matrix0 (body0->GetMatrix());
	Matrix4 matrix1 (body1->GetMatrix());
	matrix0.setTrans (Vector3 (0.0f, 0.0f, 0.0f));
	matrix1.setTrans (Vector3 (0.0f, 0.0f, 0.0f));

	Vector3 pin0 (matrix0.transformAffine(Vector3 (1.0f, 0.0f, 0.0f)));
	Vector3 pin1 (matrix1.transformAffine(Vector3 (1.0f, 0.0f, 0.0f)));
	new dNewtonGearJoint (4.0f, &pin0.x, body0, &pin1.x, body1);
}

void AddPulley (SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& origin)
{
	OgreNewtonDynamicBody* const reel0 = CreateBox(sceneMgr, world, origin + Vector3 (0.0f, 4.0f, 3.0f), Vector3(4.0f, 0.25f, 0.25f));
	OgreNewtonDynamicBody* const reel1 = CreateBox(sceneMgr, world, origin + Vector3 (0.0f, 4.0f, 0.0f), Vector3(4.0f, 0.25f, 0.25f));
	reel0->SetMassAndInertia (0.0f, 0.0f, 0.0f, 0.0f);
	reel1->SetMassAndInertia (0.0f, 0.0f, 0.0f, 0.0f);

	dNewtonSliderJoint* const slider0 = AddSliderWheel (sceneMgr, world, origin + Vector3 (0.0f, 4.0f, 3.0f), 0.5f, 1.0f, reel0);
	dNewtonSliderJoint* const slider1 = AddSliderWheel (sceneMgr, world, origin + Vector3 (0.0f, 4.0f, 0.0f), 0.5f, 0.5f, reel1);
	slider0->EnableLimits(true);
	slider0->SetLimis (-2.0f, 2.0f);

	OgreNewtonDynamicBody* const body0 = (OgreNewtonDynamicBody*)slider0->GetBody0();
	OgreNewtonDynamicBody* const body1 = (OgreNewtonDynamicBody*)slider1->GetBody0();

	Matrix4 matrix0 (body0->GetMatrix());
	Matrix4 matrix1 (body1->GetMatrix());
	matrix0.setTrans (Vector3 (0.0f, 0.0f, 0.0f));
	matrix1.setTrans (Vector3 (0.0f, 0.0f, 0.0f));

	Vector3 pin0 (matrix0.transformAffine(Vector3( 1.0f, 0.0f, 0.0f)));
	Vector3 pin1 (matrix1.transformAffine(Vector3( 1.0f, 0.0f, 0.0f)));
	new dNewtonPulleyJoint (1.0f, &pin0.x, body0, &pin1.x, body1);
}


void AddGearAndRack (SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& origin)
{
	OgreNewtonDynamicBody* const reel0 = CreateCylinder(sceneMgr, world, origin + Vector3 (0.0f, 4.0f, 0.0f), 0.25f, 4.0f);
	OgreNewtonDynamicBody* const reel1 = CreateBox(sceneMgr, world, origin + Vector3 (0.0f, 4.0f, 3.0f), Vector3(4.0f, 0.25f, 0.25f));
	reel0->SetMassAndInertia (0.0f, 0.0f, 0.0f, 0.0f);
	reel1->SetMassAndInertia (0.0f, 0.0f, 0.0f, 0.0f);

	dNewtonHingeJoint* const hinge0 = AddHingeWheel (sceneMgr, world, origin + Vector3 (-1.0f, 4.0f, 0.0f), 0.5f, 0.5f, reel0);
	dNewtonHingeJoint* const hinge1 = AddHingeWheel (sceneMgr, world, origin + Vector3 ( 1.0f, 4.0f, 0.0f), 0.5f, 0.5f, reel0);
	dNewtonCylindricalJoint* const cylinder = AddCylindricalWheel(sceneMgr, world, origin + Vector3 (0.0f, 4.0f, 3.0f), 0.5f, 1.0f, reel1);
	cylinder->EnableLimit_0(true);
	cylinder->SetLimis_0(-2.0f, 2.0f);

	OgreNewtonDynamicBody* const body0 = (OgreNewtonDynamicBody*)hinge0->GetBody0();
	OgreNewtonDynamicBody* const body1 = (OgreNewtonDynamicBody*)hinge1->GetBody0();
	OgreNewtonDynamicBody* const body2 = (OgreNewtonDynamicBody*)cylinder->GetBody0();

	Matrix4 matrix0 (body0->GetMatrix());
	Matrix4 matrix1 (body1->GetMatrix());
	Matrix4 matrix2 (body2->GetMatrix());
	matrix0.setTrans (Vector3 (0.0f, 0.0f, 0.0f));
	matrix1.setTrans (Vector3 (0.0f, 0.0f, 0.0f));
	matrix2.setTrans (Vector3 (0.0f, 0.0f, 0.0f));

	Vector3 pin0 (matrix0.transformAffine(Vector3( 1.0f, 0.0f, 0.0f)));
	Vector3 pin1 (matrix1.transformAffine(Vector3( 1.0f, 0.0f, 0.0f)));
	Vector3 pin2 (matrix2.transformAffine(Vector3( 1.0f, 0.0f, 0.0f)));
	new dNewtonGearJoint (1.0f, &pin0.x, body0, &pin2.x, body2);
	new dNewtonGearAndRackJoint (1.0f, &pin1.x, body1, &pin2.x, body2);
}
