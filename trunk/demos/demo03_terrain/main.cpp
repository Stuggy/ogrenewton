
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
#include <OgreNewtonBody.h>
#include <OgreNewtonWorld.h>
#include <OgreNewtonRayCast.h>
#include <OgreNewtonDebugger.h>
#include <OgreNewtonSceneBody.h>
#include <OgreNewtonRayPickManager.h>
#include <OgreNewtonExampleApplication.h>


#include "Utils.h"
#include "BuildJenga.h"
#include "BuildPyramid.h"
#include "DemoApplication.h"


using namespace Ogre;

class OgreNewtonDemoApplication: public DemoApplication
{
	public:

	OgreNewtonDemoApplication()
		:DemoApplication()
		,m_shootingTimer(0.25f)
		,m_terrainGroup(NULL)
		,m_terrainGlobals(NULL)
		,m_terrainsImported(false)
	{
	}

	virtual ~OgreNewtonDemoApplication()
	{
	}

	protected:
	void loadStaticScene ()
	{

		// create a scene body to add all static collidable meshes in the world 
		OgreNewtonSceneBody* const sceneBody = new OgreNewtonSceneBody (m_physicsWorld);

		// start adding collision shape to the scene body
		sceneBody->BeginAddRemoveCollision();

		// load a visual ogre terrain
		LoadVisualTerrain();

		// iterate over all terrain tile and add a collision for each one
		TerrainGroup::TerrainIterator ti = m_terrainGroup->getTerrainIterator();
		while(ti.hasMoreElements())	{
			Terrain* const terrain = ti.getNext()->instance;
			sceneBody->AddTerrain (terrain);
		}

		// done adding collision shape to the scene body, now optimize the scene
		sceneBody->EndAddRemoveCollision();
	}

	void getTerrainImage(bool flipX, bool flipY, Image& img)
	{
		img.load("terrain.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		if (flipX) {
			img.flipAroundY();
		}
		if (flipY) {
			img.flipAroundX();
		}
	}


	void defineTerrain(long x, long y)
	{
		String filename = m_terrainGroup->generateFilename(x, y);
		if (ResourceGroupManager::getSingleton().resourceExists(m_terrainGroup->getResourceGroup(), filename)) {
			m_terrainGroup->defineTerrain(x, y);
		} else {
			Image img;
			getTerrainImage(x % 2 != 0, y % 2 != 0, img);
			m_terrainGroup->defineTerrain(x, y, &img);
			m_terrainsImported = true;
		}
	}


	void configureTerrainDefaults ()
	{
		// Configure global
		m_terrainGlobals->setMaxPixelError(8);
		// testing composite map
		m_terrainGlobals->setCompositeMapDistance(3000);

		// Important to set these so that the terrain knows what to use for derived (non-realtime) data
		m_terrainGlobals->setLightMapDirection(m_light0->getDerivedDirection());
		m_terrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
		m_terrainGlobals->setCompositeMapDiffuse(m_light0->getDiffuseColour());

		// Configure default import settings for if we use imported image
		Terrain::ImportData& defaultimp = m_terrainGroup->getDefaultImportSettings();
		defaultimp.terrainSize = 513;
		defaultimp.worldSize = 12000.0f;
		defaultimp.inputScale = 600;
		defaultimp.minBatchSize = 33;
		defaultimp.maxBatchSize = 65;
		// textures
		defaultimp.layerList.resize(3);
		defaultimp.layerList[0].worldSize = 100;
		defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
		defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
		defaultimp.layerList[1].worldSize = 30;
		defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
		defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
		defaultimp.layerList[2].worldSize = 200;
		defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
		defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
	}


	void initBlendMaps(Terrain* const terrain)
	{
		TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
		TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
		Real minHeight0 = 70;
		Real fadeDist0 = 40;
		Real minHeight1 = 70;
		Real fadeDist1 = 15;
		float* pBlend0 = blendMap0->getBlendPointer();
		float* pBlend1 = blendMap1->getBlendPointer();
		for (uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y) {
			for (uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x) {
				Real tx, ty;

				blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
				Real height = terrain->getHeightAtTerrainPosition(tx, ty);
				Real val = (height - minHeight0) / fadeDist0;
				val = Math::Clamp(val, (Real)0, (Real)1);
				*pBlend0++ = val;

				val = (height - minHeight1) / fadeDist1;
				val = Math::Clamp(val, (Real)0, (Real)1);
				*pBlend1++ = val;
			}
		}
		blendMap0->dirty();
		blendMap1->dirty();
		blendMap0->update();
		blendMap1->update();
	}

	void LoadVisualTerrain()
	{
		m_terrainGlobals = OGRE_NEW TerrainGlobalOptions();
	 	m_terrainGroup = OGRE_NEW TerrainGroup(mSceneMgr, Terrain::ALIGN_X_Z, 513, 12000.0f);

		//m_terrainGroup->setFilenameConvention(String("BasicTutorial3Terrain"), String("dat"));
		m_terrainGroup->setFilenameConvention(String("testTerrain"), String("dat"));
		m_terrainGroup->setOrigin(Vector3::ZERO);
	 
		//configureTerrainDefaults(light);
		configureTerrainDefaults();
	 
		for (long x = 0; x < 1; ++x) {
			for (long y = 0; y < 1; ++y) {
				defineTerrain(x, y);
			}
		}
	 
		// sync load since we want everything in place when we start
		m_terrainGroup->loadAllTerrains(true);
	 
		if (m_terrainsImported) {
			TerrainGroup::TerrainIterator ti = m_terrainGroup->getTerrainIterator();
			while(ti.hasMoreElements())	{
				Terrain* t = ti.getNext()->instance;
				initBlendMaps(t);
			}
		}
	 
		m_terrainGroup->freeTemporaryResources();
	}

	void LoadDynamicScene(const Vector3& origin)
	{
	}

	void createFrameListener()
	{
		// this is our custom frame listener for this app, that lets us shoot cylinders with the space bar, move the camera, etc.
//		m_listener = new ApplicationFrameListener (mRoot, mWindow, mCamera, mSceneMgr, this, m_debugRender);
//		mRoot->addFrameListener(m_listener);
	}

	void OnPhysicUpdateBegin(dFloat timestepInSecunds)
	{
		DemoApplication::OnPhysicUpdateBegin(timestepInSecunds);

		m_shootingTimer -= timestepInSecunds;
		if ((m_shootingTimer < 0.0f) && m_keyboard->isKeyDown(OIS::KC_SPACE)) {
			m_shootingTimer = 0.1f;

			Vector3 cameraPosit;
			Quaternion cameraRotation;
			GetInterpolatedCameraMatrix (cameraPosit, cameraRotation);

			int index = (rand() >> 3) % int (sizeof (m_shootingMesh) / sizeof (m_shootingMesh[0]));
			//index = 0;

   		 	Entity* const ent = mSceneMgr->createEntity(MakeName ("shootObject"), m_shootingMesh[index]);
			SceneNode* const node = CreateNode (mSceneMgr, ent, cameraPosit, cameraRotation);
			Matrix4 matrix;
			matrix.makeTransform (cameraPosit, Vector3(1.0f, 1.0f, 1.0f), cameraRotation);
			OgreNewtonBody* const body = new OgreNewtonBody (m_physicsWorld, 30.0f, m_shootingCollisions[index], node, matrix);

			const Real speed = -40.0f;
			Vector3 veloc (Vector3 (matrix[0][2], matrix[1][2], matrix[2][2]) * speed);   
			body->SetVeloc(veloc);
		}
			
	}

	virtual void destroyScene()
	{
	    OGRE_DELETE m_terrainGroup;
		OGRE_DELETE m_terrainGlobals;

		for (int i = 0; i < int (sizeof (m_shootingCollisions) / sizeof (m_shootingCollisions[0])); i ++) {
			delete m_shootingCollisions[i];
		}
	}

	void createScene()
	{
		// create the physic world first
		DemoApplication::createScene();

		//make a light
		mSceneMgr->setAmbientLight(ColourValue(0.2f, 0.2f, 0.2f));

		m_light0 = mSceneMgr->createLight( "Light0" );
		
		//m_light0->setType (Light::LT_POINT );
		//m_light0->setPosition (Vector3(-100.0f, 100.0f, -100.0f) );

		Vector3 lightdir(0.55f, -0.3f, -0.75f);
		m_light0->setType(Light::LT_DIRECTIONAL);
		m_light0->setDirection(lightdir);
		m_light0->setDiffuseColour(ColourValue(1.0f, 1.0f, 1.0f));
		m_light0->setSpecularColour(ColourValue(0.4f, 0.4f, 0.4f));
		
		// sky box.
		//mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");
		mSceneMgr->setSkyBox(true, "Examples/MorningSkyBox");

		// load all of the static geometry
		loadStaticScene ();

		// position camera using the ray cast functionality
		Vector3 start (0.0f, 1000.0f, 10.0f);
		Vector3 end (0.0f, -1000.0f, 10.0f);
		OgreNewtonRayCast raycaster(m_physicsWorld); 
		raycaster.CastRay (&start.x, &end.x);

		Vector3 origin (raycaster.m_contact + Vector3 (0.0f, 2.0f, 0.0f));
		mCamera->setPosition(origin);

		// set the near and far clip plane
		mCamera->setNearClipDistance(0.1f);
		mCamera->setFarClipDistance(10000.0f);

		//mCamera->setPolygonMode(Ogre::PM_WIREFRAME);

		// now load the dynamics Scene
		LoadDynamicScene(origin);

		// create shutting components
		CreateComponentsForShutting(sizeof (m_shootingCollisions)/sizeof (m_shootingCollisions[0]), m_physicsWorld, m_shootingCollisions, m_shootingMesh);

		// initialize the Camera position after the scene was loaded
		ResetCamera (mCamera->getPosition(), mCamera->getOrientation());



/*
{
Vector3 cameraPosit;
Quaternion cameraRotation;
GetInterpolatedCameraMatrix (cameraPosit, cameraRotation);

int index = (rand() >> 3) % int (sizeof (m_shootingMesh) / sizeof (m_shootingMesh[0]));
index = 0;

Entity* const ent = mSceneMgr->createEntity(MakeName ("shootObject"), m_shootingMesh[index]);
SceneNode* const node = CreateNode (mSceneMgr, ent, cameraPosit, cameraRotation);
Matrix4 matrix;
matrix.makeTransform (cameraPosit, Vector3(1.0f, 1.0f, 1.0f), cameraRotation);
OgreNewtonBody* const body = new OgreNewtonBody (m_physicsWorld, 30.0f, m_shootingCollisions[index], node, matrix);

const Real speed = -40.0f;
Vector3 veloc (Vector3 (matrix[0][2], matrix[1][2], matrix[2][2]) * speed);   
body->SetVeloc(veloc);
}
*/


	}

	Real m_shootingTimer;
	MeshPtr m_shootingMesh[2];
	dNewtonCollision* m_shootingCollisions[2];

	Light* m_light0;
	TerrainGroup* m_terrainGroup;
	TerrainGlobalOptions* m_terrainGlobals;
	bool m_terrainsImported;
};



INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
	OgreNewtonDemoApplication application;

	try {
		application.go();
	} catch(Exception &e) {
		MessageBox(NULL, e.getFullDescription().c_str(), "Well, this is embarrassing.. an Ogre exception has occurred.", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}
	return 0;
}




 
 
