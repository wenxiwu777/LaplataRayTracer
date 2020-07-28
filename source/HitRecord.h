#pragma once

#include "Vec3.h"
#include "Ray.h"
#include "PDF.h"

namespace LaplataRayTracer
{
	class Material;

	//
	class HitRecord
	{
	public:
		bool	hit;
		float	t;
		Vec3f   wpt; // world hit point
		Vec3f	pt;  // local hit point
		Vec3f	n;
//		Vec3f	dpdu; // Only for testing, shall be removed later.
//		Vec3f	dpdv; // As above.
		float	u;
		float	v;
		Color3f albedo;
		Material *pMaterial;

		//
		// ... ...

	public:
		HitRecord() {
			hit = false;
			t = -1.0f;
			u = 0.0f;
			v = 0.0f;
			pMaterial = nullptr;

		}

	};

	//
	class ScatterRecord {
	public:
		Ray specular_ray;
		bool is_specular;
		Color3f albedo;
		PDF *pPDF;

	};

	//
	class WhittedRecord {
	public:
		int feature;
		Vec3f wr;
		Vec3f wt;
		Color3f fr;
		Color3f ft;
		Color3f cf1;
		Color3f cf2;

	};

	//
	class GlobalPathRecord {
	public:
		int depth;
		float pdf;
		Color3f albedo;
		Vec3f reflected_dir;
		int mat_type;
//		Color3f  emissive_albedo;
	};
}
