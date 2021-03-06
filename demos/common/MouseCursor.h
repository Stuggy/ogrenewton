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

#ifndef _MOUSECURSOR_H_
#define _MOUSECURSOR_H_

#include "HelpersStdAfx.h"

using namespace Ogre;

class MouseCursor
{
	public:
	MouseCursor();
	~MouseCursor();

	struct Position
	{
		Position() 
		{
			X = 0;
			Y = 0;
			normalisedX = 0.0f;
			normalisedY = 0.0f;
		}

		int X;
		int Y;
		float normalisedX;
		float normalisedY;
	};

	void setImage(const String& filename);
	void setWindowDimensions(unsigned int width, unsigned int height);
	void setVisible(bool visible);
	void updatePosition(int x, int y);

	Position getPosition() { return position; };

	//static MouseCursor& getSingleton(void);

	private:
	Real clamp(Real a, Real min, Real max);

	Overlay* guiOverlay;
	OverlayContainer* cursorContainer;
	TexturePtr texture;
	MaterialPtr material;
	Real windowWidth;
	Real windowHeight;
	Position position;
};

#endif