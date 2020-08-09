#pragma once
#include <iostream>
#include "GeometricObject.h"
//#include "ShadeObject.h"

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
		    Vec3f pt = rec.pt - mCenter;

			Vec3f norm;

			float nx = 2.0f * mTempA * pt.X();
			float ny = 4.0f * mTempC * pt.Y() * pt.Y() * pt.Y() + 3.0f * mTempD * pt.Y() * pt.Y() + mTempE;
			float nz = 2.0f * mTempB * pt.Z();
			norm.Set(nx, ny, nz);
			norm.MakeUnit();

			return norm;
		}

        virtual Vec3f SampleRandomPoint() const
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

        virtual Vec3f SampleRandomDirection(Vec3f const& v) const
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

        virtual Vec3f SampleRandomPoint() const
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

        virtual Vec3f SampleRandomDirection(Vec3f const& v) const
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

	// Rotational sweeping
    class RotationalSweeping : public GeometricObject {
    public:
        static const int MAT_DIM = 4;
        static const int CP_COUNT = 6;

    public:
        RotationalSweeping() : mCenter(0.0f, 0.0f, 0.0f), mColor(1.0f, 0.0f, 0.0f) {
            // NOTE: those control points can be enlarged according to real cases, like X 10 times.
            float controlPoints[CP_COUNT][2] = {
                    {-1.0,  5.0}, {2.0,  4.0}, {2.0,  1.0},
                    {-0.5,  1.0}, {1.5, -3.0}, {3.0,  0.0}
            };
            this->update(controlPoints);
        }

        RotationalSweeping(Vec3f const& center, Color3f const& color, float controlPoints[CP_COUNT][2]) {
            mCenter = center;
            mColor = color;
            this->update(controlPoints);
        }

        virtual ~RotationalSweeping() {

        }

    public:
        virtual void *Clone() {
            return (void *)(new RotationalSweeping(*this));
        }

    public:
        virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec) {
            if (!mBBox.HitTest(inRay)) {
                return false;
            }

            Vec3f p = inRay.O() - mCenter;
            float x0 = p.X();
            float y0 = p.Y();
            float z0 = p.Z();
            float dx = inRay.D().X();
            float dy = inRay.D().Y();
            float dz = inRay.D().Z();

//            if (std::fabs(dx) < 1e-3) {
//                dx = 1e-3;
//            }
//            if (std::fabs(dy) < 1e-3) {
//                dy = 1e-3;
//            }
//            if (std::fabs(dz) < 1e-3) {
//                dz = 1e-3;
//            }

            float a = dx*dx+dz*dz;
            float b = 2.0f*((x0*dx+z0*dz)*dy-a*y0);
            float c = (x0*dy-dx*y0)*(x0*dy-dx*y0)+(z0*dy-dz*y0)*(z0*dy-dz*y0);
            float d = dy*dy;

            float coeff[7];
            float roots[6];
            int total_roots_num = 0;
            // because each b-spline may have up to 6 roots, so the size of 1-d is 18
            // 2-d[0] is t value in HitRecord, 2-d[1] is index of 3 b-spline, 2-d[2] is root of s itself.
            float total_roots[18][3];
            float y_value;
            // now we find roots For each b-spline
            for (int i = 0; i < 3; ++i) {
                coeff[0] = a*mCV[3][i]*mCV[3][i]-d*mCU[3][i]*mCU[3][i];
                coeff[1] = a*2.0f*mCV[2][i]*mCV[3][i]-d*2.0f*mCU[2][i]*mCU[3][i];
                coeff[2] = a*(mCV[2][i]*mCV[2][i]+2.0f*mCV[1][i]*mCV[3][i])-d*(mCU[2][i]*mCU[2][i]+2.0f*mCU[1][i]*mCU[3][i]);
                coeff[3] = a*2.0f*(mCV[0][i]*mCV[3][i]+mCV[1][i]*mCV[2][i])+b*mCV[3][i]-d*2.0f*(mCU[0][i]*mCU[3][i]+mCU[1][i]*mCU[2][i]);
                coeff[4] = a*(mCV[1][i]*mCV[1][i]+2.0f*mCV[0][i]*mCV[2][i])+b*mCV[2][i]-d*(mCU[1][i]*mCU[1][i]+2.0f*mCU[0][i]*mCU[2][i]);
                coeff[5] = a*2.0f*mCV[0][i]*mCV[1][i]+b*mCV[1][i]-d*2.0f*mCU[0][i]*mCU[1][i];
                coeff[6] = a*mCV[0][i]*mCV[0][i]+b*mCV[0][i]+c-d*mCU[0][i]*mCU[0][i];

                int roots_num = RTMath::SolveOne6th_equation(coeff, 0.0f, 1.0f, roots);
                // deal with each root
                for (int k = 0; k < roots_num; ++k) {
                    y_value = mCV[0][i]+mCV[1][i]*roots[k]+mCV[2][i]*roots[k]*roots[k]+mCV[3][i]*roots[k]*roots[k]*roots[k];
                    total_roots[total_roots_num][0] = (y_value-y0)/dy;
                    total_roots[total_roots_num][1] = (float)i;
                    total_roots[total_roots_num][2] = roots[k];

                    ++total_roots_num;
                }
            }

            // Once we got all the roots, we sort them in ascent order according to t-value at index 0 in total_roots
            float temp0, temp1, temp2;
            if (total_roots_num > 0) {
                for (int i = 0; i < total_roots_num; ++i) {
                    for (int j = i+1; j < total_roots_num; ++j) {
                        if (total_roots[i][0] > total_roots[j][0]) {
                            temp0 = total_roots[i][0];
                            total_roots[i][0] = total_roots[j][0];
                            total_roots[j][0] = temp0;

                            temp1 = total_roots[i][1];
                            total_roots[i][1] = total_roots[j][1];
                            total_roots[j][1] = temp1;

                            temp2 = total_roots[i][2];
                            total_roots[i][2] = total_roots[j][2];
                            total_roots[j][2] = temp2;
                        }
                    }
                }

                // Use the minimal root to get other hit information.
                if ((total_roots[0][0] > tmin + KEpsilon()) && (total_roots[0][0] < tmax)) {
                    rec.t = total_roots[0][0];
                    rec.pt = inRay.O() + rec.t * inRay.D();
                    rec.albedo = mColor;
                    rec.hit = true;
                    rec.n = this->get_normal(rec, (int)total_roots[0][1], total_roots[0][2]);
                    if (Dot(inRay.D(), rec.n) > 0.0f) {
                        rec.n = -rec.n;
                    }
                    rec.pMaterial = nullptr;

                    tmax = rec.t;

                    return true;
                }
            }

            return false;
        }

        virtual bool IntersectP(Ray const& inRay, float& tvalue) const {
            if (!mBBox.HitTest(inRay)) {
                return false;
            }

            Vec3f p = inRay.O() - mCenter;
            float x0 = p.X();
            float y0 = p.Y();
            float z0 = p.Z();
            float dx = inRay.D().X();
            float dy = inRay.D().Y();
            float dz = inRay.D().Z();

            float a = dx*dx+dz*dz;
            float b = 2.0f*((x0*dx+z0*dz)*dy-a*y0);
            float c = (x0*dy-dx*y0)*(x0*dy-dx*y0)+(z0*dy-dz*y0)*(z0*dy-dz*y0);
            float d = dy*dy;

            float coeff[7];
            float roots[6];
            int total_roots_num = 0;
            // because each b-spline may have up to 6 roots, so the size of 1-d is 18
            // 2-d[0] is t value in HitRecord, 2-d[1] is index of 3 b-spline, 2-d[2] is root of s itself.
            float total_roots[18][3];
            float y_value;
            // now we find roots For each b-spline
            for (int i = 0; i < 3; ++i) {
                coeff[0] = a*mCV[3][i]*mCV[3][i]-d*mCU[3][i]*mCU[3][i];
                coeff[1] = a*2.0f*mCV[2][i]*mCV[3][i]-d*2.0f*mCU[2][i]*mCU[3][i];
                coeff[2] = a*(mCV[2][i]*mCV[2][i]+2.0f*mCV[1][i]*mCV[3][i])-d*(mCU[2][i]*mCU[2][i]+2.0f*mCU[1][i]*mCU[3][i]);
                coeff[3] = a*2.0f*(mCV[0][i]*mCV[3][i]+mCV[1][i]*mCV[2][i])+b*mCV[3][i]-d*2.0f*(mCU[0][i]*mCU[3][i]+mCU[1][i]*mCU[2][i]);
                coeff[4] = a*(mCV[1][i]*mCV[1][i]+2.0f*mCV[0][i]*mCV[2][i])+b*mCV[2][i]-d*(mCU[1][i]*mCU[1][i]+2.0f*mCU[0][i]*mCU[2][i]);
                coeff[5] = a*2.0f*mCV[0][i]*mCV[1][i]+b*mCV[1][i]-d*2.0f*mCU[0][i]*mCU[1][i];
                coeff[6] = a*mCV[0][i]*mCV[0][i]+b*mCV[0][i]+c-d*mCU[0][i]*mCU[0][i];

                int roots_num = RTMath::SolveOne6th_equation(coeff, 0.0f, 1.0f, roots);
                // deal with each root
                for (int k = 0; k < roots_num; ++k) {
                    y_value = mCV[0][i]+mCV[1][i]*roots[k]+mCV[2][i]*roots[k]*roots[k]+mCV[3][i]*roots[k]*roots[k]*roots[k];
                    total_roots[total_roots_num][0] = (y_value-y0)/dy;
                    total_roots[total_roots_num][1] = i;
                    total_roots[total_roots_num][2] = roots[k];

                    ++total_roots_num;
                }
            }

            // Once we got all the roots, we sort them in ascent order according to t-value at index 0 in total_roots
            float temp0, temp1, temp2;
            if (total_roots_num > 0) {
                for (int i = 0; i < total_roots_num; ++i) {
                    for (int j = i+1; j < total_roots_num; ++j) {
                        if (total_roots[i][0] > total_roots[j][0]) {
                            temp0 = total_roots[i][0];
                            total_roots[i][0] = total_roots[j][0];
                            total_roots[j][0] = temp0;

                            temp1 = total_roots[i][1];
                            total_roots[i][1] = total_roots[j][1];
                            total_roots[j][1] = temp1;

                            temp2 = total_roots[i][2];
                            total_roots[i][2] = total_roots[j][2];
                            total_roots[j][2] = temp2;
                        }
                    }
                }

                // Use the minimal root to get other hit information.
                if (total_roots[0][0] > KEpsilon()) {
                    tvalue = total_roots[0][0];
                    return true;
                }
            }

            return false;
        }

    public:
        virtual float Area() const {
            return 0.0f;
        }

        virtual bool GetBoundingBox(float t0, float t1, AABB& bounding) {
            bounding = mBBox;
            return true;
        }

        virtual Vec3f GetNormal(const HitRecord& rec) const {
            return Vec3f(0.0f, 0.0f, 0.0f);
        }

        virtual Vec3f SampleRandomPoint() const
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

        virtual Vec3f SampleRandomDirection(Vec3f const& v) const
        {
            return Vec3f(0.0f, 0.0f, 0.0f);
        }

    public:
        inline void SetCenter(Vec3f const& center) {
            mCenter = center;
        }
        inline void SetColor(const Color3f color) {
            mColor = color;
        }
        inline void SetControlPoints(const float controlPoints[CP_COUNT][2]) {
            this->update(controlPoints);
        }

    public:
        inline static float KEpsilon() { return 0.001f; }

    private:
        inline void update(const float controlPoints[CP_COUNT][2]) {
            // bezier central matrix
            const float mat_bezier_N[MAT_DIM][MAT_DIM] = {
                    { 1,  4,  1, 0},
                    {-3,  0,  3, 0},
                    { 3, -6,  3, 0},
                    {-1,  3, -3, 1}
            };
            // convert above matrix to b-spline central matrix
            float mat_b_spline_N[MAT_DIM][MAT_DIM];
            for (int i = 0; i < MAT_DIM; ++i) {
                for (int j = 0; j < MAT_DIM; ++j) {
                    mat_b_spline_N[i][j] = mat_bezier_N[i][j] / 6.0f;
                }
            }

            // find bounding-box
            float org_u_points[CP_COUNT];
            float org_v_points[CP_COUNT];
            // i used to do it like this, then i spent almost a full day to debug it.
            // max_u min_v max_v are all undefined values.
//            float max_u = org_u_points[0]; // because it's symmetric in U direction, so we only need to know half of its width.
//            float min_v = org_v_points[0];
//            float max_v = org_v_points[0];
            for (int i = 0; i < CP_COUNT; ++i) {
                org_u_points[i] = controlPoints[i][0];
                org_v_points[i] = controlPoints[i][1];
            }
            float max_u = org_u_points[0]; // because it's symmetric in U direction, so we only need to know half of its width.
            float min_v = org_v_points[0];
            float max_v = org_v_points[0];
            for (int i = 0; i < CP_COUNT; ++i) {
                if (org_u_points[i] > max_u) {
                    max_u = org_u_points[i];
                }
                if (org_v_points[i] < min_v) {
                    min_v = org_v_points[i];
                }
                if (org_v_points[i] > max_v) {
                    max_v = org_v_points[i];
                }
            }
            Vec3f v_box_min(mCenter.X() - max_u, mCenter.Y() + min_v, mCenter.Z() - max_u);
            Vec3f v_box_max(mCenter.X() + max_u, mCenter.Y() + max_v, mCenter.Z() + max_u);
            mBBox = AABB(v_box_min, v_box_max);
//            std::cout << max_u << " " << min_v << " " << max_v << std::endl;

            // Set up the final control points in U and V coord
            // 0 1 2 3 4 5 => 0 1 2 3; 1 2 3 4; 2 3 4 5
            float temp_u_points[4][1];
            float temp_v_points[4][1];
            float final_u_points[4][1];
            float final_v_points[4][1];
            int temp_num = 0;
            for (int i = 0; i < 3; ++i) {
                // get each b-spline and convert it to final control points by multiplying mat_b_spline_N
                temp_u_points[0][0] = org_u_points[i];
                temp_u_points[1][0] = org_u_points[i+1];
                temp_u_points[2][0] = org_u_points[i+2];
                temp_u_points[3][0] = org_u_points[i+3];
                temp_v_points[0][0] = org_v_points[i];
                temp_v_points[1][0] = org_v_points[i+1];
                temp_v_points[2][0] = org_v_points[i+2];
                temp_v_points[3][0] = org_v_points[i+3];

                Matrix4x4_Mul_Matrix_4_1(mat_b_spline_N, temp_u_points, final_u_points);
                Matrix4x4_Mul_Matrix_4_1(mat_b_spline_N, temp_v_points, final_v_points);

                for (int k = 0; k < MAT_DIM; ++k) {
                    mCU[k][temp_num] = (std::fabs(final_u_points[k][0]) < 1e-3) ? 0.0f : final_u_points[k][0];
                    mCV[k][temp_num] = (std::fabs(final_v_points[k][0]) < 1e-3) ? 0.0f : final_v_points[k][0];
                }

                ++temp_num;
            }

        }

    private:
        inline Vec3f get_normal(const HitRecord& rec, int root_index, float root_s) {
            float nx = rec.pt.X() - mCenter.X();
            float nz = rec.pt.Z() - mCenter.Z();
            float nu = -(mCV[1][root_index]+2.0*mCV[2][root_index]*root_s+3.0*mCV[3][root_index]*root_s*root_s);
            float nv = (mCU[1][root_index]+2.0*mCU[2][root_index]*root_s+3.0*mCU[3][root_index]*root_s*root_s);
            float ny = std::sqrt(nx*nx+nz*nz)*nv/nu - mCenter.Y();
            Vec3f norm(nx, ny, nz);
            norm.MakeUnit();
            return norm;
        }

    private:
        Vec3f mCenter;
        Color3f mColor;
        AABB mBBox;
        // Stands For each b-spline has 4 control points, 'U' is u-coord of each control point
        // and we've got 3 b-spline here.
        float mCU[4][3];
        // Stands For each b-spline has 4 control points, 'V' is v-coord of each control point
        // and we've got 3 b-spline here.
        float mCV[4][3];

    };

	//
	// Complex compound obects(can be with different materials for each part)
	//
	class Material;

    // Solid cylinder object(it can be used as normal geometric object or MeshObject)
	// Which means its material can be set seperately for each part.
    class SolidCylinder : public CompoundObject {
	public:
		enum SOLID_PART { BODY = 0, TOP, BOTTOM, };

    public:
        SolidCylinder()
            : mPos(0.0f, 0.0f, 0.0f), mRadius(2.0f), mY0(0.0f), mY1(5.0f), mColor(0.9f, 0.3f, 0.0f), mMaterial(false) {
            construct_object(mPos, mRadius, mY0, mY1, mColor);
        }
        SolidCylinder(Vec3f const& pos, float radius, float y0, float y1, Color3f const& color)
            : mPos(pos), mRadius(radius), mY0(y0), mY1(y1), mColor(color), mMaterial(false) {
            construct_object(mPos, mRadius, mY0, mY1, mColor);
        }
        virtual ~SolidCylinder() {

        }

    public:
        virtual void *Clone() {
            return (void *)(new SolidCylinder(*this));
        }

    public:
        virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec) {
            return CompoundObject::HitTest(inRay, tmin, tmax, rec);
        }

        virtual bool IntersectP(Ray const& inRay, float& tvalue) const {
            return CompoundObject::IntersectP(inRay, tvalue);
        }

    public:
        virtual float Area() const {
            return CompoundObject::Area();
        }

        virtual bool GetBoundingBox(float t0, float t1, AABB& bounding) {
            return CompoundObject::GetBoundingBox(t0, t1, bounding);
        }

        virtual Vec3f GetNormal(const HitRecord& rec) const {
            return CompoundObject::GetNormal(rec);
        }

        virtual Vec3f SampleRandomPoint() const {
            return CompoundObject::SampleRandomPoint();
        }

        virtual void Update(float t) {
            CompoundObject::Update(t);
        }

        virtual bool IsCompound() const {
            return true;
        }

        virtual float PDFValue(Vec3f const& o, Vec3f const& v) const {
            return CompoundObject::PDFValue(o, v);
        }

        virtual Vec3f SampleRandomDirection(Vec3f const& v) const {
            return CompoundObject::SampleRandomDirection(v);
        }

	public:
		void SetMaterial(Material *bodyMaterail = nullptr, bool autoDelete0 = false,
			Material *topMaterial = nullptr, bool autoDelete1 = false,
			Material *bottomMaterial = nullptr, bool autoDelete2 = false);

		void SetPartMaterial(int partNo, Material *material, bool autoDelete);

    public:
        inline static float KEpsilon() { return 0.001f; }

    private:
        inline void construct_object(Vec3f const& pos, float radius, float y0, float y1, Color3f const& color) {
            CompoundObject::mbAutoDelete = true;
            // It consists of the following sub-objects.
            CompoundObject::AddObject(new SimpleCylinder(pos, radius, y0, y1, color));

            SimpleDisk *pTopDisk = new SimpleDisk(Vec3f(mPos.X(), mPos.Y() + y1, mPos.Z()), Vec3f(0.0f, 1.0f, 0.0f), mRadius);
        //    pTopDisk->SetColor(Color3f(0.0f, 0.0f, 1.0f));

            SimpleDisk *pBottomDisk = new SimpleDisk(Vec3f(mPos.X(), mPos.Y() + y0, mPos.Z()), Vec3f(0.0f, -1.0f, 0.0f), mRadius);
       //     pBottomDisk->SetColor(Color3f(0.0f, 0.0f, 1.0f));

            CompoundObject::AddObject(pTopDisk);
            CompoundObject::AddObject(pBottomDisk);

        }

    private:
        Vec3f mPos;
        float mRadius;
        float mY0;
        float mY1;
        Color3f mColor;

		bool mMaterial;
    };

	// FlatRimmedBowl(it can be used as normal geometric object or MeshObject)
	// Which means its material can be set seperately for each part.
	class FlatRimmedBowl : public CompoundObject {
	public:
		enum BOWL_PART { INNER = 0, OUTER, TOP, };

	public:
		FlatRimmedBowl()
			: mCenter(0.0f, 0.0f, 0.0f), mInnerRadius(4.5f), mOuterRadius(5.0f), mColor(0.0f, 0.3f, 0.9f), 
			mMaterial(false), mPhiMin(0.0f), mPhiMax(360.0f), mThetaMin(90.0f), mThetaMax(180.0f) {
			this->construct_obect();
		}

		FlatRimmedBowl(Vec3f const& center, float innerRadius, float outerRadius, Color3f const& color,
			float phiMin = 0.0f, float phiMax = 360.0f, float thetaMin = 90.0f, float thetaMax = 180.0f)
			: mCenter(center), mInnerRadius(innerRadius), mOuterRadius(outerRadius), mColor(color), mMaterial(false),
			  mPhiMin(phiMin), mPhiMax(phiMax), mThetaMin(thetaMin), mThetaMax(thetaMax) {
			this->construct_obect();
		}

		virtual ~FlatRimmedBowl() {

		}

	public:
		virtual void *Clone() {
			return (void *)(new FlatRimmedBowl(*this));
		}

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec) {
			return CompoundObject::HitTest(inRay, tmin, tmax, rec);
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const {
			return CompoundObject::IntersectP(inRay, tvalue);
		}

	public:
		virtual float Area() const {
			return CompoundObject::Area();
		}

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding) {
			return CompoundObject::GetBoundingBox(t0, t1, bounding);
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const {
			return CompoundObject::GetNormal(rec);
		}

        virtual Vec3f SampleRandomPoint() const {
            return CompoundObject::SampleRandomPoint();
		}

		virtual void Update(float t) {
			CompoundObject::Update(t);
		}

		virtual bool IsCompound() const {
			return true;
		}

		virtual float PDFValue(Vec3f const& o, Vec3f const& v) const {
			return CompoundObject::PDFValue(o, v);
		}

        virtual Vec3f SampleRandomDirection(Vec3f const& v) const {
            return CompoundObject::SampleRandomDirection(v);
		}

	public:
		void SetMaterial(Material *innerMaterail = nullptr, bool autoDelete0 = false,
			Material *outerMaterial = nullptr, bool autoDelete1 = false,
			Material *topMaterial = nullptr, bool autoDelete2 = false);

		void SetPartMaterial(int partNo, Material *material, bool autoDelete);

	public:
		inline static float KEpsilon() { return 0.001f; }

	private:
		inline void construct_obect() {
			mbAutoDelete = true;

			SimpleSphere *pInnerHalfShpere = new SimpleSphere(mCenter, mInnerRadius);
			pInnerHalfShpere->SetColor(mColor);
			pInnerHalfShpere->SetPartParams(true, mPhiMin, mPhiMax, mThetaMin, mThetaMax);
			mvecObjects.push_back(pInnerHalfShpere);

			SimpleSphere *pOuterHalfShpere = new SimpleSphere(mCenter, mOuterRadius);
			pOuterHalfShpere->SetColor(mColor);
			pOuterHalfShpere->SetPartParams(true, mPhiMin, mPhiMax, mThetaMin, mThetaMax);
			mvecObjects.push_back(pOuterHalfShpere);

			SimpleRing *pTopRing = new SimpleRing(mCenter, Vec3f(0.0f, 1.0f, 0.0f), mOuterRadius, mInnerRadius);
			pTopRing->SetColor(mColor);
			mvecObjects.push_back(pTopRing);
		}

	private:
		Vec3f mCenter;
		float mInnerRadius;
		float mOuterRadius;
		Color3f mColor;
		float mPhiMin;
		float mPhiMax;
		float mThetaMin;
		float mThetaMax;

		bool mMaterial;

	};

	// ThickRing
	class ThickRing : public CompoundObject {
	public:
		enum THICK_RING_PART { INNER = 0, OUTER, TOP, BOTTOM, };

	public:
		ThickRing()
			: mCenter(0.0f, 0.0f, 0.0f), mInnerRadius(1.5f), mOuterRadius(4.0f), 
			  mY0(-0.2f), mY1(0.2f), mColor(0.9f, 0.3f, 0.0f), mMaterial(false) {
			this->construct_object();
		}

		ThickRing(Vec3f const& center, float innerRaidus, float outerRadius, float y0, float y1, Color3f const& color)
			: mCenter(center), mInnerRadius(innerRaidus), mOuterRadius(outerRadius), 
			  mY0(y0), mY1(y1), mColor(color), mMaterial(false) {
			this->construct_object();
		}

		virtual ~ThickRing() {

		}

	public:
		virtual void *Clone() {
			return (void *)(new ThickRing(*this));
		}

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec) {
			return CompoundObject::HitTest(inRay, tmin, tmax, rec);
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const {
			return CompoundObject::IntersectP(inRay, tvalue);
		}

	public:
		virtual float Area() const {
			return CompoundObject::Area();
		}

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding) {
			return CompoundObject::GetBoundingBox(t0, t1, bounding);
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const {
			return CompoundObject::GetNormal(rec);
		}

        virtual Vec3f SampleRandomPoint() const {
            return CompoundObject::SampleRandomPoint();
		}

		virtual void Update(float t) {
			CompoundObject::Update(t);
		}

		virtual bool IsCompound() const {
			return true;
		}

		virtual float PDFValue(Vec3f const& o, Vec3f const& v) const {
			return CompoundObject::PDFValue(o, v);
		}

        virtual Vec3f SampleRandomDirection(Vec3f const& v) const {
            return CompoundObject::SampleRandomDirection(v);
		}

	public:
		void SetMaterial(Material *innerMaterial = nullptr, bool autoDelete0 = false,
			Material *outerMaterial = nullptr, bool autoDelete1 = false,
			Material *topMaterial = nullptr, bool autoDelete2 = false,
			Material *bottomMaterial = nullptr, bool autoDelete3 = false);

		void SetPartMaterial(int partNo, Material *material, bool autoDelete);

	public:
		inline static float KEpsilon() { return 0.001f; }

	private:
		inline void construct_object() {
			SimpleCylinder *innerCylinder = new SimpleCylinder(mCenter, mInnerRadius, mY0, mY1, mColor);
			mvecObjects.push_back(innerCylinder);

			SimpleCylinder *outerCylinder = new SimpleCylinder(mCenter, mOuterRadius, mY0, mY1, mColor);
			mvecObjects.push_back(outerCylinder);

			Vec3f topRingCenter(mCenter.X(), mY1, mCenter.Z());
			SimpleRing *topRing = new SimpleRing(topRingCenter, Vec3f(0.0f, 1.0f, 0.0f), mOuterRadius, mInnerRadius);
			topRing->SetColor(mColor);
			mvecObjects.push_back(topRing);

			Vec3f bottomRingCenter(mCenter.X(), mY0, mCenter.Z());
			SimpleRing *bottomRing = new SimpleRing(bottomRingCenter, Vec3f(0.0f, -1.0f, 0.0f), mOuterRadius, mInnerRadius);
			bottomRing->SetColor(mColor);
			mvecObjects.push_back(bottomRing);

		}

	private:
		Vec3f mCenter;
		float mInnerRadius;
		float mOuterRadius;
		float mY0;
		float mY1;
		Color3f mColor;

		bool mMaterial;
	};

	// SolidCone
	class SolidCone : public CompoundObject {
	public:
		enum SOLID_CONE_PART { BODY = 0, BOTTOM, };

	public:
		SolidCone()
			: mCenter(0.0f, 0.0f, 0.0f), mRadius(1.8f), mY0(0.0f), mY1(6.0f)
			,mColor(0.3f, 0.9f, 0.0f), mMaterial(false) {
			this->construct_object();
		}

		SolidCone(Vec3f const& center, float radius, float y0, float y1, Color3f const& color)
			: mCenter(center), mRadius(radius), mY0(y0), mY1(y1),
			mColor(color), mMaterial(false) {
			this->construct_object();
		}

		virtual ~SolidCone() {

		}

	public:
		virtual void *Clone() {
			return (void *)(new SolidCone(*this));
		}

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec) {
			return CompoundObject::HitTest(inRay, tmin, tmax, rec);
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const {
			return CompoundObject::IntersectP(inRay, tvalue);
		}

	public:
		virtual float Area() const {
			return CompoundObject::Area();
		}

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding) {
			return CompoundObject::GetBoundingBox(t0, t1, bounding);
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const {
			return CompoundObject::GetNormal(rec);
		}

        virtual Vec3f SampleRandomPoint() const {
            return CompoundObject::SampleRandomPoint();
		}

		virtual void Update(float t) {
			CompoundObject::Update(t);
		}

		virtual bool IsCompound() const {
			return true;
		}

		virtual float PDFValue(Vec3f const& o, Vec3f const& v) const {
			return CompoundObject::PDFValue(o, v);
		}

        virtual Vec3f SampleRandomDirection(Vec3f const& v) const {
            return CompoundObject::SampleRandomDirection(v);
		}

	public:
		void SetMaterial(Material *bodyMaterial = nullptr, bool autoDelete0 = false,
			Material *bottomMaterial = nullptr, bool autoDelete2 = false);

		void SetPartMaterial(int partNo, Material *material, bool autoDelete);

	public:
		inline static float KEpsilon() { return 0.001f; }

	private:
		inline void construct_object() {
			SimpleCone *cone = new SimpleCone(mCenter, mRadius, mY0, mY1, mColor);

			SimpleDisk *bottom = new SimpleDisk(mCenter, Vec3f(0.0f, -1.0f, 0.0f), mRadius);
			bottom->SetColor(mColor);

			mvecObjects.push_back(cone);
			mvecObjects.push_back(bottom);
		}

	private:
		Vec3f mCenter;
		float mRadius;
		float mY0;
		float mY1;
		Color3f mColor;

		bool mMaterial;
	};

	// FrustumCone
	class FrustumCone : public CompoundObject {
	public:
		enum FRUSTUM_CONE_PART { BODY = 0, TOP, BOTTOM, };

	public:
		FrustumCone()
			: mCenter(0.0f, 0.0f, 0.0f), mRadius(1.8f), mY0(0.0f), mY1(6.0f), mInterceptionH(3.6f), 
			mColor(0.3f, 0.9f, 0.0f), mMaterial(false) {
			this->construct_object();
		}

		FrustumCone(Vec3f const& center, float radius, float y0, float y1, float interceptionH, Color3f const& color)
			: mCenter(center), mRadius(radius), mY0(y0), mY1(y1), mInterceptionH(interceptionH),
			mColor(color), mMaterial(false) {
			this->construct_object();
		}

		virtual ~FrustumCone() {

		}

	public:
		virtual void *Clone() {
			return (void *)(new FrustumCone(*this));
		}

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec) {
			return CompoundObject::HitTest(inRay, tmin, tmax, rec);
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const {
			return CompoundObject::IntersectP(inRay, tvalue);
		}

	public:
		virtual float Area() const {
			return CompoundObject::Area();
		}

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding) {
			return CompoundObject::GetBoundingBox(t0, t1, bounding);
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const {
			return CompoundObject::GetNormal(rec);
		}

        virtual Vec3f SampleRandomPoint() const {
            return CompoundObject::SampleRandomPoint();
		}

		virtual void Update(float t) {
			CompoundObject::Update(t);
		}

		virtual bool IsCompound() const {
			return true;
		}

		virtual float PDFValue(Vec3f const& o, Vec3f const& v) const {
			return CompoundObject::PDFValue(o, v);
		}

        virtual Vec3f SampleRandomDirection(Vec3f const& v) const {
            return CompoundObject::SampleRandomDirection(v);
		}

	public:
		void SetMaterial(Material *bodyMaterial = nullptr, bool autoDelete0 = false,
			Material *topMaterial = nullptr, bool autoDelete1 = false,
			Material *bottomMaterial = nullptr, bool autoDelete2 = false);

		void SetPartMaterial(int partNo, Material *material, bool autoDelete);

	public:
		inline static float KEpsilon() { return 0.001f; }

	private:
		inline void construct_object() {
			SimpleCone *cone = new SimpleCone(mCenter, mRadius, mY0, mY1, mColor);
			cone->SetPartParams(true, 0.0f, 360.0f, mInterceptionH);

			// r1	(y1 - y0 - interception_h)
			// -- = ---------------------------  =>  r1 = (r0 * (y1 - y0 - interception_h)) / (y1 - y0)
			// r0	 (y1 - y0)
			float r1 = (mRadius * (mY1 - mY0 - mInterceptionH) / (mY1 - mY0));
			Vec3f top_center(mCenter.X(), mY0 + mInterceptionH, mCenter.Z());
			SimpleDisk *top = new SimpleDisk(top_center, Vec3f(0.0f, 1.0f, 0.0f), r1);
			top->SetColor(mColor);

			SimpleDisk *bottom = new SimpleDisk(mCenter, Vec3f(0.0f, -1.0f, 0.0f), mRadius);
			bottom->SetColor(mColor);

			mvecObjects.push_back(cone);
			mvecObjects.push_back(top);
			mvecObjects.push_back(bottom);
		}

	private:
		Vec3f mCenter;
		float mRadius;
		float mY0;
		float mY1;
		float mInterceptionH;
		Color3f mColor;

		bool mMaterial;
	};

	// ThickTrimedCone
	class ThickTrimedCone : public CompoundObject {
	public:
		enum THICK_TRIMED_CONE_PART { INNER = 0, OUTER, TOP, BOTTOM, };

	public:
		ThickTrimedCone()
			: mCenter(0.0f, 0.0f, 0.0f), mRadius0(1.4f), mRadius1(2.0f), mY0(0.0f), mY1(6.0f), 
			mInterceptionH(3.6f), mColor(0.3f, 0.9f, 0.0f), mMaterial(false) {
			this->construct_object();
		}

		ThickTrimedCone(Vec3f const& center, float radius0, float radius1, float y0, float y1, float interceptionH, Color3f const& color)
			: mCenter(center), mRadius0(radius0), mRadius1(radius1), mY0(y0), mY1(y1),
			  mInterceptionH(interceptionH), mColor(color), mMaterial(false) {
			this->construct_object();
		}

		virtual ~ThickTrimedCone() {

		}

	public:
		virtual void *Clone() {
			return (void *)(new ThickTrimedCone(*this));
		}

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec) {
			return CompoundObject::HitTest(inRay, tmin, tmax, rec);
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const {
			return CompoundObject::IntersectP(inRay, tvalue);
		}

	public:
		virtual float Area() const {
			return CompoundObject::Area();
		}

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding) {
			return CompoundObject::GetBoundingBox(t0, t1, bounding);
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const {
			return CompoundObject::GetNormal(rec);
		}

        virtual Vec3f SampleRandomPoint() const {
            return CompoundObject::SampleRandomPoint();
		}

		virtual void Update(float t) {
			CompoundObject::Update(t);
		}

		virtual bool IsCompound() const {
			return true;
		}

		virtual float PDFValue(Vec3f const& o, Vec3f const& v) const {
			return CompoundObject::PDFValue(o, v);
		}

        virtual Vec3f SampleRandomDirection(Vec3f const& v) const {
            return CompoundObject::SampleRandomDirection(v);
		}

	public:
		void SetMaterial(Material *innerMaterial = nullptr, bool autoDelete0 = false,
			Material *outerMaterial = nullptr, bool autoDelete1 = false,
			Material *topMaterial = nullptr, bool autoDelete2 = false,
			Material *bottomMaterial = nullptr, bool autoDelete3 = false);

		void SetPartMaterial(int partNo, Material *material, bool autoDelete);

	public:
		inline static float KEpsilon() { return 0.001f; }

	private:
		inline void construct_object() {
			SimpleCone *inner_cone = new SimpleCone(mCenter, mRadius0, mY0, mY1, mColor);
			inner_cone->SetPartParams(true, 0.0f, 360.0f, mInterceptionH);

			SimpleCone *outer_cone = new SimpleCone(mCenter, mRadius1, mY0, mY1, mColor);
			outer_cone->SetPartParams(true, 0.0f, 360.0f, mInterceptionH);

			// r1	(y1 - y0 - interception_h)
			// -- = ---------------------------  =>  r1 = (r0 * (y1 - y0 - interception_h)) / (y1 - y0)
			// r0	 (y1 - y0)
			float r1_inner = (mRadius0 * (mY1 - mY0 - mInterceptionH) / (mY1 - mY0));
			float r1_outer = (mRadius1 * (mY1 - mY0 - mInterceptionH) / (mY1 - mY0));
			Vec3f top_center(mCenter.X(), mY0 + mInterceptionH, mCenter.Z());
			SimpleRing *top = new SimpleRing(top_center, Vec3f(0.0f, 1.0f, 0.0f), r1_outer, r1_inner);
			top->SetColor(mColor);

			SimpleRing *bottom = new SimpleRing(mCenter, Vec3f(0.0f, -1.0f, 0.0f), mRadius1, mRadius0);
			bottom->SetColor(mColor);

			mvecObjects.push_back(inner_cone);
			mvecObjects.push_back(outer_cone);
			mvecObjects.push_back(top);
			mvecObjects.push_back(bottom);
		}

	private:
		Vec3f mCenter;
		float mRadius0;
		float mRadius1;
		float mY0;
		float mY1;
		float mInterceptionH;
		Color3f mColor;

		bool mMaterial;
	};

}
