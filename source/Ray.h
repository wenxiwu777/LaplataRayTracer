#pragma once

#include "Vec3.h"

namespace LaplataRayTracer
{
	class Ray
	{
	public:
		Ray() { }
		Ray(Vec3f const& o, Vec3f const& d, float t)
			: mo(o), md(d), mt(t)
		{

		}
		~Ray() { }

		inline Vec3f At(float t)
		{
			Vec3f v = md;
			v *= t;
			return (mo + v);
		}

		inline void Set(Vec3f const& o, Vec3f const& d, const float t = 0.0f)
		{
			mo = o;
			md = d;
			mt = t;
		}

		inline Vec3f O() const { return mo; }
		inline Vec3f D() const { return md; }
		inline float T() const { return mt; }

	private:
		Vec3f mo;
		Vec3f md;
		float mt;

	};
}
