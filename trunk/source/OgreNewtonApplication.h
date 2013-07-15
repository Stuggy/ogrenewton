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


#ifndef _OGRE_NEWTON_APPLICATION_H_
#define _OGRE_NEWTON_APPLICATION_H_

#include "OgreNewtonStdAfx.h"

using namespace Ogre;


#include <OgreOverlay.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <ExampleApplication.h>
#include <ExampleFrameListener.h>


class OGRE_NEWTON_API OgreNewtonApplication: public ExampleApplication
{
	public:
	OgreNewtonApplication();
	virtual ~OgreNewtonApplication();

	OgreNewtonWorld* GetPhysics() const { return m_physicsWorld;}

	// these are the main even call back the client application implement for controlling the  application
	// these function are all call asynchronous and in parallel
	virtual void OnPhysicUpdateBegin(dFloat timestepInSecunds) {}
	virtual void OnPhysicUpdateEnd(dFloat timestepInSecunds) {}

	virtual bool OnRenderUpdateBegin(dFloat updateParam) 
	{
		return true;
	}

	virtual bool OnRenderUpdateEnd(dFloat updateParam) 
	{
		return true;
	}

	
	protected:
	virtual void createScene(void);
	virtual void destroyScene(void);

	protected:
	OgreNewtonWorld* m_physicsWorld;
};



#endif
