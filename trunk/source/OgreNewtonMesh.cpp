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
{
}


OgreNewtonMesh::OgreNewtonMesh (const dNewtonCollision* const collision)
	:dNewtonMesh (*collision)
{

}

OgreNewtonMesh::~OgreNewtonMesh()
{
}

ManualObject* OgreNewtonMesh::CreateEntiry (const String& name) const
{
	ManualObject* const object = new ManualObject(name);

	int pointCount = GetPointCount();
	int indexCount = GetTotalIndexCount();

	int* const indexList = new int [indexCount];
	bool* const remapIndex = new bool [indexCount];
	dNewtonMesh::dPoint* const posits = new dNewtonMesh::dPoint[pointCount];
	dNewtonMesh::dPoint* const normals = new dNewtonMesh::dPoint[pointCount];
	dNewtonMesh::dUV* const uv0 = new dNewtonMesh::dUV[pointCount];
	dNewtonMesh::dUV* const uv1 = new dNewtonMesh::dUV[pointCount];
	
	GetVectexStreams(posits, normals, uv0, uv1);

	void* const materialsHandle = BeginMaterialHandle (); 
	for (int handle = GetMaterialIndex (materialsHandle); handle != -1; handle = GetNextMaterialIndex (materialsHandle, handle)) {
		

		int materialIndex = MaterialGetMaterial (materialHandle, handle); 
		int indexCount = MaterialGetIndexCount (materialHandle, handle); 
		MaterialGetIndexStream (materialHandle, handle, indexList); 

		object->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		// ogre does not support shared vertex for sub mesh, 
		// we will have remap the vertex data
		int vertexCount = 0;
		memset (remapIndex, false,  indexCount * sizeof (bool));
		for( int i = 0; i < indexCount; i ++) {
			int index = indexList[i];
			if (!remapIndex[i]) {
				remapIndex[i] = true;
				indexList[i] = vertexCount;


				vertexCount ++;
			}
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
