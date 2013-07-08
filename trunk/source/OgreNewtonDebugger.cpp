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


#include "OgreNewtonStdAfx.h"
#include "OgreNewtonWorld.h"
#include "OgreNewtonDebugger.h"


void OgreNewtonDebugger::OgreNewtonGetCollisionMesh::OnDrawFace (int vertexCount, const dFloat* const faceVertex, int id)
{
	Vector3 p0 (faceVertex[(vertexCount - 1) * 3 + 0], faceVertex[(vertexCount - 1) * 3 + 1], faceVertex[(vertexCount - 1) * 3 + 2]);
	for (int i = 0; i < vertexCount; i++) {
		Vector3 p1 (faceVertex[(i*3) + 0], faceVertex[(i*3) + 1], faceVertex[(i*3) + 2]);

		m_data->m_lines->position (p0);
		m_data->m_lines->position (p1);

		p0 = p1;
	}
}


void OgreNewtonDebugger::BodyDebugData::Cleanup(OgreNewtonDebugger* const debugger)
{
	delete m_lines;
	//debugger->m_debugNode->removeChild(m_node);
	debugger->m_debugNode->removeAndDestroyChild(m_node->getName());
}

void OgreNewtonDebugger::BodyDebugData::Init(OgreNewtonDebugger* const debugger, dNewtonBody* const body)
{
	m_lru = debugger->m_lru;

//	Vector3 pos, vel, omega;
//	Quaternion ori;
//	bod->getVisualPositionOrientation(pos, ori);

//	vel = bod->getVelocity();
//	omega = bod->getOmega();

	// ----------- create debug-text ------------
//	std::ostringstream oss_name;
//	oss_name << "__OgreNewt__Debugger__Body__" << bod << "__";
//	std::ostringstream oss_info;
//	oss_info.precision(2);
//	oss_info.setf(std::ios::fixed,std::ios::floatfield);
//	Vector3 inertia;
//	Real mass;
//	bod->getMassMatrix(mass, inertia);

//	oss_info << "[" << bod->getOgreNode()->getName() << "]" << std::endl;
//	oss_info << "Mass: " << mass << std::endl;
//	oss_info << "Position: " << pos[0] << " x " << pos[1] << " x " << pos[2] << std::endl;
//	oss_info << "Velocity: " << vel[0] << " x " << vel[1] << " x " << vel[2] << std::endl;
//	oss_info << "Omega: " << omega[0] << " x " << omega[1] << " x " << omega[2] << std::endl;
//	oss_info << "Inertia: " << inertia[0] << " x " << inertia[1] << " x " << inertia[2] << std::endl;

	// ----------- ------------------ ------------
	// look for cached data
//	BodyDebugData* data = &m_cachemap[bod];

/*
	if( data->m_lastcol == bod->getCollision() ) // use cached data
	{
		// set new position...
		data->m_node->setPosition(pos);
		data->m_node->setOrientation(ori);
		data->m_updated = 1;
		m_debugnode->addChild(data->m_node);
		data->m_text->setCaption(oss_info.str());
		data->m_text->setLocalTranslation(bod->getAABB().getSize().y * 1.1f * Vector3::UNIT_Y);
	}
	else
	{
		data->m_lastcol = bod->getCollision();
		data->m_updated = 1;

		if( data->m_node )
		{
			data->m_node->detachAllObjects();
			data->m_node->setPosition(pos);
			data->m_node->setOrientation(ori);
		}
		else
			data->m_node = m_debugnode->createChildSceneNode(pos, ori);

		if( data->m_lines )
		{
			data->m_lines->clear();
		}
		else
		{
			std::ostringstream oss;
			oss << "__OgreNewt__Debugger__Lines__" << bod << "__";
			data->m_lines = new ManualObject(oss.str());
		}

		if( data->m_text )
		{
			data->m_text->setCaption(oss_info.str());
			data->m_text->setLocalTranslation(bod->getAABB().getMaximum().y * 1.1f * Vector3::UNIT_Y);
		}
		else
		{
			data->m_text = new OgreNewt::OgreAddons::MovableText( oss_name.str(), oss_info.str(), "BlueHighway-10",0.5);
			data->m_text->setLocalTranslation(bod->getAABB().getMaximum().y / 2.0f * Vector3::UNIT_Y + Vector3::UNIT_Y * 0.1f);
			data->m_text->setTextAlignment( OgreNewt::OgreAddons::MovableText::H_LEFT, OgreNewt::OgreAddons::MovableText::V_ABOVE );
		}

		data->m_node->attachObject(data->m_text);
	    

//		data->m_lines->begin("BaseWhiteNoLighting", RenderOperation::OT_LINE_LIST );
		// set color
//		if( it != m_materialcolors.end() )
//			data->m_lines->colour(it->second);
//		else
//			data->m_lines->colour(m_defaultcolor);
//
//		float matrix[16];
//		Converters::QuatPosToMatrix(Quaternion::IDENTITY, Vector3::ZERO, &matrix[0]);
//	    
//		NewtonCollisionForEachPolygonDo( NewtonBodyGetCollision(newtonBody), &matrix[0], newtonPerPoly, data->m_lines );
//		data->m_lines->end();

		buildDebugObjectFromCollision (data->m_lines, m_defaultcolor, *bod->getCollision());

		data->m_node->attachObject(data->m_lines);
	}
*/

	Matrix4 matrix;
	body->GetMatrix (matrix[0]);
	matrix.transpose();
	m_node = debugger->m_debugNode->createChildSceneNode (matrix.getTrans(), matrix.extractQuaternion());

	char name[256];
	sprintf (name, "__debug_collsion_shape_%d__", debugger->m_uniqueID);
	debugger->m_uniqueID ++;
	m_lines = new ManualObject(name);

	dNewtonCollision* const collision = body->GetCollision();
	OgreNewtonGetCollisionMesh getMeshFaces (collision, this);

	m_lines->begin("BaseWhiteNoLighting", RenderOperation::OT_LINE_LIST );

	Matrix4 localMatrix (Matrix4::IDENTITY);
	collision->DebugRender (localMatrix[0], &getMeshFaces);
	m_lines->end();

	m_node->attachObject(m_lines);
}


OgreNewtonDebugger::OgreNewtonDebugger (SceneManager* const sceneMgr, OgreNewtonWorld* const world)
	:m_sceneMgr (sceneMgr)
	,m_world(world)
	,m_lru(0)
	,m_debugMode(false)
{
	m_debugNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode("_OgreNewton_Debugger_Node_");
}

OgreNewtonDebugger::~OgreNewtonDebugger()
{
	HideDebugInformation();
	m_debugNode->removeAndDestroyAllChildren();
	m_debugNode->getParentSceneNode()->removeAndDestroyChild (m_debugNode->getName());
}

void OgreNewtonDebugger::SetDebugMode(bool onOff)
{
	m_debugMode = onOff;
}



bool OgreNewtonDebugger::frameStarted(const FrameEvent &evt)
{
	if (m_debugMode) {
		ShowDebugInformation();
	} else {
		HideDebugInformation();
	}
	return true;
}



void OgreNewtonDebugger::ShowDebugInformation()
{
	m_lru ++;
	for (dNewtonBody* body = m_world->GetFirstBody(); body; body = m_world->GetNextBody(body)) {
		BodyDebugDataMap::iterator item (m_cachemap.find(body));
		if (item == m_cachemap.end()) {
			std::pair<BodyDebugDataMap::iterator, bool> pair (m_cachemap.insert(std::make_pair (body, BodyDebugData())));
			item = pair.first;
			BodyDebugData& data = item->second;
			data.Init(this, body);
		}
		BodyDebugData& data = item->second;
		if (data.m_lru < (m_lru - 1)) {
			data.Cleanup(this);
			m_cachemap.erase (item);
		} else {
			data.m_lru = m_lru;

			Matrix4 matrix;
			body->GetMatrix (matrix[0]);
			matrix = matrix.transpose();
			data.m_node->setPosition (matrix.getTrans());
			data.m_node->setOrientation (matrix.extractQuaternion());
		}
	}
}

void OgreNewtonDebugger::HideDebugInformation()
{
	for(BodyDebugDataMap::iterator iter = m_cachemap.begin(); iter != m_cachemap.end(); iter++) {
		BodyDebugData& data = iter->second;
		data.Cleanup(this);
	}
	m_cachemap.erase(m_cachemap.begin(), m_cachemap.end());
}


