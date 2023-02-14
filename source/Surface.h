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

		inline static bool Refract(const Vec3f& v, const Vec3f& n, float ni_over_nt, Vec3f& refracted)
		{
			Vec3f vn = MakeUnit(v);
			float vndotn = Dot(vn, n);
			float cos_theta_t_square = 1.0f - ni_over_nt * ni_over_nt * (1.0f - vndotn * vndotn);

			if (cos_theta_t_square > 0.0f)
			{
				refracted = ni_over_nt * (v - n * vndotn) - n * std::sqrt(cos_theta_t_square);
				return true;
            }

			return false;
		}

		inline static float Fresnel(Vec3f const& n, Vec3f const& dir, float etaIn, float etaOut)
		{
			Vec3f norm = n;
			float ndotdir = Dot(norm, dir);
			float eta;

			if (ndotdir < 0.0f) {
				norm = -norm;
				eta = etaOut / etaIn;
			}
			else {
				eta = etaIn / etaOut;
			}

			float cos_theta_i = Dot(norm, dir);
			float cos_theta_t = std::sqrt(1.0f - (1.0f - cos_theta_i * cos_theta_i) / (eta * eta));
			float r_parallel = (eta * cos_theta_i - cos_theta_t) / (eta * cos_theta_i + cos_theta_t);
			float r_pependicular = (cos_theta_i - eta * cos_theta_t) / (cos_theta_i + eta * cos_theta_t);
			float kr = 0.5f * (r_parallel * r_parallel + r_pependicular * r_pependicular);

			return kr;
		}

		inline static float Schlick(float consine, float ref_idx)
        {
            float r0 = (1 - ref_idx) / (1 + ref_idx);
            r0 = r0 * r0;
            float value = r0 + (1 - r0) * std::pow((1 - consine), 5);
            return value;
        }

		inline static Color3f SchlickFresnel(Color3f const& rs, float costheta) {
			return rs + std::pow(1 - costheta, 5.0f) * (WHITE - rs);
		}
	};

	class ShadingCoord {
	public:
		ShadingCoord(Vec3f const& dpdu, Vec3f const& dpdv) {
			ns = Cross(dpdu, dpdv);
			ns.MakeUnit();

			ss = dpdu;
			ss.MakeUnit();

			ts = Cross(ns, ss);
		}

	public:
		inline Vec3f WorldToLocal(const Vec3f &v) const {
			return Vec3f(Dot(v, ss), Dot(v, ts), Dot(v, ns));
		}

		inline Vec3f LocalToWorld(const Vec3f &v) const {
			return Vec3f(ss.X() * v.X() + ts.X() * v.Y() + ns.X() * v.Z(),
				ss.Y() * v.X() + ts.Y() * v.Y() + ns.Y() * v.Z(),
				ss.Z() * v.X() + ts.Z() * v.Y() + ns.Z() * v.Z());
		}

	private:
		Vec3f ss;
		Vec3f ts;
		Vec3f ns;

	};
}
