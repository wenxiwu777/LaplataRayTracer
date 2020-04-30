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
		Vec3f	pt;
		Vec3f	n;
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
		Color3f alebdo;
		PDF *pPDF;

	};
}
