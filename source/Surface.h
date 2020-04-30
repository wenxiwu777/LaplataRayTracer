#pragma once

#include "Vec3.h"

namespace LaplataRayTracer
{
	class Surface
	{
	public:
		Surface() { }
		~Surface() { }

	public:
		inline static Vec3f Reflect(Vec3f const& v, Vec3f const& n)
		{
			return (v - 2 * (Dot(v, n)*n));
		}

		inline static bool Refract(const Vec3f& v, const Vec3f& n, float ni_over_nt, Vec3f& refacted)
		{
			Vec3f vn = MakeUnit(v);
			float vndotn = Dot(vn, n);
			float cos_theta_t_square = 1.0f - ni_over_nt * ni_over_nt * (1.0f - vndotn * vndotn);

			if (cos_theta_t_square > 0.0f)
			{
				refacted = ni_over_nt * (v - n * vndotn) - n * std::sqrt(cos_theta_t_square);
				return true;
			}

			return false;
		}

	};
}