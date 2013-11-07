
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
#include "MaterialScene.h"
#include "DemoApplication.h"

static void MakeStaticRamp(SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& location, int rampMaterialID)
{
	Vector3 blockBoxSize (20.0f, 0.25f, 40.0f);
	dNewtonCollisionBox shape (world, blockBoxSize.x, blockBoxSize.y, blockBoxSize.z, m_all);

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
	Matrix4 matrix (Quaternion (Degree(-30.0f), Vector3 (0.0f, 0.0f, 1.0f)));
	matrix.setTrans (Vector3 (location.x, location.y, location.z));

	Entity* const ent = sceneMgr->createEntity(MakeName ("ramp"), mesh);
	SceneNode* const node = CreateNode (sceneMgr, ent, matrix.getTrans(), matrix.extractQuaternion());

	shape.SetMaterialId (rampMaterialID);
	new OgreNewtonDynamicBody (world, 0.0f, &shape, node, matrix);

	delete object;
}

static void AddFrictionSamples(SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& location, int materialStartID)
{
	Vector3 blockBoxSize (0.75f, 0.25f, 0.5f);
	dNewtonCollisionBox shape (world, blockBoxSize.x, blockBoxSize.y, blockBoxSize.z, m_all);

	// create a texture for using with this material
	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().load("smilli.tga", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	// make a material to use with this mesh
	MaterialPtr renderMaterial = MaterialManager::getSingleton().create("smalli", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	renderMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(true);
	renderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("smilli.tga");
	renderMaterial->setAmbient(0.2f, 0.2f, 0.2f);

	OgreNewtonMesh boxMesh (&shape);
	boxMesh.Triangulate();
	int materialId = boxMesh.AddMaterial(renderMaterial);
	boxMesh.ApplyBoxMapping (materialId, materialId, materialId);

	// create a manual object for rendering 
	ManualObject* const object = boxMesh.CreateEntity(MakeName ("ramp"));
	MeshPtr mesh (object->convertToMesh (MakeName ("ramp")));

	//Matrix4 matrix (Matrix4::IDENTITY);
	Matrix4 matrix (Quaternion (Degree(-30.0f), Vector3 (0.0f, 0.0f, 1.0f)));
	Vector3 origin (location.x - 8.0f, location.y + 5.125f, location.z + 15.0f);

	dFloat mass = 10.0f;
	for (int i = 0; i < 10; i ++) {
		matrix.setTrans (origin);
		origin.z -= 3.0f;
		Entity* const ent = sceneMgr->createEntity(MakeName ("ramp"), mesh);
		SceneNode* const node = CreateNode (sceneMgr, ent, matrix.getTrans(), matrix.extractQuaternion());

		shape.SetMaterialId(materialStartID + i);
		OgreNewtonDynamicBody* const body = new OgreNewtonDynamicBody (world, mass, &shape, node, matrix);

		// set the linear and angular drag do zero
		body->SetLinearDrag (0.0f);
		body->SetAngularDrag(Vector3 (0.0f, 0.0f, 0.0f));
	}

	delete object;
}


static void AddRestitutionSamples(SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& location, int materialStartID)
{
//	Vector3 blockBoxSize (0.5f, 0.5f, 0.5f);
	dNewtonCollisionSphere shape (world, 0.5f, m_all);

	// create a texture for using with this material
	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().load("smilli.tga", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	// make a material to use with this mesh
	MaterialPtr renderMaterial = MaterialManager::getSingleton().create("smalli", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	renderMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(true);
	renderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("smilli.tga");
	renderMaterial->setAmbient(0.2f, 0.2f, 0.2f);

	OgreNewtonMesh sphMesh (&shape);
	sphMesh.Triangulate();
	int materialId = sphMesh.AddMaterial(renderMaterial);
	sphMesh.ApplyBoxMapping (materialId, materialId, materialId);

	// create a manual object for rendering 
	ManualObject* const object = sphMesh.CreateEntity(MakeName ("ramp"));
	MeshPtr mesh (object->convertToMesh (MakeName ("ramp")));

	Matrix4 matrix (Matrix4::IDENTITY);
	Vector3 origin (location.x + 5.0f, location.y + 10.0f, location.z - 15.0f);

	dFloat mass = 10.0f;
	for (int i = 0; i < 10; i ++) {
		matrix.setTrans (origin);
		origin.x += 2.0f;
		Entity* const ent = sceneMgr->createEntity(MakeName ("ramp"), mesh);
		SceneNode* const node = CreateNode (sceneMgr, ent, matrix.getTrans(), matrix.extractQuaternion());

		shape.SetMaterialId(materialStartID + i);
		OgreNewtonDynamicBody* const body = new OgreNewtonDynamicBody (world, mass, &shape, node, matrix);

		// set the linear and angular drag do zero
		body->SetLinearDrag (0.0f);
		body->SetAngularDrag(Vector3 (0.0f, 0.0f, 0.0f));
	}

	delete object;
}



void BuildMaterialScene (SceneManager* const sceneMgr, OgreNewtonWorld* const world, const Vector3& location, Real mass)
{
	int rampMaterialId = 0;
	int frictionMaterialIDStart = rampMaterialId + 1;
	int restitutionMaterialIDStart = frictionMaterialIDStart + 10;

	// create 10 diffErent friCtion material 
	dFloat friction = 0.0f;
	for (int i = 0; i < 10; i ++) {
		dMaterialPairManager::dMaterialPair materialInterAction;
		materialInterAction.m_staticFriction0 = friction;
		materialInterAction.m_staticFriction1 = friction;
		materialInterAction.m_kineticFriction0 = friction;
		materialInterAction.m_kineticFriction1 = friction;
		world->AddMaterialPair (frictionMaterialIDStart + i, rampMaterialId, materialInterAction);
		friction += 0.065f;
	}

	// create 10 restitution materials
	dFloat restitution = 0.1f;
	for (int i = 0; i < 10; i ++) {
		dMaterialPairManager::dMaterialPair materialInterAction;
		materialInterAction.m_restitution = restitution;
		world->AddMaterialPair (restitutionMaterialIDStart + i, rampMaterialId, materialInterAction);
		restitution += 0.1f;
	}

	MakeStaticRamp(sceneMgr, world, location, rampMaterialId);
	AddFrictionSamples(sceneMgr, world, location, frictionMaterialIDStart);
	AddRestitutionSamples(sceneMgr, world, location, restitutionMaterialIDStart);
}	



