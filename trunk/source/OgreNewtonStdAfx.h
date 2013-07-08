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

#ifndef _OGRE_NEWTON_STDAFX_H_
#define _OGRE_NEWTON_STDAFX_H_

#ifdef _MSC_VER
	#pragma warning (disable: 4193)//: #pragma warning(pop) : no matching '#pragma warning(push)'
	#pragma warning (disable: 4127) // conditional expression is constant
	#pragma warning (disable: 4512) //'OIS::MouseEvent' : assignment operator could not be generated
	#pragma warning (disable: 4100) // 'code' : unreferenced formal parameter
	#pragma warning (disable: 4275) //: non dll-interface struct 'Ogre::Box' used as base for dll-interface class 'Ogre::PixelBox'
	#pragma warning (disable: 4251) //'Ogre::Material::mTechniques' : class 'std::vector<_Ty,_Ax>' needs to have dll-interface to be used by clients of class 'Ogre::Material'
#endif


#include <Ogre.h>
#include <OgreRoot.h>
#include <OgreNode.h>
#include <OgreVector3.h>
#include <OgreSceneNode.h>
#include <OgreQuaternion.h>
#include <OgreRenderable.h>
#include <OgreSceneManager.h>
#include <OgreManualObject.h>
#include <OgreMovableObject.h>
#include <OgreFrameListener.h>


#include <Newton.h>
#include <dNewton.h>
#include <dNewtonBody.h>
#include <dNewtonMesh.h>
#include <dNewtonRayCast.h>
#include <dNewtonCollision.h>
#include <dNewtonTranformLerp.h>
#include <CustomControllerManager.h>


#endif



