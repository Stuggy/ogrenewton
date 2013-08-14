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

#ifndef SCREEN_WRITER_H
#define SCREEN_WRITER_H

#include <HelpersStdAfx.h>

using namespace Ogre;

class ScreenWriter// : public Singleton<ScreenWriter>
{
public:
	struct Entry
	{
		Entry()
		{
			x = 0;
			y = 0;
			size = 1.0f;
		}

		float x;
		float y;
		float size;

		std::string text;
	};

	typedef std::vector<Entry> Strings;

	ScreenWriter(int windowWidth = 800, int windowHeight = 600);
	~ScreenWriter();
	
	void updateWindowExtents(int winWidth, int winHeight);
	
	void write(float x, float y, const char*, ...);
	void write(float x, float y, String, ...);
	void write(float x, float y, float size, const char*, ...);
	void write(Entry& entry);

	void begin(float x, float y, float size);
	void write(const char*, ...);

	void removeVerticalAt(float x);
	void removeAll();

	void update();

	static ScreenWriter& getSingleton(void);
	static ScreenWriter* getSingletonPtr(void);

private:
	void initialise();
	const char* vprint(const char* format, va_list args);

	Strings texts;

	float defaultSize;

	int windowWidth;
	int windowHeight;

	Overlay* overlay;
	OverlayContainer* panel;

	enum { MAX_TEXT_AREAS = 50 };

	TextAreaOverlayElement* textAreas[MAX_TEXT_AREAS];

	struct Context
	{
		Context();

		float x;
		float y;
		float size;
	} context;
};

#endif
