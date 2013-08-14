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
#include "MouseCursor.h"



MouseCursor::MouseCursor()
	:guiOverlay(0)
	,cursorContainer(0)
{
	material = MaterialManager::getSingleton().create("MouseCursor/Default", "General");
	
	cursorContainer = (OverlayContainer*) OverlayManager::getSingletonPtr()->createOverlayElement("Panel", "MouseCursor");
	cursorContainer->setMaterialName(material->getName());
	cursorContainer->setPosition(0.0f, 0.0f);
	cursorContainer->hide();

	guiOverlay = OverlayManager::getSingletonPtr()->create("MouseCursor");
	guiOverlay->setZOrder(649);
	guiOverlay->add2D(cursorContainer);
	guiOverlay->show(); 
}

MouseCursor::~MouseCursor()
{
	OverlayManager::getSingletonPtr()->destroy(guiOverlay);
	OverlayManager::getSingletonPtr()->destroyOverlayElement(cursorContainer);
	MaterialManager::getSingleton().unload("MouseCursor/Default");
}

void MouseCursor::setImage(const String& filename)
{
	texture = TextureManager::getSingleton().load(filename, "General");
	TextureUnitState *textureUnitState;

	if(material->getTechnique(0)->getPass(0)->getNumTextureUnitStates())
	{
		textureUnitState = material->getTechnique(0)->getPass(0)->getTextureUnitState(0);
	}
	else
	{
		textureUnitState = material->getTechnique(0)->getPass(0)->createTextureUnitState( texture->getName() );
	}

	textureUnitState->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
	material->getTechnique(0)->getPass(0)->setSceneBlending(SBT_TRANSPARENT_ALPHA);
}

void MouseCursor::setWindowDimensions(unsigned int width, unsigned int height)
{
	windowWidth = Real ((width > 0) ? width : 1);
	windowHeight = Real ((height > 0) ? height : 1);

	cursorContainer->setWidth(Real (texture->getWidth()) / Real (windowWidth));
	cursorContainer->setHeight(Real(texture->getHeight()) / Real  (windowHeight));
}

void MouseCursor::setVisible(bool visible)
{
	if(visible)
	{
		cursorContainer->show();
	}
	else
	{
		cursorContainer->hide();
	}
}

void MouseCursor::updatePosition(int x, int y)
{
	Real rx = clamp(x / windowWidth, 0.0f, 1.0f);
	Real ry = clamp(y / windowHeight, 0.0f, 1.0f);

	position.X = x;
	position.Y = y;
	position.normalisedX = rx;
	position.normalisedY = ry;

	cursorContainer->setPosition(rx, ry);
}

Real MouseCursor::clamp(Real a, Real min, Real max)
{
	if(a < min)
	{
		return min;
	}
	if(a > max)
	{
		return max;
	}

	return a;
}

/*
MouseCursor& MouseCursor::getSingleton(void)
{  
	static MouseCursor cursor;
	return cursor;
}
*/