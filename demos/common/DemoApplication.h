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

#ifndef _DEMO_APPLICATION_H_
#define _DEMO_APPLICATION_H_

#include "HelpersStdAfx.h"
#include "Utils.h"

using namespace Ogre;

class MouseCursor;
class ScreenWriter;


class DemoApplication: public OIS::MouseListener, public OIS::KeyListener, public WindowEventListener
{
	public:
	class SmoothCamera: public dNewtonTransformLerp
	{
		public:
		SmoothCamera();
		void Reset (const Vector3& posit, const Quaternion& rotation);
		void Move (Real deltaTranslation, Real deltaStrafe, Radian pitchAngleStep, Radian yawAngleStep);
		
		Radian m_cameraYawAngle;
		Radian m_cameraPitchAngle;
		Vector3 m_cameraTranslation;
	};

	class OgreNewtonExample: public OgreNewtonWorld
	{
		public:
		OgreNewtonExample(DemoApplication* const application);
		~OgreNewtonExample();

		// called asynchronous at the beginning of a physics update. 
		virtual void OnBeginUpdate (dFloat timestepInSecunds);

		// called asynchronous at the beginning end a physics update. 
		virtual void OnEndUpdate (dFloat timestepInSecunds);

		// called synchronous with ogre update loop, at the beginning of setting all node transform after a physics update  
		virtual void OnNodesTransformBegin(Real interpolationParam);

		// called synchronous with ogre update loop, at the end of setting all node transform after a physics update  
		virtual void OnNodesTransformEnd(Real interpolationParam);


		// broad phase AABB overlap
		virtual bool OnBodiesAABBOverlap (const dNewtonBody* const body0, const dNewtonBody* const body1, int threadIndex) const;
		virtual bool OnCompoundSubCollisionAABBOverlap (const dNewtonBody* const body0, const dNewtonCollision* const subShape0, const dNewtonBody* const body1, const dNewtonCollision* const subShape1, int threadIndex) const;
		virtual void OnContactProcess (dNewtonContactMaterial* const contacts, dFloat timestep, int threadIndex) const;


		DemoApplication* const m_application;
	};


	DemoApplication ();
	virtual ~DemoApplication(void);

	bool go(void);
	virtual void createScene();
	virtual void destroyScene();

	SceneManager* GetSceneManager() const;
	OgreNewtonWorld* GetPhysics() const;
	OIS::Keyboard* GetKeyboard() const;

	Real GetCameraYawAngle() const;
	Matrix4 GetCameraTransform () const;
	void SeCameraTransform (const Matrix4& matrix);
	void ResetCamera (const Vector3& posit, const Quaternion& rot);
	virtual OgreNewtonExample* OnCreateWorld (DemoApplication* const application);

	protected:
	void InitCamera();
	void InitInputSystem();
	void UpdateMousePick ();

	virtual void UpdateFreeCamera ();

	// called asynchronous at the beginning of a physics update. do any pre-physics update here  
	virtual void OnPhysicUpdateBegin(dFloat timestepInSecunds);

	// called asynchronous at the beginning of a physics update. do any post-physics update here, moving the camera matrix  
	virtual void OnPhysicUpdateEnd(dFloat timestepInSecunds);

	// called synchronous from ogre update loop before updating all sceneNodes controlled by a physic body  
	virtual void OnRenderUpdateBegin(dFloat updateParam);

	// called synchronous from ogre update loop after of updating updating all sceneNodes controlled by a physic body  
	virtual void OnRenderUpdateEnd(dFloat updateParam);


	// collect operating system events
	virtual void windowMoved (RenderWindow* rw); 
	virtual void windowResized(RenderWindow* rw);
	virtual void windowFocusChange(RenderWindow* rw);
	virtual void windowClosed(RenderWindow* rw);
	virtual bool keyPressed(const OIS::KeyEvent &evt);
	virtual bool keyReleased(const OIS::KeyEvent &evt);
	virtual bool mouseMoved (const OIS::MouseEvent &evt);
	virtual bool mousePressed( const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	virtual bool mouseReleased( const OIS::MouseEvent &evt, OIS::MouseButtonID id);



	Root* mRoot;
	Camera* mCamera;
	RenderWindow* mWindow;
	SceneManager* mSceneMgr;
	OgreNewtonWorld* m_physicsWorld;
	OgreNewtonDebugger* m_debugRender;
	OIS::InputManager* m_ois;
	OIS::Mouse* m_mouse;
	OIS::Keyboard* m_keyboard;
	MouseCursor* m_cursor;
	ScreenWriter* m_screen;
	SmoothCamera m_cameraSmoothing;

	KeyTrigger m_onScreeHelp;
	KeyTrigger m_debugTriggerKey;
	KeyTrigger m_asyncronousUpdateKey;
	String mResourcesCfg;
	String mPluginsCfg;
	OIS::MouseState m_mouseState;

	Real m_pickParam;
	unsigned m_scopeLock;
	bool m_mousePickMemory;
	bool m_exitApplication;
	bool m_initializationSuccessful;

	friend class OgreNewtonExample;
};



#endif