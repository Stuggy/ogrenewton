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

OgreNewtonMesh::OgreNewtonMesh (dNewton* const world, const Entity* const entity)
	:dNewtonMesh (world)
	,m_materialMap()
{
	BeginPolygon();
	ParseEntity (entity);
	EndPolygon();
}


OgreNewtonMesh::OgreNewtonMesh (dNewton* const world, SceneNode* const sceneNode)
	:dNewtonMesh (world)
	,m_materialMap()

{
	Vector3 rootPos (Vector3::ZERO);
	Vector3 rootScale = sceneNode->getScale();
	Quaternion rootOrient = Quaternion::IDENTITY;

	// parse the entire node adding all static mesh to the collision tree
	int stack = 1;
	SceneNode* stackNode[32];
	Vector3 scale[32]; 
	Vector3 posit[32]; 
	Quaternion orientation[32]; 

	stackNode[0] = sceneNode;
	posit[0] = Vector3::ZERO;
	scale[0] = Vector3 (1.0f, 1.0f, 1.0f);
	orientation[0] = Quaternion::IDENTITY;

	BeginPolygon();

	while (stack) {
		stack --;
		Vector3 curScale (scale[stack]);
		Vector3 curPosit (posit[stack]);
		Quaternion curOrient (orientation[stack]);
		SceneNode* const node = stackNode[stack];

		// parse this scene node.
		Quaternion thisOrient (curOrient * node->getOrientation());
		Vector3 thisPos (curPosit + (curOrient * (node->getPosition() * curScale)));
		Vector3 thisScale (curScale * node->getScale());

		// now add the polys from this node.
		unsigned int num_obj = node->numAttachedObjects();
		for (unsigned int co = 0; co < num_obj; co++) {
			MovableObject* const obj = node->getAttachedObject(short(co));
			if (obj->getMovableType() == "Entity") {
				ParseEntity ((Entity*) obj);
			}
		}

		SceneNode::ChildNodeIterator child_it =	node->getChildIterator();
		while (child_it.hasMoreElements()) {
			stackNode[stack] = sceneNode;
			scale[stack] = thisScale;
			posit[stack] = thisPos;
			orientation[stack] = thisOrient;
			stack ++;
			dAssert (stack < int (sizeof (stackNode[stack]) / sizeof (stackNode[0])));
		}
	}

	EndPolygon();
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

	dNewtonScopeBuffer<int> indexList (indexCount);
	dNewtonScopeBuffer<int> remapIndex (indexCount);
	dNewtonScopeBuffer<dNewtonMesh::dPoint> posits (pointCount);
	dNewtonScopeBuffer<dNewtonMesh::dPoint> normals (pointCount);
	dNewtonScopeBuffer<dNewtonMesh::dUV> uv0 (pointCount);
	dNewtonScopeBuffer<dNewtonMesh::dUV> uv1 (pointCount);
	
	GetVertexStreams(&posits[0], &normals[0], &uv0[0], &uv1[0]);

	void* const materialsHandle = BeginMaterialHandle (); 
	for (int handle = GetMaterialIndex (materialsHandle); handle != -1; handle = GetNextMaterialIndex (materialsHandle, handle)) {
		int materialIndex = MaterialGetMaterial (materialsHandle, handle); 
		int indexCount = MaterialGetIndexCount (materialsHandle, handle); 
		MaterialGetIndexStream (materialsHandle, handle, &indexList[0]); 

		MaterialMap::const_iterator materialItr = m_materialMap.find(materialIndex);

		//object->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		object->begin(materialItr->second->getName(), Ogre::RenderOperation::OT_TRIANGLE_LIST);
		
		// ogre does not support shared vertex for sub mesh, we will have remap the vertex data
		int vertexCount = 0;
		memset (&remapIndex[0], 0xff, indexCount * sizeof (remapIndex[0]));
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

	return object;
}

void OgreNewtonMesh::ParseEntity (const Entity* const entity)
{
	MeshPtr mesh = entity->getMesh();

	//find number of sub-meshes
	unsigned short sub = mesh->getNumSubMeshes();

	for (unsigned short cs = 0; cs < sub; cs++) {

		SubMesh* const sub_mesh = mesh->getSubMesh(cs);

		//vertex data!
		VertexData* v_data;

		if (sub_mesh->useSharedVertices) {
			v_data = mesh->sharedVertexData;
		} else {
			v_data = sub_mesh->vertexData;
		}

		//let's find more information about the Vertices...
		VertexDeclaration* const v_decl = v_data->vertexDeclaration;

		const VertexElement* const vertexElem = v_decl->findElementBySemantic(VES_POSITION);
		HardwareVertexBufferSharedPtr vertexPtr = v_data->vertexBufferBinding->getBuffer(vertexElem->getSource());
		dNewtonScopeBuffer<Vector3> points(vertexPtr->getNumVertices());
		{
			int size = vertexPtr->getVertexSize();
			int offset = vertexElem->getOffset() / sizeof (float);
			unsigned char* const ptr = static_cast<unsigned char*> (vertexPtr->lock(HardwareBuffer::HBL_READ_ONLY));
			for (int i = 0; i < points.GetElementsCount(); i ++) {
				float* data;
				vertexElem->baseVertexPointerToElement(ptr + i * size, &data);
				points[i] = Vector3 (data[offset + 0], data[offset + 1], data[offset + 2]);
			}
			vertexPtr->unlock();
		}

		const VertexElement* const normalElem = v_decl->findElementBySemantic(VES_NORMAL);
		HardwareVertexBufferSharedPtr normalPtr = v_data->vertexBufferBinding->getBuffer(normalElem->getSource());
		dNewtonScopeBuffer<Vector3> normals (normalPtr->getNumVertices());
		{
			int size = normalPtr->getVertexSize();
			int offset = vertexElem->getOffset() / sizeof (float);
			unsigned char* const ptr = static_cast<unsigned char*> (normalPtr->lock(HardwareBuffer::HBL_READ_ONLY));
			for (int i = 0; i < normals.GetElementsCount(); i ++) {
				float* data;
				vertexElem->baseVertexPointerToElement(ptr + i * size, &data);
				normals[i] = Vector3 (data[offset + 0], data[offset + 1], data[offset + 2]);
			}
			normalPtr->unlock();
		}


		const VertexElement* const uvElem = v_decl->findElementBySemantic(VES_TEXTURE_COORDINATES);
		HardwareVertexBufferSharedPtr uvPtr = v_data->vertexBufferBinding->getBuffer(uvElem->getSource());
		dNewtonScopeBuffer<Vector3> uvs (uvPtr->getNumVertices());
		{
			int size = uvPtr->getVertexSize();
			int offset = vertexElem->getOffset() / sizeof (float);
			unsigned char* const ptr = static_cast<unsigned char*> (uvPtr->lock(HardwareBuffer::HBL_READ_ONLY));
			for (int i = 0; i < uvs.GetElementsCount(); i ++) {
				float* data;
				uvElem->baseVertexPointerToElement(ptr + i * size, &data);
				uvs[i] = Vector3 (data[offset + 0], data[offset + 1], 0.0f);
			}
			uvPtr->unlock();
		}

		//now find more about the index!!
		IndexData* const i_data = sub_mesh->indexData;
		size_t index_count = i_data->indexCount;
		size_t poly_count = index_count / 3;

		// get pointer!
		HardwareIndexBufferSharedPtr i_sptr = i_data->indexBuffer;

		// 16 or 32 bit indices?
		bool uses32bit = (i_sptr->getType()	== HardwareIndexBuffer::IT_32BIT);
		unsigned long* i_Longptr = NULL;
		unsigned short* i_Shortptr = NULL;

		if (uses32bit) {
			i_Longptr = static_cast<unsigned long*> (i_sptr->lock(HardwareBuffer::HBL_READ_ONLY));
		} else {
			i_Shortptr = static_cast<unsigned short*> (i_sptr->lock(HardwareBuffer::HBL_READ_ONLY));
		}

		//now loop through the indices, getting polygon info!
		int i_offset = 0;

		for (size_t i = 0; i < poly_count; i++)	{
			Real poly_verts[3][12];
			for (int j = 0; j < 3; j++) {
				// index to first vertex!
				int idx = uses32bit ? i_Longptr[i_offset + j] : i_Shortptr[i_offset + j]; 

				poly_verts[j][0] = points[idx].x;
				poly_verts[j][1] = points[idx].y;
				poly_verts[j][2] = points[idx].z;
				poly_verts[j][3] = 0.0f;

				poly_verts[j][4] = normals[idx].x;
				poly_verts[j][5] = normals[idx].y;
				poly_verts[j][6] = normals[idx].z;

				poly_verts[j][7] = uvs[idx].x;
				poly_verts[j][8] = uvs[idx].y;

				poly_verts[j][9]  = 0.0f;
				poly_verts[j][10] = 0.0f;
			}
			AddFace(3, &poly_verts[0][0], 12 * sizeof (Real), cs);
			i_offset += 3;
		}
	}
}


void OgreNewtonMesh::ApplyTransform (const Vector3& posit, const Vector3& scale, const Quaternion& rotation)
{
	Matrix4 matrix;
	matrix.makeTransform (posit, scale, rotation);
	matrix = matrix.transpose();
	dNewtonMesh::ApplyTransform (&matrix[0][0]);
}