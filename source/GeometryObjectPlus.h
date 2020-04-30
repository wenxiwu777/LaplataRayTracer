#pragma once

#include "GeometricObject.h"

namespace LaplataRayTracer
{
	class Teardrop : public GeometricObject
	{
	public:
		Teardrop()
			: mCenter(Vec3f(0.0f, 0.0f, 0.0f)), mA1(2.0f), mA2(2.0f), mA3(2.0f), mColor(1.0f, 0.0f, 0.0f)
		{
			this->calc_ABCDE();
		}

		Teardrop(Vec3f const& center, float a1, float a2, float a3, const Color3f& color = Color3f(1.0f, 0.0f, 0.0f))
			: mCenter(center), mA1(a1), mA2(a2), mA3(a3), mColor(color)
		{
			this->calc_ABCDE();
		}

		virtual ~Teardrop()
		{

		}

	public:
		virtual void *Clone() { return (Teardrop *)(new Teardrop(*this)); }

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
		{
			Vec3f OC = inRay.O() - mCenter;

			float x0 = OC.X(); 
			float y0 = OC.Y(); 
			float z0 = OC.Z();
			float dx = inRay.D().X(); 
			float dy = inRay.D().Y(); 
			float dz = inRay.D().Z();

			//
			float coefficients[5] = { 0.0f };
			float roots[4] = { 0.0f };
			int num_real_roots = 0;

			coefficients[4] = dy*dy*dy*dy*mTempC;
			coefficients[3] = 4.0f*y0*dy*dy*dy*mTempC + dy*dy*dy*mTempD;
			coefficients[2] = 6.0f*y0*y0*dy*dy*mTempC + 3.0f*y0*dy*dy*mTempD + dz*dz*mTempB + dx*dx*mTempA;
			coefficients[1] = 4.0f*y0*y0*y0*dy*mTempC + 3.0f*y0*y0*dy*mTempD + 2.0f*z0*dz*mTempB + 2.0f*x0*dx*mTempA + dy*mTempE;
			coefficients[0] = y0*y0*y0*y0*mTempC + y0*y0*y0*mTempD + y0*mTempE + mTempF + z0*z0*mTempB + x0*x0*mTempA;

			num_real_roots = RTMath::SolveQuartic(coefficients, roots);
			bool is_hit = false;
			if (num_real_roots == 0)
			{
				return false;
			}
			else
			{
		//		g_Console.Write("solved\n");
				// sort the answers
				float temp = FLT_MAX;
				for (int i = 0; i < num_real_roots; ++i)
				{
					for (int j = i + 1; j < num_real_roots; ++j)
					{
						if (roots[j] < roots[i])
						{
							temp = roots[j];
							roots[j] = roots[i];
							roots[i] = temp;
						}
					}
				}

				for (int i = 0; i < num_real_roots; ++i)
				{
					if ((roots[i] > tmin + KEpsilon()) && (roots[i] < tmax))
					{
						is_hit = true;
	//					g_Console.Write("hit\n");
						rec.hit = true;
						rec.t = roots[i];
						rec.pt = inRay.O() + roots[i] * inRay.D();
						rec.n = GetNormal(rec);
						rec.albedo = mColor;
						rec.pMaterial = nullptr;

						break;
					}
				}
			}

			return is_hit;
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const
		{
			Vec3f OC = inRay.O() - mCenter;

			float x0 = OC.X();
			float y0 = OC.Y();
			float z0 = OC.Z();
			float dx = inRay.D().X();
			float dy = inRay.D().Y();
			float dz = inRay.D().Z();

			//
			float coefficients[5] = { 0.0f };
			float roots[4] = { 0.0f };
			int num_real_roots = 0;

			coefficients[4] = dy*dy*dy*dy*mTempC;
			coefficients[3] = 4.0f*y0*dy*dy*dy*mTempC + dy*dy*dy*mTempD;
			coefficients[2] = 6.0f*y0*y0*dy*dy*mTempC + 3.0f*y0*dy*dy*mTempD + dz*dz*mTempB + dx*dx*mTempA;
			coefficients[1] = 4.0f*y0*y0*y0*dy*mTempC + 3.0f*y0*y0*dy*mTempD + 2.0f*z0*dz*mTempB + 2.0f*x0*dx*mTempA + dy*mTempE;
			coefficients[0] = y0*y0*y0*y0*mTempC + y0*y0*y0*mTempD + y0*mTempE + mTempF + z0*z0*mTempB + x0*x0*mTempA;

			num_real_roots = RTMath::SolveQuartic(coefficients, roots);
			bool is_hit = false;
			if (num_real_roots == 0)
			{
				return false;
			}
			else
			{
				// sort the answers
				float temp = FLT_MAX;
				for (int i = 0; i < num_real_roots; ++i)
				{
					for (int j = i + 1; j < num_real_roots; ++j)
					{
						if (roots[j] < roots[i])
						{
							temp = roots[j];
							roots[j] = roots[i];
							roots[i] = temp;
						}
					}
				}

				for (int i = 0; i < num_real_roots; ++i)
				{
					if (roots[i] > KEpsilon())
					{
						is_hit = true;
						tvalue = roots[i];

						break;
					}
				}
			}

			return is_hit;
		}

	public:
		virtual float Area() const
		{
			return 0.0f;
		}

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding)
		{
			return false;
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const
		{
			Vec3f norm;

			float nx = 2.0f * mTempA * rec.pt.X();
			float ny = 4.0f * mTempC * rec.pt.Y() * rec.pt.Y() * rec.pt.Y() + 3.0f * mTempD * rec.pt.Y() * rec.pt.Y() + mTempE;
			float nz = 2.0f * mTempB * rec.pt.Z();
			norm.Set(nx, ny, nz);
			norm.MakeUnit();

			return norm;
		}

		virtual Vec3f RandomSamplePoint() const
		{
			return Vec3f(0.0f, 0.0f, 0.0f);
		}

		virtual void Update(float t)
		{

		}

		virtual bool IsCompound() const
		{
			return false;
		}

		virtual float PDFValue(Vec3f const& o, Vec3f const& v) const
		{
			return 0.0f;
		}

		virtual Vec3f RandomSampleDirection(Vec3f const& v) const
		{
			return Vec3f(0.0f, 0.0f, 0.0f);
		}

	public:
		inline void SetCenter(Vec3f const& center) { mCenter = center; }
		inline void SetParam_A1A2A3(float a1, float a2, float a3)
		{
			mA1 = a1;
			mA2 = a2;
			mA3 = a3;
			this->calc_ABCDE();
		}
		inline void SetColor(Color3f const& color) { mColor = color; }

	public:
		inline static float KEpsilon() { return 0.001f; }

	private:
		inline void calc_ABCDE()
		{
			mTempA = 4.0f * mA3 * mA3 * mA2 * mA2 * mA2 * mA2;
			mTempB = 4.0f * mA1 * mA1 * mA2 * mA2 * mA2 * mA2;
			mTempC = mA1 * mA1 * mA3 * mA3;
			mTempD = -2.0f * mA1 * mA1 * mA3 * mA3 * mA2;
			mTempE = 2.0f * mA1 * mA1 * mA3 * mA3 * mA2 * mA2 * mA2;
			mTempF = -mA1 * mA1 * mA3 * mA3 * mA2 * mA2 * mA2 * mA2;

		}

	private:
		Vec3f mCenter;
		float mA1;
		float mA2;
		float mA3;
		Color3f mColor;

		float mTempA;
		float mTempB;
		float mTempC;
		float mTempD;
		float mTempE;
		float mTempF;

	};

}
