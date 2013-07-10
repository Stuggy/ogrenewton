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
#include "OgreNewtonMesh.h"
#include "OgreNewtonWorld.h"


OgreNewtonMesh::OgreNewtonMesh (dNewton* const world)
	:dNewtonMesh (world)
	,m_materialMap()
{
}


OgreNewtonMesh::OgreNewtonMesh (const dNewtonCollision* const collision)
	:dNewtonMesh (*collision)
{

}

OgreNewtonMesh::~OgreNewtonMesh()
{
}

int OgreNewtonMesh::AddMaterial (const Ogre::MaterialPtr& material)
{
	m_materialMap.insert(std::make_pair (m_materialMap.m_enumerator, material));
	m_materialMap.m_enumerator ++;
	return m_materialMap.m_enumerator - 1;
}


ManualObject* OgreNewtonMesh::CreateEntity (const String& name) const
{
	ManualObject* const object = new ManualObject(name);

	int pointCount = GetPointCount();
	int indexCount = GetTotalIndexCount();

	int* const indexList = new int [indexCount];
	int* const remapIndex = new int [indexCount];
	dNewtonMesh::dPoint* const posits = new dNewtonMesh::dPoint[pointCount];
	dNewtonMesh::dPoint* const normals = new dNewtonMesh::dPoint[pointCount];
	dNewtonMesh::dUV* const uv0 = new dNewtonMesh::dUV[pointCount];
	dNewtonMesh::dUV* const uv1 = new dNewtonMesh::dUV[pointCount];
	
	GetVertexStreams(posits, normals, uv0, uv1);

	void* const materialsHandle = BeginMaterialHandle (); 
	for (int handle = GetMaterialIndex (materialsHandle); handle != -1; handle = GetNextMaterialIndex (materialsHandle, handle)) {
		int materialIndex = MaterialGetMaterial (materialsHandle, handle); 
		int indexCount = MaterialGetIndexCount (materialsHandle, handle); 
		MaterialGetIndexStream (materialsHandle, handle, indexList); 

		MaterialMap::const_iterator materialItr = m_materialMap.find(materialIndex);

		//object->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		object->begin(materialItr->second->getName(), Ogre::RenderOperation::OT_TRIANGLE_LIST);
		
		// ogre does not support shared vertex for sub mesh, we will have remap the vertex data
		int vertexCount = 0;
		memset (remapIndex, 0xff, indexCount * sizeof (remapIndex[0]));
		for( int i = 0; i < indexCount; i ++) {
			int index = indexList[i];
			if (remapIndex[index] == -1) {
				remapIndex[index] = vertexCount;
				object->position (posits[index].m_x, posits[index].m_y, posits[index].m_z);
				object->normal (normals[index].m_x, normals[index].m_y, normals[index].m_z);
				object->textureCoord (uv0[index].m_u, uv0[index].m_v);
				vertexCount ++;
			}
			indexList[i] = remapIndex[index];
			
		}

		for (int i = 0; i < indexCount; i += 3) {
			object->triangle (indexList[i + 0], indexList[i + 1], indexList[i + 2]);
		}
		object->end();
	}
	EndMaterialHandle (materialsHandle); 


	delete[] indexList;
	delete[] remapIndex;
	delete[] uv1;
	delete[] uv0;
	delete[] normals;
	delete[] posits;
	return object;
}
