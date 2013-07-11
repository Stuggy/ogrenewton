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

void OgreNewtonMesh::BuildFromSceneNode(SceneNode* const sceneNode)
{
	Vector3 rootPos (Vector3::ZERO);
	Vector3 rootScale = sceneNode->getScale();
	Quaternion rootOrient = Quaternion::IDENTITY;

	// parse the entire node adding all static mesh to the collision tree
//	BeginFace();
//	ParseNode (startNode, rootOrient, rootPos, rootScale, winding);
//	EndFace();
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
			if (obj->getMovableType() != "Entity") {
				continue;
			}

			Entity* const ent = (Entity*) obj;

			//if (!entityFilter(node, ent, fw)) {
			//		continue;
			//}

			MeshPtr mesh = ent->getMesh();

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
				const VertexElement* const p_vertexElem = v_decl->findElementBySemantic(VES_POSITION);
				const VertexElement* const p_normalElem = v_decl->findElementBySemantic(VES_NORMAL);
				const VertexElement* const p_uvElem = v_decl->findElementBySemantic(VES_TEXTURE_COORDINATES);

				// get pointer!
				HardwareVertexBufferSharedPtr v_sptr = v_data->vertexBufferBinding->getBuffer(p_vertexElem->getSource());
				unsigned char* const v_ptr = static_cast<unsigned char*> (v_sptr->lock(HardwareBuffer::HBL_READ_ONLY));

				HardwareVertexBufferSharedPtr n_sptr = v_data->vertexBufferBinding->getBuffer(p_normalElem->getSource());
				unsigned char* const n_ptr = static_cast<unsigned char*> (n_sptr->lock(HardwareBuffer::HBL_READ_ONLY));

				HardwareVertexBufferSharedPtr uv_sptr = v_data->vertexBufferBinding->getBuffer(p_uvElem->getSource());
				unsigned char* const uv_ptr = static_cast<unsigned char*> (uv_sptr->lock(HardwareBuffer::HBL_READ_ONLY));


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
					if (uses32bit) {
						for (int j = 0; j < 3; j++) {
							// index to first vertex!
							int idx = i_Longptr[i_offset + j]; 

							// get the vertex position
							float* v_Posptr;
							unsigned char* v_offset = v_ptr + (idx * v_sptr->getVertexSize());
							p_vertexElem->baseVertexPointerToElement(v_offset, &v_Posptr);
							Vector3 p (v_Posptr[0], v_Posptr[1], v_Posptr[2]);
							p = thisPos + (thisOrient * (p * curScale));

							// get the normal position
							float* n_Posptr;
							unsigned char* n_offset = n_ptr + (idx * n_sptr->getVertexSize());
							p_normalElem->baseVertexPointerToElement(n_offset, &n_Posptr);
							Vector3 n (n_Posptr[0], n_Posptr[1], n_Posptr[2]);
							n = thisOrient * (n * curScale);

							// get the uv0 position
							float* uv_Posptr;
							unsigned char* uv_offset = uv_ptr + (idx * uv_sptr->getVertexSize());
							p_uvElem->baseVertexPointerToElement(uv_offset, &uv_Posptr);
							Vector3 uv (uv_Posptr[0], uv_Posptr[1], 0.0f);

							poly_verts[j][0] = p.x;
							poly_verts[j][1] = p.y;
							poly_verts[j][2] = p.z;
							poly_verts[j][3] = 0.0f;

							poly_verts[j][4] = n.x;
							poly_verts[j][5] = n.y;
							poly_verts[j][6] = n.z;

							poly_verts[j][7] = uv.x;
							poly_verts[j][8] = uv.y;

							poly_verts[j][9]  = 0.0f;
							poly_verts[j][10] = 0.0f;
						}
						AddFace(3, &poly_verts[0][0], 12 * sizeof (Real), cs);

					} else {
						for (int j = 0; j < 3; j++)	{
							dAssert (0);
/*
							idx = i_Shortptr[i_offset + j]; // index to first vertex!
							v_offset = v_ptr + (idx * v_sptr->getVertexSize());
							p_vertexElem->baseVertexPointerToElement(v_offset, &v_Posptr);
							//now get vertex position from v_Posptr!

							// switch poly winding.
							poly_verts[j].x = *v_Posptr;
							v_Posptr++;
							poly_verts[j].y = *v_Posptr;
							v_Posptr++;
							poly_verts[j].z = *v_Posptr;
							v_Posptr++;

							poly_verts[j] = thisPos + (thisOrient * (poly_verts[j] * curScale));
*/
						}
					}

//					AddFace(3, &poly_verts[0].x, sizeof(Vector3), cs);

					i_offset += 3;
				}

				//unlock the buffers!
				v_sptr->unlock();
				n_sptr->unlock();
				uv_sptr->unlock();
				i_sptr->unlock();
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

//	AddFace (int vertexCount, const dFloat* const vertex, int strideInBytes, int materialIndex);
	EndPolygon();
}