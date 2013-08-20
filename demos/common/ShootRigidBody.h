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

#ifndef _SHOOT_ROGID_BODY_H__
#define _SHOOT_ROGID_BODY_H__

#include <HelpersStdAfx.h>
//#include <OgreNewtonStdAfx.h>
//#include <OgreTexture.h>
//#include <OgreOverlayManager.h>
//#include <OgreMaterialManager.h>
//#include <OgrePanelOverlayElement.h>

using namespace Ogre;

class DemoApplication;


class ShootRigidBody: public FrameListener
{
	public:
	ShootRigidBody (DemoApplication* const application);
	~ShootRigidBody();
	virtual bool frameStarted(const FrameEvent& evt);

	Real m_shootingTimer;
	MeshPtr m_shootingMesh[2];
	dNewtonCollision* m_shootingCollisions[2];
	DemoApplication* m_application;
};

#endif