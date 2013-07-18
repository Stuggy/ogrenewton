
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
#include <OgreNewtonPlayerManager.h>
#include <OgreNewtonRayPickManager.h>
#include <OgreNewtonExampleApplication.h>


#include "Utils.h"
#include "BuildJenga.h"
#include "BuildPyramid.h"
#include "DemoApplication.h"


using namespace Ogre;

static String animNames[] = {"IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed", "DrawSwords",  "SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"};
#define NUM_ANIMS int (sizeof (animNames)/ sizeof (animNames[0]))

class OgreNewtonDemoApplication: public DemoApplication
{
	public:

	// subclass the basic player 
	class MyPlayerContyroller: public OgreNewtonPlayerManager::OgreNetwonPlayer
	{
		public:
		enum AnimID
		{
			ANIM_IDLE_BASE,
			ANIM_IDLE_TOP,
			ANIM_RUN_BASE,
			ANIM_RUN_TOP,
			ANIM_HANDS_CLOSED,
			ANIM_HANDS_RELAXED,
			ANIM_DRAW_SWORDS,
			ANIM_SLICE_VERTICAL,
			ANIM_SLICE_HORIZONTAL,
			ANIM_DANCE,
			ANIM_JUMP_START,
			ANIM_JUMP_LOOP,
			ANIM_JUMP_END,
			ANIM_NONE
		};

		MyPlayerContyroller (OgreNewtonPlayerManager* const manager, Entity* const playerMesh, SceneNode* const node, dFloat mass, dFloat outerRadius, dFloat innerRadius, dFloat height, dFloat stairStep)
			:OgreNewtonPlayerManager::OgreNetwonPlayer (manager, node, mass, outerRadius, innerRadius, height, stairStep)
		{
			// load player animations
			playerMesh->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);

			// populate our animation list
			for (int i = 0; i < (sizeof animNames / sizeof (animNames[0])); i++) {
				mAnims[i] = playerMesh->getAnimationState(animNames[i]);
				mAnims[i]->setLoop(true);
				//mFadingIn[i] = false;
				//mFadingOut[i] = false;
			}

			// start off in the idle state (top and bottom together)
			setBaseAnimation(ANIM_IDLE_BASE);
			setTopAnimation(ANIM_IDLE_TOP);

			// relax the hands since we're not holding anything
			mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
//			mSwordsDrawn = false;
		}

		void setBaseAnimation(AnimID id, bool reset = false)
		{
//			if (mBaseAnimID >= 0 && mBaseAnimID < NUM_ANIMS)
//			{
//				// if we have an old animation, fade it out
//				mFadingIn[mBaseAnimID] = false;
//				mFadingOut[mBaseAnimID] = true;
//			}

			mBaseAnimID = id;
			mAnims[id]->setEnabled(true);
			mAnims[id]->setWeight(1.0f);

//			if (id != ANIM_NONE)
//			{
//				// if we have a new animation, enable it and fade it in
//				mAnims[id]->setEnabled(true);
//				mAnims[id]->setWeight(0);
//				mFadingOut[id] = false;
//				mFadingIn[id] = true;
//				if (reset) mAnims[id]->setTimePosition(0);
//			}
		}


		void setTopAnimation(AnimID id, bool reset = false)
		{
			mTopAnimID = id;
			mAnims[id]->setEnabled(true);
			mAnims[id]->setWeight(1.0f);
/*
			if (mTopAnimID >= 0 && mTopAnimID < NUM_ANIMS)
			{
				// if we have an old animation, fade it out
				mFadingIn[mTopAnimID] = false;
				mFadingOut[mTopAnimID] = true;
			}

			mTopAnimID = id;

			if (id != ANIM_NONE)
			{
				// if we have a new animation, enable it and fade it in
				mAnims[id]->setEnabled(true);
				mAnims[id]->setWeight(0);
				mFadingOut[id] = false;
				mFadingIn[id] = true;
				if (reset) mAnims[id]->setTimePosition(0);
			}
*/
		}

		// called at rendering time, for game play stuff (we will update the player animations here
		void OnApplicationPostTransform (dFloat timestep) 
		{
			Real baseAnimSpeed = 1.0f;
			Real topAnimSpeed = 1.0f;

			// increment the current base and top animation times
			//if (mBaseAnimID != ANIM_NONE) mAnims[mBaseAnimID]->addTime(deltaTime * baseAnimSpeed);
			//if (mTopAnimID != ANIM_NONE) mAnims[mTopAnimID]->addTime(deltaTime * topAnimSpeed);

			mAnims[mBaseAnimID]->addTime (timestep * baseAnimSpeed);
			mAnims[mTopAnimID]->addTime (timestep * topAnimSpeed);
		}


		// called at physics time, we most update the player motion here, 
		// the basic play only apply the gravity, to do anything interesting we must overload OnPlayerMove
		void OnPlayerMove (Real timestep)
		{
			const OgreNewtonWorld* const world = (OgreNewtonWorld*) GetNewton();
			const Vector3& gravity = world->GetGravity();
			//	SetPlayerVelocity (dFloat forwardSpeed, dFloat lateralSpeed, dFloat verticalSpeed, dFloat headingAngle, const dFloat* const gravity, dFloat timestep);
			SetPlayerVelocity (0.0f, 0.0f, 0.0f, 0.0f, &gravity.x, timestep);
		}

		AnimationState* mAnims[NUM_ANIMS];    // master animation list
		//bool mFadingIn[NUM_ANIMS];            // which animations are fading in
		//bool mFadingOut[NUM_ANIMS];           // which animations are fading out

		AnimID mBaseAnimID;                   // current base (full- or lower-body) animation
		AnimID mTopAnimID;                    // current top (upper-body) animation
	};


	OgreNewtonDemoApplication()
		:DemoApplication()
		,m_shootingTimer(0.25f)
		,m_player(NULL)
		,m_playerManager(NULL)
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

		// floor object!
		//Entity* const levelMap = mSceneMgr->createEntity(MakeName("Floor"), "flatplane.mesh" );		
		//Entity* const levelMap = mSceneMgr->createEntity(MakeName("Level"), "chiropteradm.mesh" );
		Entity* const levelMap = mSceneMgr->createEntity(MakeName("Level"), "castle.mesh" );

		SceneNode* const floorNode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "FloorNode" );
		floorNode->attachObject( levelMap );
		levelMap->setCastShadows( false );

		// add this collision to the scene body
		sceneBody->AddCollisionTree (floorNode);

		// done adding collision shape to the scene body, now optimize the scene
		sceneBody->EndAddRemoveCollision();
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

			Entity* const ent = mSceneMgr->createEntity(MakeName ("shootObject"), m_shootingMesh[index]);
			SceneNode* const node = CreateNode (mSceneMgr, ent, cameraPosit, cameraRotation);
			Matrix4 matrix;
			matrix.makeTransform (cameraPosit, Vector3(1.0f, 1.0f, 1.0f), cameraRotation);
			OgreNewtonDynamicBody* const body = new OgreNewtonDynamicBody (m_physicsWorld, 30.0f, m_shootingCollisions[index], node, matrix);

			const Real speed = -40.0f;
			Vector3 veloc (Vector3 (matrix[0][2], matrix[1][2], matrix[2][2]) * speed);   
			body->SetVeloc(veloc);
		}

	}

	virtual void destroyScene()
	{
		for (int i = 0; i < int (sizeof (m_shootingCollisions) / sizeof (m_shootingCollisions[0])); i ++) {
			delete m_shootingCollisions[i];
		}
	}

	MyPlayerContyroller* CreatePlayer()
	{
		// create main model
		Entity* const playerMesh = mSceneMgr->createEntity(MakeName("SinbadBody"), "Sinbad.mesh");
		SceneNode* const playerNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(MakeName("SinbadBody"));
		playerNode->attachObject(playerMesh);

		// the model is about 10 meters hight, calculate the scale to make the play 2 meter high
		playerNode->_updateBounds();
		AxisAlignedBox bBox (playerNode->_getWorldAABB());
		Real high = bBox.getMaximum().y - bBox.getMinimum().y;
		Real scale = 2.0f / high;
		playerNode->setScale(scale, scale, scale);



/*
		// create swords and attach to sheath
		LogManager::getSingleton().logMessage("Creating swords");
		mSword1 = sceneMgr->createEntity("SinbadSword1", "Sword.mesh");
		mSword2 = sceneMgr->createEntity("SinbadSword2", "Sword.mesh");
		mBodyEnt->attachObjectToBone("Sheath.L", mSword1);
		mBodyEnt->attachObjectToBone("Sheath.R", mSword2);

		LogManager::getSingleton().logMessage("Creating the chains");
		// create a couple of ribbon trails for the swords, just for fun
		NameValuePairList params;
		params["numberOfChains"] = "2";
		params["maxElements"] = "80";
		mSwordTrail = (RibbonTrail*)sceneMgr->createMovableObject("RibbonTrail", &params);
		mSwordTrail->setMaterialName("Examples/LightRibbonTrail");
		mSwordTrail->setTrailLength(20);
		mSwordTrail->setVisible(false);
		sceneMgr->getRootSceneNode()->attachObject(mSwordTrail);
		for (int i = 0; i < 2; i++)
		{
			mSwordTrail->setInitialColour(i, 1, 0.8, 0);
			mSwordTrail->setColourChange(i, 0.75, 1.25, 1.25, 1.25);
			mSwordTrail->setWidthChange(i, 1);
			mSwordTrail->setInitialWidth(i, 0.5);
		}

		mKeyDirection = Vector3::ZERO;
		mVerticalVelocity = 0;
*/

		high = 2.0f;
		Real mass = 200.0f;
		Real stairStep = 0.25f * high;
		Real outerRadius = scale * (bBox.getMaximum().z - bBox.getMinimum().z) * 0.5f;
		Real innerRadius = outerRadius * 0.25f;

		MyPlayerContyroller* const player = new MyPlayerContyroller(m_playerManager, playerMesh, playerNode, mass, outerRadius, innerRadius, high, stairStep);

//		Matrix4 matrix;
//		matrix.makeTransform (Vector3(0.0f, 2.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), Quaternion(Quaternion::IDENTITY));
//		matrix= matrix.transpose();
//		player->SetMatrix (&matrix[0][0]);

//delete player;

		return player;
	}

	void createScene()
	{
		// create the physic world first
		DemoApplication::createScene();

		//make a light
		mSceneMgr->setAmbientLight(ColourValue(0.2f, 0.2f, 0.2f));

		Light* const light0 = mSceneMgr->createLight( "Light0" );
		Vector3 lightdir0(-0.55f, -0.5f, -0.75f);
		light0->setType(Light::LT_DIRECTIONAL);
		light0->setDirection(lightdir0);
		light0->setDiffuseColour(ColourValue(1.0f, 1.0f, 1.0f));
		light0->setSpecularColour(ColourValue(0.4f, 0.4f, 0.4f));


		Light* const light1 = mSceneMgr->createLight( "Light1" );
		Vector3 lightdir1(0.4f, -0.9f, 0.5f);
		light1->setType(Light::LT_DIRECTIONAL);
		light1->setDirection(lightdir1);
		light1->setDiffuseColour(ColourValue(1.0f, 1.0f, 1.0f));
		light1->setSpecularColour(ColourValue(0.4f, 0.4f, 0.4f));

		// sky box.
		//mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");
		mSceneMgr->setSkyBox(true, "Examples/MorningSkyBox");

		// load all of the static geometry
		loadStaticScene ();

		// position camera using the ray cast functionality
		Vector3 start(-1.0f,  1000.0f, 4.0f);
		Vector3 end  (-1.0f, -1000.0f, 4.0f);
		OgreNewtonRayCast raycaster(m_physicsWorld); 
		raycaster.CastRay (&start.x, &end.x);

		Vector3 origin (raycaster.m_contact + Vector3 (0.0f, 1.0f, 0.0f));
		mCamera->setPosition(origin);

		// set the near and far clip plane
		mCamera->setNearClipDistance(0.1f);
		mCamera->setFarClipDistance(10000.0f);

		// now load the dynamics Scene
		LoadDynamicScene(origin);

		// create shutting components
		CreateComponentsForShutting(sizeof (m_shootingCollisions)/sizeof (m_shootingCollisions[0]), m_physicsWorld, m_shootingCollisions, m_shootingMesh);

		// initialize the Camera position after the scene was loaded
		ResetCamera (mCamera->getPosition(), mCamera->getOrientation());

		// create a player manager for control all players
		m_playerManager = new OgreNewtonPlayerManager (m_physicsWorld);

		// add some players
		m_player = CreatePlayer();

	}

	Real m_shootingTimer;
	MeshPtr m_shootingMesh[2];
	dNewtonCollision* m_shootingCollisions[2];

	MyPlayerContyroller* m_player;
	OgreNewtonPlayerManager* m_playerManager;

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




 
 

