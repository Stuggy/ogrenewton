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

#include "ScreenWriter.h"
#include <OgreTextAreaOverlayElement.h>

//template<> ScreenWriter* Singleton<ScreenWriter>::ms_Singleton = 0;

ScreenWriter::ScreenWriter(int winWidth, int winHeight)
{
	panel        = NULL;
	overlay      = NULL;
	defaultSize  = 0.025f;
	windowWidth  = winWidth;
	windowHeight = winHeight;

	memset(textAreas, 0, MAX_TEXT_AREAS * sizeof(TextAreaOverlayElement*));

	initialise();
}

ScreenWriter::~ScreenWriter()
{
	OverlayManager::getSingleton().destroy(overlay);

	for (int i = 0; i < MAX_TEXT_AREAS; ++i)
	{
		OverlayManager::getSingleton().destroyOverlayElement(textAreas[i]);
	}

	OverlayManager::getSingleton().destroyOverlayElement(panel);
}

void ScreenWriter::initialise()
{
	panel = static_cast<OverlayContainer*>(OverlayManager::getSingleton().createOverlayElement("Panel", "DebugTextPanel"));
	panel->setMetricsMode(GMM_RELATIVE);
	panel->setPosition(0, 0);
	panel->setDimensions(1.0f, 1.0f);
	//panel->setMaterialName("MaterialName"); // Optional background material

	for (int i = 0; i < MAX_TEXT_AREAS; ++i)
	{
		textAreas[i] = static_cast<TextAreaOverlayElement*>(OverlayManager::getSingleton().createOverlayElement("TextArea", StringConverter::toString(i) + "TextAreaName"));
		textAreas[i]->setMetricsMode(GMM_RELATIVE);
		textAreas[i]->setFontName("DebugText");
		textAreas[i]->setColourBottom(ColourValue(1,1,1));
		textAreas[i]->setColourTop(ColourValue(1,1,1));
		textAreas[i]->setDimensions(1.0f, 1.0f);
		textAreas[i]->setCharHeight(defaultSize);
		panel->addChild(textAreas[i]);
	}

	overlay = OverlayManager::getSingleton().create("ScreenWriter");
	overlay->add2D(panel);
	overlay->setZOrder(500);
	overlay->show();
}

void ScreenWriter::updateWindowExtents(int winWidth, int winHeight)
{
	windowWidth = winWidth;
	windowHeight = winHeight;
}

void ScreenWriter::update()
{
	int count = 0;

	for(Strings::iterator i = texts.begin(); i != texts.end() && count < MAX_TEXT_AREAS; ++i)
	{
		Entry& entry = *i;

		if(entry.text.length() > 0)
		{
			Vector2 position(entry.x, entry.y);

			if (position.x < 0)
			{
				if (position.x >= -1.0f)
				{
					position.x = 1.0f + position.x;
				}
				else
				{
					position.x = windowWidth + position.x;
				}
			}

			if (position.y < 0)
			{
				if (position.y >= -1.0f)
				{
					position.y = 1.0f + position.y;
				}
				else
				{
					position.y = windowHeight + position.y;
				}
			}

			if(fabs(position.x) > 1.0f)
			{
				position.x = position.x / windowWidth;
			}

			if(fabs(position.y) > 1.0f)
			{
				position.y = position.y / windowHeight;
			}

			textAreas[count]->setPosition(position.x, position.y);
			textAreas[count]->setCaption(entry.text.c_str());
			textAreas[count]->setCharHeight(defaultSize * entry.size);
			textAreas[count]->show();
			
			count++;

			assert(count < MAX_TEXT_AREAS);
		}
	}

	for(int j = count; j < MAX_TEXT_AREAS; j++)
	{
		textAreas[j]->setCaption("");
	}
}

const char* ScreenWriter::vprint(const char* format, va_list args)
{
	static const int BUFFER_SIZE = 200;
	static char buffer[BUFFER_SIZE];
	int size = printf(format, args);
	assert(size < BUFFER_SIZE);
	vsnprintf(buffer, BUFFER_SIZE, format, args);
	return buffer;
}

void ScreenWriter::write(float x, float y, const char* format, ...)
{
	Entry entry;
	entry.x = x;
	entry.y = y;
	va_list args;
	va_start(args, format);   
	entry.text = vprint(format, args);
	va_end(args);   
	
	write(entry);
}

void ScreenWriter::write(float x, float y, String format, ...)
{
	Entry entry;
	entry.x = x;
	entry.y = y;
	va_list args;
	const char* formatc = format.c_str();
	va_start(args, formatc);   
	entry.text = vprint(formatc, args);
	va_end(args);   
	
	write(entry);
}

void ScreenWriter::write(float x, float y, float size, const char* format, ...)
{
	Entry entry;
	entry.x = x;
	entry.y = y;
	entry.size = size;
	va_list args;
	va_start(args, format);   
	entry.text = vprint(format, args);
	va_end(args);   
	
	write(entry);
}

ScreenWriter::Context::Context()
{
	x = 0.0f;
	y = 0.0f;
	size = 1.0f;
}

void ScreenWriter::begin(float x, float y, float size)
{
	context.x = x;
	context.y = y;
	context.size = size;
}
void ScreenWriter::write(const char* format, ...)
{
	Entry entry;
	entry.x = context.x;
	entry.y = context.y;

	entry.size = context.size;
	context.y += context.size * defaultSize;
	va_list args;
	va_start(args, format);   
	entry.text = vprint(format, args);
	va_end(args);   
	
	write(entry);
}

void ScreenWriter::write(Entry& entry)
{
	if(texts.size() > 0)
	{
		for(Strings::iterator i = texts.begin(); i != texts.end(); i++)
		{
			Entry& existingEntry = *i;

			if(existingEntry.x == entry.x && existingEntry.y == entry.y)
			{
				existingEntry.text = entry.text;
				existingEntry.size = entry.size;

				return;
			}
		}
	}

	texts.push_back(entry);
}

void ScreenWriter::removeVerticalAt(float x)
{
	if(x > 1.0f)
	{
		x = x / windowWidth;
	}

	if(texts.size() > 0)
	{
		for(Strings::iterator i = texts.begin(); i != texts.end();)
		{
			Entry& existingEntry = *i;

			if(fabs(existingEntry.x - x) < 0.00001f)
			{
				i = texts.erase(i);
			}
			else
			{
				i++;
			}
		}
	}
}

void ScreenWriter::removeAll()
{
	if(texts.size() > 0)
	{
		for(Strings::iterator i = texts.begin(); i != texts.end();)
		{
			i = texts.erase(i);
		}
	}
}
