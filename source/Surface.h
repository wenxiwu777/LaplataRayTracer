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

		inline static float FresnelDielectric(Vec3f const& n, Vec3f const& dir, float etaIn, float etaOut)
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

        inline static Color3f FresnelConductor(const Color3f& real, const Color3f& imagnary, float n1, float cos_theta)
        {
            float cos_theta2 = std::pow(cos_theta, 2.0f);
            float sin_theta2 = 1.0f - cos_theta2;

            Color3f temp = real / n1;
            Vec3f  eta2 = Vec3f(std::pow(temp.X(), 2.0f), std::pow(temp.Y(), 2.0f), std::pow(temp.Z(), 2.0f));
            Color3f  temp2 = imagnary / n1;
            Vec3f  eta_k2 = Vec3f(std::pow(temp2.X(), 2.0f), std::pow(temp2.Y(), 2.0f), std::pow(temp2.Z(), 2.0f));
            Vec3f t0 = eta2 - eta_k2 - sin_theta2;
            Vec3f  temp3 = 4.0f * eta2 * eta_k2;
            Vec3f  temp4 = Vec3f(std::pow(t0.X(), 2.0f), std::pow(t0.Y(), 2.0f), std::pow(t0.Z(), 2.0f));
            Vec3f temp5 = temp4 + temp3;
            Vec3f  a2_p_b2 = Vec3f(std::sqrt(temp5.X()), std::sqrt(temp5.Y()), std::sqrt(temp5.Z()));
            Vec3f t1 = a2_p_b2 + cos_theta2;

            Vec3f temp6 = 0.5f * (a2_p_b2 + t0);
            Vec3f temp7 = Vec3f(std::sqrt(temp6.X()), std::sqrt(temp6.Y()), std::sqrt(temp6.Z()));
            Vec3f t2 = 2.0 * cos_theta * temp7;
            Vec3f r_perpendicual = (t1 - t2) / (t1 + t2);

            Vec3f t3 = cos_theta2 * a2_p_b2 + std::pow(sin_theta2, 2.0f);
            Vec3f t4 = t2 * sin_theta2;
            Vec3f r_parallel = r_perpendicual * (t3 - t4) / (t3 + t4);

            return (r_parallel + r_perpendicual) * 0.5f;
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
