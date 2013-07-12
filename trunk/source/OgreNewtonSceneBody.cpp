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

#include "OgreNewtonSceneBody.h"

/*
class OgreNewtonSceneBody::OgreNewtonCollisionTree: public dNewtonCollisionMesh
{
	public:
	OgreNewtonCollisionTree (OgreNewtonWorld* const world, SceneNode* const startNode, FaceWinding winding = FW_DEFAULT)
		:dNewtonCollisionMesh (world)
	{
		// save the node as the use data for this collision
		SetUserData (startNode);

		Vector3 rootPos (Vector3::ZERO);
		Vector3 rootScale = startNode->getScale();
		Quaternion rootOrient = Quaternion::IDENTITY;

		// parse the entire node adding all static mesh to the collision tree
		BeginFace();
		ParseNode (startNode, rootOrient, rootPos, rootScale, winding);
		EndFace();
	}

	OgreNewtonCollisionTree (const OgreNewtonCollisionTree& src, NewtonCollision* const shape)
		:dNewtonCollisionMesh (src, shape)
	{
	}


	virtual ~OgreNewtonCollisionTree()
	{
	}

	void ParseNode(SceneNode* const node,	const Quaternion &curOrient, const Vector3 &curPos, const Vector3 &curScale, FaceWinding fw)
	{
		// parse this scene node.
		// do children first.
		Quaternion thisOrient = curOrient * node->getOrientation();
		Vector3 thisPos = curPos + (curOrient * (node->getPosition() * curScale));
		Vector3 thisScale = curScale * node->getScale();

		SceneNode::ChildNodeIterator child_it =	node->getChildIterator();
		while (child_it.hasMoreElements()) {
			ParseNode((SceneNode*) child_it.getNext(), thisOrient, thisPos, thisScale, fw);
		}

		// now add the polys from this node.
		//now get the mesh!
		unsigned int num_obj = node->numAttachedObjects();
		for (unsigned int co = 0; co < num_obj; co++) {
			MovableObject* const obj = node->getAttachedObject(short(co));
			if (obj->getMovableType() != "Entity") {
				continue;
			}

			Entity* const ent = (Entity*) obj;

			//		if (!entityFilter(node, ent, fw)) {
			//			continue;
			//		}

			MeshPtr mesh = ent->getMesh();

			//find number of sub-meshes
			unsigned short sub = mesh->getNumSubMeshes();

			for (unsigned short cs = 0; cs < sub; cs++) {

				SubMesh* sub_mesh = mesh->getSubMesh(cs);

				//vertex data!
				VertexData* v_data;

				if (sub_mesh->useSharedVertices) {
					v_data = mesh->sharedVertexData;
				} else {
					v_data = sub_mesh->vertexData;
				}

				//let's find more information about the Vertices...
				VertexDeclaration* const v_decl = v_data->vertexDeclaration;
				const VertexElement* const p_elem = v_decl->findElementBySemantic(VES_POSITION);

				// get pointer!
				HardwareVertexBufferSharedPtr v_sptr = v_data->vertexBufferBinding->getBuffer(p_elem->getSource());
				unsigned char* const v_ptr = static_cast<unsigned char*> (v_sptr->lock(HardwareBuffer::HBL_READ_ONLY));

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
					Vector3 poly_verts[3];
					unsigned char* v_offset;
					float* v_Posptr;
					int idx;

					if (uses32bit) {
						for (int j = 0; j < 3; j++) {
							idx = i_Longptr[i_offset + j]; // index to first vertex!
							v_offset = v_ptr + (idx * v_sptr->getVertexSize());
							p_elem->baseVertexPointerToElement(v_offset, &v_Posptr);
							//now get vertex position from v_Posptr!
							poly_verts[j].x = *v_Posptr;
							v_Posptr++;
							poly_verts[j].y = *v_Posptr;
							v_Posptr++;
							poly_verts[j].z = *v_Posptr;
							v_Posptr++;

							poly_verts[j] = thisPos + (thisOrient * (poly_verts[j] * curScale));
						}
					} else {
						for (int j = 0; j < 3; j++)	{

							idx = i_Shortptr[i_offset + j]; // index to first vertex!
							v_offset = v_ptr + (idx * v_sptr->getVertexSize());
							p_elem->baseVertexPointerToElement(v_offset, &v_Posptr);
							//now get vertex position from v_Posptr!

							// switch poly winding.
							poly_verts[j].x = *v_Posptr;
							v_Posptr++;
							poly_verts[j].y = *v_Posptr;
							v_Posptr++;
							poly_verts[j].z = *v_Posptr;
							v_Posptr++;

							poly_verts[j] = thisPos + (thisOrient * (poly_verts[j] * curScale));
						}
					}


					if (fw == FW_DEFAULT) {
						AddFace(3, &poly_verts[0].x, sizeof(Vector3), cs);
					} else {
						Vector3 rev_poly_verts[3];
						rev_poly_verts[0] = poly_verts[0];
						rev_poly_verts[0] = poly_verts[2];
						rev_poly_verts[0] = poly_verts[1];
						AddFace(3, &rev_poly_verts[0].x, sizeof(Vector3), cs);
					}

					i_offset += 3;
				}

				//unlock the buffers!
				v_sptr->unlock();
				i_sptr->unlock();
			}
		}
	}


	dNewtonCollision* Clone(NewtonCollision* const shape) const
	{
		return new OgreNewtonCollisionTree (*this, shape);
	}
};

*/


OgreNewtonSceneBody::OgreNewtonSceneBody (OgreNewtonWorld* const ogreWorld)
	:OgreNewtonBody ()
{
	Matrix4 matrix (Matrix4::IDENTITY);
	dNewtonCollisionScene collision (ogreWorld);
	SetBody (NewtonCreateDynamicBody (ogreWorld->GetNewton (), collision.GetShape(), matrix[0]));
}

OgreNewtonSceneBody::~OgreNewtonSceneBody()
{
}

void OgreNewtonSceneBody::BeginAddRemoveCollision()
{
	dNewtonCollisionScene* const scene = (dNewtonCollisionScene*) GetCollision();
	scene->BeginAddRemoveCollision();
}

void* OgreNewtonSceneBody::AddCollision(const dNewtonCollision* const collision)
{
	dNewtonCollisionScene* const scene = (dNewtonCollisionScene*) GetCollision();
	return scene->AddCollision(collision);
}

void OgreNewtonSceneBody::RemoveCollision (void* const handle)
{
	dNewtonCollisionScene* const scene = (dNewtonCollisionScene*) GetCollision();
	scene->RemoveCollision(handle);
}

void OgreNewtonSceneBody::EndAddRemoveCollision()
{
	dNewtonCollisionScene* const scene = (dNewtonCollisionScene*) GetCollision();
	scene->EndAddRemoveCollision();

	// need to update the aabb in the broad phase, for this we call set matrix
	dMatrix matrix;
	NewtonBody* const body = GetNewtonBody();
	NewtonBodyGetMatrix(body, &matrix[0][0]);
	NewtonBodySetMatrix(body, &matrix[0][0]);
}


void* OgreNewtonSceneBody::AddCollisionTree (SceneNode* const treeNode)
{
	OgreNewtonWorld* const world = (OgreNewtonWorld*) GetNewton();

	// convert the nod and all its children to a newton mesh
	OgreNewtonMesh mesh (world);
	mesh.BuildFromSceneNode (treeNode);
	mesh.Polygonize();

	// create a collision tree mesh
	dNewtonCollisionMesh meshCollision (world, mesh, 0);

//	OgreNewtonCollisionTree meshCollision (world, treeNode);
	// add this collision to the scene body
	return AddCollision (&meshCollision);
}





