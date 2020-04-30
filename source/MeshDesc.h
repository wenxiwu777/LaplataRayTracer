#pragma once

#include "Common.h"
#include "Vec3.h"

namespace LaplataRayTracer
{
	typedef struct TriFace_t
	{
		int index0, index1, index2;
	} TriFace;

	// Currently only triangle-based
	typedef struct MeshDesc_t
	{
		typedef vector<int> FaceListPerVertex;

		MeshDesc_t()
		{
			mesh_vertex_count = 0U;
			mesh_face_count = 0U;
			mesh_support_uv = 0;
		}

		vector<Vec3f>	mesh_vertices;
		vector<float>	mesh_texU;
		vector<float>	mesh_texV;
		vector<Vec3f>	mesh_normal; // This one is special, we will postpone to set up it in some kind of UpdateNormals via MeshObject
		vector<FaceListPerVertex>	mesh_vertex_faces;
		// This one is uesed when the desc is filled up by user themselves instead of via reading mesh file 
		// and aother case is that when reading a mesh file but the reading process callback interface pointer
		// is not set, then all the face datas will be filled up in this member.
		// Which means that in the default case, reading process callback will fire an event when it reads
		// a line of face data, so the response is responsible for holding the face data and store them in some place.
		vector<TriFace>	mesh_face_datas;

		unsigned long   mesh_vertex_count;
		unsigned long	mesh_face_count;

		int		mesh_support_uv;

	} MeshDesc;
	
}
