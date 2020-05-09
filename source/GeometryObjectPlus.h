#pragma once

#include "GeometricObject.h"

namespace LaplataRayTracer
{
	//
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
						tmax = roots[i];

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

	//
	class JoinBlendCylinder : public GeometricObject
	{
	public:
		JoinBlendCylinder()
			: mCenter1(0.0f, 3.0f, 0.0f), mCenter2(0.0f, 3.0f, 0.0f), mA1(1.0f), mB1(1.0f), mA2(1.0f), mB2(1.0f), mA3(0.5f), mB3(0.5f), mHalfWidth(3.0f), mHalfHeight(3.0f), mColor(1.0f, 1.0f, 0.0f)
		{

		}

		JoinBlendCylinder(Vec3f const& center1, Vec3f const& center2, float a1, float b1, float a2, float b2, float a3, float b3, float halfWidth, float halfHeight, const Color3f& color = Color3f(1.0f, 0.0f, 0.0f))
			: mCenter1(center1), mCenter2(center2), mA1(a1), mB1(b1), mA2(a2), mB2(b2), mA3(a3), mB3(b3), mHalfWidth(halfWidth), mHalfHeight(halfHeight), mColor(color)
		{

		}

		virtual ~JoinBlendCylinder()
		{

		}

	public:
		virtual void *Clone()
		{
			return (JoinBlendCylinder *)(new JoinBlendCylinder(*this));
		}

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
		{
			//
			Vec3f vpos1 = inRay.O() - mCenter1;
			Vec3f vpos2 = inRay.O() - mCenter2;

			float x0 = vpos1.X();
			float y0 = vpos1.Y();
			float z0 = vpos1.Z();
			float x1 = vpos2.X();
			float y1 = vpos2.Y();
			float z1 = vpos2.Z();
			float dx = inRay.D().X();
			float dy = inRay.D().Y();
			float dz = inRay.D().Z();

			//
			float A1 = mB1*mB1*dx*dx + mA1*mA1*dz*dz;
			float B1 = 2.0f*mB1*mB1*dx*x0 + 2.0f*mA1*mA1*dz*z0;
			float C1 = mB1*mB1*x0*x0 + mA1*mA1*z0*z0;
			
			float A2 = A1*A1*mB3*mB3;
			float B2 = 2.0f*A1*B1*mB3*mB3;
			float C2 = (B1*B1 + 2.0f*A1*C1 - 2.0f*(mA1*mA1*mB1*mB1 + mA3)*A1)*mB3*mB3;
			float D2 = (2.0f*B1*C1 - 2.0f*(mA1*mA1*mB1*mB1 + mA3)*B1)*mB3*mB3;
			float E2 = (C1*C1 - 2.0f*(mA1*mA1*mB1*mB1 + mA3)*C1 + (mA1*mA1*mB1*mB1 + mA3)*(mA1*mA1*mB1*mB1 + mA3))*mB3*mB3;

			float A3 = mB2*mB2*dy*dy + mA2*mA2*dz*dz;
			float B3 = 2.0f*mB2*mB2*dy*y1 + 2.0f*mA2*mA2*dz*z1;
			float C3 = mB2*mB2*y1*y1 + mA2*mA2*z1*z1;

			float A4 = A3*A3*mA3*mA3;
			float B4 = 2.0f*A3*B3*mA3*mA3;
			float C4 = (B3*B3 + 2.0f*A3*C3 - 2.0f*(mA2*mA2*mB2*mB2 + mB3)*A3)*mA3*mA3;
			float D4 = (2.0f*B3*C3 - 2.0f*(mA2*mA2*mB2*mB2 + mB3)*B3)*mA3*mA3;
			float E4 = (C3*C3 - 2.0f*(mA2*mA2*mB2*mB2 + mB3)*C3 + (mA2*mA2*mB2*mB2 + mB3)*(mA2*mA2*mB2*mB2 + mB3))*mA3*mA3 - mA3*mA3*mB3*mB3;

			//
			float coefficients[5] = { 0.0f };
			coefficients[0] = E2 + E4;
			coefficients[1] = D2 + D4;
			coefficients[2] = C2 + C4;
			coefficients[3] = B2 + B4;
			coefficients[4] = A2 + A4;
			float roots[4] = { 0.0f };
			int nRootNum = RTMath::SolveQuartic(coefficients, roots);

			if (nRootNum < 0)
			{
				return false;
			}

			float temp = FLT_MAX;
			for (int i = 0; i < nRootNum; ++i)
			{
				for (int j = i + 1; j < nRootNum; ++j)
				{
					if (roots[j] < roots[i])
					{
						temp = roots[j];
						roots[j] = roots[i];
						roots[i] = temp;
					}
				}
			}

			bool is_hit = false;
			for (int i = 0; i < nRootNum; ++i)
			{
				if ((roots[i] > tmin + KEpsilon()) && (roots[i] < tmax))
				{
					Vec3f pt = inRay.O() + roots[i] * inRay.D();

					Vec3f pt1 = pt - mCenter1;
					Vec3f pt2 = pt - mCenter2;
					bool in_x_range = (pt2.X() >= -mHalfWidth) && (pt2.X() <= mHalfWidth);
					bool in_y_range = (pt1.Y() >= -mHalfHeight) && (pt1.Y() <= mHalfHeight);
					if (in_x_range && in_y_range)
					{
						tmax = roots[i];

						is_hit = true;

						rec.hit = true;
						rec.t = roots[i];
						rec.pt = pt;
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
			//
			Vec3f vpos1 = inRay.O() - mCenter1;
			Vec3f vpos2 = inRay.O() - mCenter2;

			float x0 = vpos1.X();
			float y0 = vpos1.Y();
			float z0 = vpos1.Z();
			float x1 = vpos2.X();
			float y1 = vpos2.Y();
			float z1 = vpos2.Z();
			float dx = inRay.D().X();
			float dy = inRay.D().Y();
			float dz = inRay.D().Z();

			//
			float A1 = mB1*mB1*dx*dx + mA1*mA1*dz*dz;
			float B1 = 2.0f*mB1*mB1*dx*x0 + 2.0f*mA1*mA1*dz*z0;
			float C1 = mB1*mB1*x0*x0 + mA1*mA1*z0*z0;

			float A2 = A1*A1*mB3*mB3;
			float B2 = 2.0f*A1*B1*mB3*mB3;
			float C2 = (B1*B1 + 2.0f*A1*C1 - 2.0f*(mA1*mA1*mB1*mB1 + mA3)*A1)*mB3*mB3;
			float D2 = (2.0f*B1*C1 - 2.0f*(mA1*mA1*mB1*mB1 + mA3)*B1)*mB3*mB3;
			float E2 = (C1*C1 - 2.0f*(mA1*mA1*mB1*mB1 + mA3)*C1 + (mA1*mA1*mB1*mB1 + mA3)*(mA1*mA1*mB1*mB1 + mA3))*mB3*mB3;

			float A3 = mB2*mB2*dy*dy + mA2*mA2*dz*dz;
			float B3 = 2.0f*mB2*mB2*dy*y1 + 2.0f*mA2*mA2*dz*z1;
			float C3 = mB2*mB2*y1*y1 + mA2*mA2*z1*z1;

			float A4 = A3*A3*mA3*mA3;
			float B4 = 2.0f*A3*B3*mA3*mA3;
			float C4 = (B3*B3 + 2.0f*A3*C3 - 2.0f*(mA2*mA2*mB2*mB2 + mB3)*A3)*mA3*mA3;
			float D4 = (2.0f*B3*C3 - 2.0f*(mA2*mA2*mB2*mB2 + mB3)*B3)*mA3*mA3;
			float E4 = (C3*C3 - 2.0f*(mA2*mA2*mB2*mB2 + mB3)*C3 + (mA2*mA2*mB2*mB2 + mB3)*(mA2*mA2*mB2*mB2 + mB3))*mA3*mA3 - mA3*mA3*mB3*mB3;

			//
			float coefficients[5] = { 0.0f };
			coefficients[0] = E2 + E4;
			coefficients[1] = D2 + D4;
			coefficients[2] = C2 + C4;
			coefficients[3] = B2 + B4;
			coefficients[4] = A2 + A4;
			float roots[4] = { 0.0f };
			int nRootNum = RTMath::SolveQuartic(coefficients, roots);

			if (nRootNum < 0)
			{
				return false;
			}

			float temp = FLT_MAX;
			for (int i = 0; i < nRootNum; ++i)
			{
				for (int j = i + 1; j < nRootNum; ++j)
				{
					if (roots[j] < roots[i])
					{
						temp = roots[j];
						roots[j] = roots[i];
						roots[i] = temp;
					}
				}
			}

			bool is_hit = false;
			for (int i = 0; i < nRootNum; ++i)
			{
				if (roots[i] > KEpsilon())
				{
					Vec3f pt = inRay.O() + roots[i] * inRay.D();

					Vec3f pt1 = pt - mCenter1;
					Vec3f pt2 = pt - mCenter2;
					bool in_x_range = (pt2.X() >= -mHalfWidth) && (pt2.X() <= mHalfWidth);
					bool in_y_range = (pt1.Y() >= -mHalfHeight) && (pt1.Y() <= mHalfHeight);
					if (in_x_range && in_y_range)
					{
						tvalue = roots[i];
						is_hit = true;
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
			Vec3f vpos1 = rec.pt - mCenter1;
			Vec3f vpos2 = rec.pt - mCenter2;

			float x0 = vpos1.X();
			float y0 = vpos1.Y();
			float z0 = vpos1.Z();
			float x1 = vpos2.X();
			float y1 = vpos2.Y();
			float z1 = vpos2.Z();

			Vec3f norm;

			float nx = 2.0f*mB3*mB3*(mB1*mB1*x0*x0 + mA1*mA1*z0*z0 - (mA1*mA1*mB1*mB1 + mA3))*(2.0f*mB1*mB1*x0*x0);
			float ny = 2.0f*mA3*mA3*(mB2*mB2*y1*y1 + mA2*mA2*z1*z1 - (mA2*mA2*mB2*mB2 + mB3))*(2.0f*mB2*mB2*y1*y1);
			float nz = 2.0f*mB3*mB3*(mB1*mB1*x0*x0 + mA1*mA1*z0*z0 - (mA1*mA1*mB1*mB1 + mA3))*(2.0f*mA1*mA1*z0*z0)
				+ 2.0f*mA3*mA3*(mB2*mB2*y1*y1 + mA2*mA2*z1*z1 - (mA2*mA2*mB2*mB2 + mB3))*(2.0f*mA2*mA2*z1*z1);
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
		inline void SetCenter(Vec3f const& center1, Vec3f const& center2)
		{
			mCenter1 = center1;
			mCenter2 = center2;
		}

		inline void SetParams(float a1, float b1, float a2, float b2, float a3, float b3)
		{
			mA1 = a1;
			mB1 = b1;
			mA2 = a2;
			mB2 = b2;
			mA3 = a3;
			mB3 = b3;
		}

		inline void SetHWRange(float halfWidth, float halfHeight)
		{
			mHalfWidth = halfWidth;
			mHalfHeight = halfHeight;
		}

		inline void SetColor(Color3f const& color)
		{
			mColor = color;
		}

	public:
		inline static float KEpsilon() { return 0.001f; }

	private:
		Vec3f mCenter1;
		Vec3f mCenter2;
		float mA1;
		float mB1;
		float mA2;
		float mB2;
		float mA3;
		float mB3;

		float mHalfWidth;
		float mHalfHeight;

		Color3f mColor;

	};

}
