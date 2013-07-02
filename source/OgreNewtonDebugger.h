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
#ifndef _OGRE_NEWTON_DEBUGGER
#define _OGRE_NEWTON_DEBUGGER


using namespace Ogre;

class OgreNewtonBody;

class OgreNewtonDebugger: public FrameListener
{
    public:

	class BodyDebugData
	{
		public:
		BodyDebugData() 
			:m_node(NULL)
			,m_lines(NULL)
			,m_lru (0)
		{
		}

		~BodyDebugData() 
		{
		}

		void Init (OgreNewtonDebugger* const debugger, dNewtonBody* const body);
		void Cleanup (OgreNewtonDebugger* const debugger);
		
		SceneNode* m_node;
		ManualObject* m_lines;
		int m_lru;
	};            

	typedef std::map<dNewtonBody*, BodyDebugData> BodyDebugDataMap;

	class OgreNewtonGetCollisionMesh: public dNewtonCollision::dDebugRenderer
	{
		public:
		OgreNewtonGetCollisionMesh (dNewtonCollision* const collision, BodyDebugData* const data)
			:dNewtonCollision::dDebugRenderer(collision)
			,m_data(data)
		{
		}

		void OnDrawFace (int vertexCount, const dFloat* const faceVertex, int id);
		
		BodyDebugData* m_data; 
	};

	OgreNewtonDebugger (SceneManager* const sceneMgr, OgreNewtonWorld* const world);
	virtual ~OgreNewtonDebugger();

	void SetDebugMode(bool onOff);

	private:
	bool frameStarted(const FrameEvent &evt);
	void ShowDebugInformation();
	void HideDebugInformation();

	SceneManager* m_sceneMgr;
	OgreNewtonWorld* m_world;
	SceneNode* m_debugNode;

	int m_lru;
	int m_uniqueID;
	BodyDebugDataMap m_cachemap;
	bool m_debugMode;
};




#endif  

