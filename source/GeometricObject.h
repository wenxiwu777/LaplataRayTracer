#pragma once
#include <iostream>
#include "Common.h"
#include "Console.h"
#include "Random.h"
#include "Vec3.h"
#include "Utility.h"
#include "Ray.h"
#include "AABB.h"
#include "HitRecord.h"
#include "ICloneable.h"
#include "Sampling.h"
#include "Transform.h"

namespace LaplataRayTracer
{
	//
	class Hitable : public ICloneable
	{
	public:
		Hitable() { }
		virtual ~Hitable() { }

	public:
		virtual void *Clone() = 0;

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec) = 0;
		virtual bool IntersectP(Ray const& inRay, float& tvalue) const = 0;

	};

	//
	class GeometricObject : public Hitable
	{
	public:
		GeometricObject() { }
		virtual ~GeometricObject() { }

	public:
		virtual void *Clone() = 0;

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec) = 0;
		virtual bool IntersectP(Ray const& inRay, float& tvalue) const = 0;

	public:
		virtual float Area() const = 0;
		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding) = 0;
		virtual Vec3f GetNormal(const HitRecord& rec) const = 0;
		virtual Vec3f RandomSamplePoint() const = 0;
		virtual void Update(float t) = 0;
		virtual bool IsCompound() const = 0;
		virtual float PDFValue(Vec3f const& o, Vec3f const& v) const = 0;
		virtual Vec3f RandomSampleDirection(Vec3f const& v) const = 0;

	};

	//
	class SimplePlane : public GeometricObject
	{
	public:
		SimplePlane(Vec3f const& vPos, Vec3f const& vNormal)
			: mColor(1.0f, 0.0f, 0.0f), mPos(vPos), mNormal(vNormal)
		{

		}
		virtual ~SimplePlane()
		{

		}

	public:
		virtual void *Clone() { return (GeometricObject *)(new SimplePlane(*this)); }

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
		{
			Vec3f OA = inRay.O() - mPos;
			float A = -Dot(OA, mNormal);
			float B = Dot(inRay.D(), mNormal);
			float t = A / B;

			if ((t > tmin + KEpsilon()) && (t < tmax))
			{
				tmax = t;

				rec.hit = true;
				rec.t = t;
				rec.n = mNormal;
				rec.pt = inRay.O() + inRay.D() * rec.t;
				rec.albedo = mColor;
				rec.pMaterial = nullptr;

				return true;
			}

			rec.hit = false;
			return false;
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const
		{
			Vec3f OA = inRay.O() - mPos;
			float A = -Dot(OA, mNormal);
			float B = Dot(inRay.D(), mNormal);
			float t = A / B;

			if (t > KEpsilon())
			{
				tvalue = t;
				return true;
			}

			return false;
		}

	public:
		virtual float Area() const
		{
			return FLT_MAX;
		}

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding)
		{
			return false;
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const
		{
			return mNormal;
		}

		virtual Vec3f RandomSamplePoint() const
		{
			return WORLD_ORIGIN;
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
		inline void SetColor(Color3f const& c) { mColor = c; }
		inline Color3f GetColor() const { return mColor; }
		inline void SetPosition(Vec3f const& v) { mPos = v; }
		inline Vec3f GetPosition() const { return mPos; }
		inline void SetNormal(Vec3f const& n) { mNormal = n; }

	public:
		inline static float KEpsilon() { return 0.001f; }

	private:
		Color3f mColor;
		Vec3f	mPos;
		Vec3f	mNormal;

	};

	//
	class SimpleSphere : public GeometricObject
	{
	public:
		SimpleSphere(Vec3f const& pos, float fRadius)
			: mColor(1.0f, 0.0f, 0.0f), mPos(pos), mfRadius(fRadius), mbPart(false)
		{
			update_shpere();
		}
		virtual ~SimpleSphere() { }

	public:
		virtual void *Clone() { return (GeometricObject *)(new SimpleSphere(*this)); }

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
		{
		//	Vec3f vOC = inRay.O() - mPos;
			Vec3f vOC = inRay.O() - getPos(inRay.T());

			float A = Dot<float>(inRay.D(), inRay.D());
			float B = 2.0f * (Dot<float>(inRay.D(), vOC));
			float C = Dot<float>(vOC, vOC) - mfRadius * mfRadius;
			float D = B * B - 4 * A * C;

			if (D < 0.0f)
			{
				return false;
			}

			float result = 0.0f;
			bool isHit = false;
			float ans0 = (-B - std::sqrt(D)) / (2.0f * A);
			if ((ans0 > tmin + KEpsilon()) && (ans0 < tmax))
			{
				// is it a part shpere?
				if (mbPart)
				{
					Vec3f ptTemp = inRay.O() + inRay.D() * ans0;
					Vec3f ptCheck = ptTemp - mPos;
					if (!this->check_in_range(ptCheck))
					{
						isHit = false;
					}
					else
					{
						isHit = true;
						result = ans0;
					}
				}
				else
				{
					isHit = true;
					result = ans0;
				}
			}
			if (!isHit)
			{
				float ans1 = (-B + std::sqrt(D)) / (2.0f * A);
				if ((ans1 > tmin + KEpsilon()) && (ans1 < tmax))
				{
					// is it a part shpere?
					if (mbPart)
					{
						Vec3f ptTemp = inRay.O() + inRay.D() * ans1;
						Vec3f ptCheck = ptTemp - mPos;
						if (!this->check_in_range(ptCheck))
						{
							isHit = false;
						}
						else
						{
							isHit = true;
							result = ans1;
						}
					}
					else
					{
						isHit = true;
						result = ans1;
					}
				}
			}

			if (isHit)
			{
			    //
				tmax = result;

				rec.hit = true;
				rec.t = result;
				rec.pt = inRay.O() + inRay.D() * rec.t;
				Vec3f vR = (rec.pt - mPos);
				vR.MakeUnit();
				rec.n = vR;
				////////////////////// Only for testing, shall be removed later.
//				float theta = std::acos(RTMath::Clamp(rec.pt.Z() / mfRadius, -1.0f, 1.0f));
//				float zRadius = std::sqrt(rec.pt.X() * rec.pt.X() + rec.pt.Y() * rec.pt.Y());
//				float invZRadius = 1 / zRadius;
//				float cosPhi = rec.pt.X() * invZRadius;
//				float sinPhi = rec.pt.Y() * invZRadius;
//				Vec3f dpdu(-TWO_PI_CONST * rec.pt.Y(), TWO_PI_CONST * rec.pt.X(), 0);
//				Vec3f dpdv(rec.pt.Z() * cosPhi, rec.pt.Z() * sinPhi, -mfRadius * std::sin(theta));
//				rec.dpdu = dpdu;
//				rec.dpdv = PI_CONST * dpdv;
				////////////////////// Only for testing, shall be removed later.
				if (mbPart)
                {
                    if (Dot(rec.n, inRay.D()) > 0.0f)
                    {
                        rec.n = -rec.n; // to handle the case of part object.
                    }
                }
				//	rec.albedo = Color3f(0.5f*(rec.n.X()+1.0f), 0.5f*(rec.n.Y() + 1.0f), 0.5f*(rec.n.Z() + 1.0f));
				rec.albedo = mColor;
				rec.pMaterial = nullptr;
			//	g_Console.Write("hit\n");
				return true;
			}

			rec.hit = false;
			return false;
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const
		{
		//	Vec3f vOC = inRay.O() - mPos;
			Vec3f vOC = inRay.O() - getPos(inRay.T());

			float A = Dot<float>(inRay.D(), inRay.D());
			float B = 2 * (Dot<float>(inRay.D(), vOC));
			float C = Dot<float>(vOC, vOC) - mfRadius * mfRadius;
			float D = B * B - 4 * A * C;

			if (D < 0.0f)
			{
				return false;
			}

			float result = 0.0f;
			bool isHit = false;
			float ans0 = (-B - std::sqrt(D)) / (2 * A);
			if (ans0 > KEpsilon())
			{
				if (mbPart)
				{
					// is it a part shpere?
					Vec3f ptTemp = inRay.O() + inRay.D() * ans0;
					Vec3f ptCheck = ptTemp - mPos;
					if (!this->check_in_range(ptCheck))
					{
						isHit = false;
					}
					else
					{
						result = ans0;
						isHit = true;
					}
				}
				else
				{
					result = ans0;
					isHit = true;
				}
			}
			if (!isHit)
			{
				float ans1 = (-B + std::sqrt(D)) / (2 * A);
				if (ans1 > KEpsilon())
				{
					//
					if (mbPart)
					{
						// is it a part shpere?
						Vec3f ptTemp = inRay.O() + inRay.D() * ans1;
						Vec3f ptCheck = ptTemp - mPos;
						if (!this->check_in_range(ptCheck))
						{
							return false;
						}
					}

					result = ans1;
					isHit = true;
				}
			}

			if (isHit)
			{
				tvalue = result;
			}
			return isHit;
		}

		/*virtual bool IntersectP(Ray const& inRay, float& tvalue) const
		{
			//	Vec3f vOC = inRay.O() - mPos;
			Vec3f vOC = inRay.O() - getPos(inRay.T());

			float A = Dot<float>(inRay.D(), inRay.D());
			float B = 2 * (Dot<float>(inRay.D(), vOC));
			float C = Dot<float>(vOC, vOC) - mfRadius * mfRadius;
			float D = B * B - 4 * A * C;

			if (D < 0.0f)
			{
				return false;
			}

			float result = 0.0f;
			bool isHit = false;
			float ans0 = (-B - std::sqrt(D)) / (2 * A);
			if (ans0 > KEpsilon())
			{
				result = ans0;
				isHit = true;
			}
			if (!isHit)
			{
				float ans1 = (-B + std::sqrt(D)) / (2 * A);
				if (ans1 > KEpsilon())
				{
					//
					if (mbPart)
					{
						// is it a part shpere?
						if (isHit)
						{
							Vec3f ptTemp = inRay.O() + inRay.D() * result;
							Vec3f ptCheck = ptTemp - mPos;
							if (!this->check_in_range(ptCheck))
							{
								return false;
							}
						}
					}

					result = ans1;
					isHit = true;
				}
			}

			tvalue = result;
			return isHit;
		}*/

	public:
		virtual float Area() const
		{
            return mfArea;
		}

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding)
		{
			bounding = AABB(mPos - Vec3f(mfRadius, mfRadius, mfRadius), mPos + Vec3f(mfRadius, mfRadius, mfRadius));
			return true;
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const
		{
			Vec3f temp = rec.pt - mPos;
			temp.MakeUnit();
			return temp;
		}

		virtual Vec3f RandomSamplePoint() const
		{
            float u = Random::frand48();
            float v = Random::frand48();

            float z = 1.0f - 2.0f * u;
            float r = std::sqrt(1.0f - z * z);
            float phi = TWO_PI_CONST * v;

            Vec3f pt = mPos + mfRadius * Vec3f(r * std::cos(phi), r * std::sin(phi), z);
            return pt;
		}

		virtual void Update(float t)
		{
			update_shpere();
		}

		virtual bool IsCompound() const
		{
			return false;
		}

		virtual float PDFValue(Vec3f const& o, Vec3f const& v) const
		{
            return (1.0f / mfArea);
		}

		virtual Vec3f RandomSampleDirection(Vec3f const& v) const
		{
            Vec3f pt = RandomSamplePoint();
            return (pt - v);
		}

	public:
		void SetColor(Color3f const& c) { mColor = c; }
		Color3f GetColor() const { return mColor; }
		void SetPosition(Vec3f const& v) { mPos = v; }
		Vec3f GetPosition() const { return mPos; }
		void SetRadius(float f) { mfRadius = f; }
		float GetRadius() const { return mfRadius; }

	public:
	    inline void SetPartParams(bool part, float phiMin, float phiMax, float thetaMin, float thetaMax)
	    {
		    mbPart = part;

		    mPhiMin = ANG2RAD(phiMin);
		    mPhiMax = ANG2RAD(phiMax);
		    mCosThetaMin = std::cos(ANG2RAD(thetaMin));
		    mCosThetaMax = std::cos(ANG2RAD(thetaMax));
		}

	public:
		inline static float KEpsilon() { return 0.001f; }

	protected:
		virtual Vec3f getPos(float t) const
		{
			return mPos;
		}

	private:
		inline void update_shpere()
		{
            mfArea = (2.0f * TWO_PI_CONST) * (mfRadius * mfRadius);
		}

	private:
	    inline bool check_in_range(const Vec3f& pt) const
        {
            double phi = std::atan2(pt.X(), pt.Z());
            if (phi < 0.0f)
                phi += TWO_PI_CONST;

            if (pt.Y() <= mfRadius * mCosThetaMin &&
                pt.Y() >= mfRadius * mCosThetaMax &&
                phi >= mPhiMin && phi <= mPhiMax) {
                return true;
            }

            return false;
		}

	protected:
		Color3f mColor;
		Vec3f  mPos;
		float mfRadius;
		float mfArea;

		//
		bool mbPart;
		float mPhiMin;
		float mPhiMax;
		float mCosThetaMin;
		float mCosThetaMax;
	};

    class MaxAngleSampledShpere : public SimpleSphere
    {
    public:
        MaxAngleSampledShpere(Vec3f const& pos, float fRadius)
            : SimpleSphere(pos, fRadius)
        {

        }
        virtual ~MaxAngleSampledShpere() { }

    public:
        virtual void *Clone() { return (GeometricObject *)(new MaxAngleSampledShpere(*this)); }

    public:
        virtual float PDFValue(Vec3f const& o, Vec3f const& v) const
        {
            Ray testRay(o, v, 0.0f);
            float tvalue = 0.0f;
            if (this->IntersectP(testRay, tvalue))
            {
                Vec3f vec_dir = mPos - o;
                float len = vec_dir.Length();
                float cos_theta_max = std::sqrt(1.0f - (mfRadius * mfRadius) / (len * len));
                float solid_angle = 2.0f * PI_CONST * (1.0f - cos_theta_max);
                return 1.0f / solid_angle;
            }

            return 0.0f;
        }

        virtual Vec3f RandomSampleDirection(Vec3f const& v) const
        {
            Vec3f vec_dir = mPos - v;
            float len = vec_dir.Length();
            float squd_dist = len * len;

            ONB onb;
            onb.BuildFromW(vec_dir);
            Vec3f rand_pt_on_shpere = SamplerBase::RandomToShpere(mfRadius, squd_dist);
            return (onb.Local2(rand_pt_on_shpere));
        }

    };

	class SimpleSphere2 : public GeometricObject
	{
	public:
        SimpleSphere2()
			: mCenter(0.0f, 0.0f, 0.0f), mRadius(20.0f), mColor(1.0f, 0.0f, 0.0f)
		{

		}

        SimpleSphere2(Vec3f const& center, float radius, const Color3f& color = Color3f(1.0f, 0.0f, 0.0f))
			: mCenter(center), mRadius(radius), mColor(color)
		{

		}

		virtual ~SimpleSphere2()
		{

		}

	public:
		virtual void *Clone()
		{
			return (SimpleSphere2 *)(new SimpleSphere2(*this));
		}

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
		{
			Vec3f vpos = inRay.O() - mCenter;
			float x0 = vpos.X();
			float y0 = vpos.Y();
			float z0 = vpos.Z();
			float dx = inRay.D().X();
			float dy = inRay.D().Y();
			float dz = inRay.D().Z();

			float A = dx*dx + dy*dy + dz*dz;
			float B = 2.0f*(x0*dx + y0*dy + z0*dz);
			float C = x0*x0 + y0*y0 + z0*z0 - mRadius*mRadius;

			float D = B*B - 4.0f*A*C;
			if (D < 0.0f)
			{
				return false;
			}

			bool is_hit = false;
			float temp = 0.0f;
			float root0 = (-B - std::sqrt(D)) / (2.0f * A);
			if ((root0 > tmin + KEpsilon()) && (root0 < tmax))
			{
				is_hit = true;
				temp = root0;
			}

			if (!is_hit)
			{
				float root1 = (-B + std::sqrt(D)) / (2.0f * A);
				if ((root1 > tmin + KEpsilon()) && (root1 < tmax))
				{
					is_hit = true;
					temp = root1;
				}
			}

			if (is_hit)
			{
				tmax = temp;

				rec.hit = true;
				rec.t = temp;
				rec.pt = inRay.O() + temp * inRay.D();
				rec.albedo = mColor;
				rec.n = GetNormal(rec);
				rec.pMaterial = nullptr;

			}

			return is_hit;
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const
		{
			Vec3f vpos = inRay.O() - mCenter;
			float x0 = vpos.X();
			float y0 = vpos.Y();
			float z0 = vpos.Z();
			float dx = inRay.D().X();
			float dy = inRay.D().Y();
			float dz = inRay.D().Z();

			float A = dx*dx + dy*dy + dz*dz;
			float B = 2.0f*(x0*dx + y0*dy + z0*dz);
			float C = x0*x0 + y0*y0 + z0*z0 - mRadius*mRadius;

			float D = B*B - 4.0f*A*C;
			if (D < 0.0f)
			{
				return false;
			}

			bool is_hit = false;
			float root0 = (-B - std::sqrt(D)) / (2.0f * A);
			if (root0 > KEpsilon())
			{
				is_hit = true;
				tvalue = root0;
			}

			if (!is_hit)
			{
				float root1 = (-B + std::sqrt(D)) / (2.0f * A);
				if (root1 > KEpsilon())
				{
					is_hit = true;
					tvalue = root1;
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
			Vec3f vmin = mCenter - Vec3f(mRadius, mRadius, mRadius);
			Vec3f vmax = mCenter + Vec3f(mRadius, mRadius, mRadius);
			bounding = AABB(vmin, vmax);

			return true;
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const
		{
			Vec3f norm;
			norm.Set(rec.pt.X() - mCenter.X(), rec.pt.Y() - mCenter.Y(), rec.pt.Z() - mCenter.Z());
			norm.MakeUnit();
			return norm;

		}

		virtual Vec3f RandomSamplePoint() const
		{
			return WORLD_ORIGIN;
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
			Ray testRay(o, v, 0.0f);
			float tvalue = 0.0f;
			if (this->IntersectP(testRay, tvalue))
			{
				Vec3f vec_dir = mCenter - o;
				float len = vec_dir.Length();
				float cos_theta_max = std::sqrt(1.0f - (mRadius * mRadius) / (len * len));
				float solid_angle = 2.0f * PI_CONST * (1.0f - cos_theta_max);
				return 1.0f / solid_angle;
			}

			return 0.0f;
		}

		virtual Vec3f RandomSampleDirection(Vec3f const& v) const
		{
			Vec3f vec_dir = mCenter - v;
			float len = vec_dir.Length();
			float squd_dist = len * len;

			ONB onb;
			onb.BuildFromW(vec_dir);
			Vec3f rand_pt_on_shpere = SamplerBase::RandomToShpere(mRadius, squd_dist);
			return (onb.Local2(rand_pt_on_shpere));
		}

	public:
		inline void SetCenter(Vec3f const& center) { mCenter = center; }
		inline void SetRadius(float radius) { mRadius = radius; }
		inline void SetColor(Color3f const& color) { mColor = color; }

	public:
		inline static float KEpsilon() { return 0.001f; }

	private:
		Vec3f mCenter;
		float mRadius;
		float mArea;
		Color3f mColor;

	};

	//
	class SimpleDisk : public GeometricObject
	{
	public:
		SimpleDisk(Vec3f const& vPos, Vec3f const& vNormal, float fRadius)
			: mPos(vPos), mColor(1.0f, 0.0f, 0.0f), mNormal(vNormal), mRadius(fRadius)
		{
			update_disk();
		}
		virtual ~SimpleDisk() { }

	public:
		virtual void *Clone() { return (GeometricObject *)(new SimpleDisk(*this)); }

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
		{
			Vec3f OP = inRay.O() - mPos;
			float A = -Dot(OP, mNormal);
			float B = Dot(inRay.D(), mNormal);
			float t = A / B;

			if ((t > tmin + KEpsilon()) && (t < tmax))
			{
				Vec3f pt = inRay.O() + t * inRay.D();
				if (pt.SquareDistance(mPos) <= mSquareRadius)
				{
					tmax = t;

					rec.hit = true;
					rec.t = t;
					rec.pt = pt;
					rec.n = mNormal;
					rec.pMaterial = nullptr;
					rec.albedo = mColor;
					calc_uv(rec.pt, rec.u, rec.v);

					return true;
				}
			}

			rec.hit = false;
			return false;
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const
		{
			Vec3f OP = inRay.O() - mPos;
			float A = -Dot(OP, mNormal);
			float B = Dot(inRay.D(), mNormal);
			float t = A / B;

			if (t > KEpsilon())
			{
				Vec3f pt = inRay.O() + t * inRay.D();
				if (pt.SquareDistance(mPos) <= mSquareRadius)
				{
					tvalue = t;
					return true;
				}
			}

			return false;
		}

	public:
		virtual float Area() const
		{
			return mArea;
		}

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding)
		{
			bounding = AABB(mPos - Vec3f(mRadius, mRadius, mRadius), mPos + Vec3f(mRadius, mRadius, mRadius));
			return true;
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const
		{
			return mNormal;
		}

		virtual Vec3f RandomSamplePoint() const
		{
			Point2f pt = SamplerBase::SampleInUnitDisk();
			Vec3f vecRand = mPos + pt.X() * mBase.U() + pt.Y() * mBase.V();
			return vecRand;
		}

		virtual void Update(float t)
		{
			update_disk();
		}

		virtual bool IsCompound() const
		{
			return false;
		}

		virtual float PDFValue(Vec3f const& o, Vec3f const& v) const
		{
			return (1.0f / mArea);
		}

		virtual Vec3f RandomSampleDirection(Vec3f const& v) const
		{
			Vec3f randDir = v - RandomSamplePoint();
			return randDir;
		}

	public:
		void SetColor(Color3f const& c) { mColor = c; }
		Color3f GetColor() const { return mColor; }
		void SetPosition(Vec3f const& v) { mPos = v; }
		Vec3f GetPosition() const { return mPos; }
		void SetRadius(float f) { mRadius = f; }
		float GetRadius() const { return mRadius; }
		void SetNormal(Vec3f const& n) { mNormal = n; }

	public:
		inline static float KEpsilon() { return 0.001f; }

	private:
		inline void update_disk()
		{
			mSquareRadius = mRadius * mRadius;
			mArea = PI_CONST * mRadius * mRadius;

			mBase.BuildFromW(mNormal);
		}

		inline void calc_uv(const Vec3f& pt,  float& u, float& v)
		{
			Vec3f vOP_ = pt - mPos;
			Vec3f vRef_;
			bool over_pi_ = false;
			if (RTMath::IsZero(vOP_[2]))
			{
				vRef_.Set(1.0f, 0.0f, 0.0f); // y
				if (pt[1] < mPos[1])
				{
					over_pi_ = true;
				}
			}
			else if (RTMath::IsZero(vOP_[1]))
			{
				vRef_.Set(0.0f, 0.0f, 1.0f); // x
				if (pt[0] < mPos[0])
				{
					over_pi_ = true;
				}
			}
			else if (RTMath::IsZero(vOP_[0]))
			{
				vRef_.Set(0.0f, 1.0f, 0.0f); // z
				if (pt[2] < mPos[2])
				{
					over_pi_ = true;
				}
			}
			else
			{
				return;
			}

			float op_len_ = vOP_.Length();
			v = op_len_ / mRadius;

			float op_dot_ref_ = Dot(vOP_, vRef_);
			u = std::acos(op_dot_ref_ / op_len_) / TWO_PI_CONST;

			if (over_pi_)
			{
				u = 1.0f - u;
			}
		}

	private:
		Vec3f mPos;
		Color3f mColor;
		Vec3f mNormal;
		float mRadius;

		float mSquareRadius;

		float mArea;

		ONB	 mBase;
	};

	//
	class SimpleRing : public GeometricObject
	{
	public:
		SimpleRing(Vec3f const& vPos, Vec3f const& vNormal, float fOuterRadius, float fInnerRadius)
			: mPos(vPos), mColor(Color3f(1.0f, 0.0f, 0.0f)), mNormal(vNormal), mOuterRadius(fOuterRadius), mInnerRadius(fInnerRadius)
		{
			update_ring();
		}

		virtual ~SimpleRing() { }

	public:
		virtual void *Clone() { return (GeometricObject *)(new SimpleRing(*this)); }

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
		{
			Vec3f OP = inRay.O() - mPos;
			float A = -Dot(OP, mNormal);
			float B = Dot(inRay.D(), mNormal);
			float t = A / B;

			if ((t > tmin + KEpsilon()) && (t < tmax))
			{
				Vec3f pt = inRay.O() + t * inRay.D();
				float fDistance = pt.SquareDistance(mPos);
				if ((fDistance >= mSquareInnerRadius) && (fDistance <= mSquareOuterRadius))
				{
					tmax = t;

					rec.hit = true;
					rec.t = t;
					rec.pt = pt;
					rec.n = mNormal;
					rec.pMaterial = nullptr;
					rec.albedo = mColor;
					calc_uv(rec.pt, rec.u, rec.v);

					return true;
				}
			}

			rec.hit = false;
			return false;
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const
		{
			Vec3f OP = inRay.O() - mPos;
			float A = -Dot(OP, mNormal);
			float B = Dot(inRay.D(), mNormal);
			float t = A / B;

			if (t > KEpsilon())
			{
				Vec3f pt = inRay.O() + t * inRay.D();
				float fDistance = pt.SquareDistance(mPos);
				if ((fDistance >= mSquareInnerRadius) && (fDistance <= mSquareOuterRadius))
				{
					tvalue = t;
					return true;
				}
			}

			return false;
		}

	public:
		virtual float Area() const
		{
			return mArea;
		}

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding)
		{
			bounding = AABB(mPos - Vec3f(mOuterRadius, mOuterRadius, mOuterRadius), mPos + Vec3f(mOuterRadius, mOuterRadius, mOuterRadius));
			return true;
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const
		{
			return mNormal;
		}

		virtual Vec3f RandomSamplePoint() const
		{
			return WORLD_ORIGIN;
		}

		virtual void Update(float t)
		{
			update_ring();
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
		void SetColor(Color3f const& c) { mColor = c; }
		Color3f GetColor() const { return mColor; }
		void SetPosition(Vec3f const& v) { mPos = v; }
		Vec3f GetPosition() const { return mPos; }
		void SetInnerRadius(float f) { mInnerRadius = f; }
		float GetInnerRadius() const { return mInnerRadius; }
		void SetOutterRadius(float f) { mOuterRadius = f; }
		float GetOutterRadius() const { return mOuterRadius; }
		void SetNormal(Vec3f const& n) { mNormal = n; }

	public:
		inline static float KEpsilon() { return 0.001f; }

	private:
		inline void update_ring()
		{
			mSquareOuterRadius = mOuterRadius * mOuterRadius;
			mSquareInnerRadius = mInnerRadius * mInnerRadius;
		}

		inline void calc_uv(const Vec3f& pt, float& u, float& v)
		{
			Vec3f vOP_ = pt - mPos;
			Vec3f vRef_;
			bool over_pi_ = false;
			if (RTMath::IsZero(vOP_[2]))
			{
				vRef_.Set(1.0f, 0.0f, 0.0f); // y
				if (pt[1] < mPos[1])
				{
					over_pi_ = true;
				}
			}
			else if (RTMath::IsZero(vOP_[1]))
			{
				vRef_.Set(0.0f, 0.0f, 1.0f); // x
				if (pt[0] < mPos[0])
				{
					over_pi_ = true;
				}
			}
			else if (RTMath::IsZero(vOP_[0]))
			{
				vRef_.Set(0.0f, 1.0f, 0.0f); // z
				if (pt[2] < mPos[2])
				{
					over_pi_ = true;
				}
			}
			else
			{
				return;
			}

			float op_len_ = vOP_.Length();
			v = op_len_ / mOuterRadius;

			float op_dot_ref_ = Dot(vOP_, vRef_);
			u = std::acos(op_dot_ref_ / op_len_) / (2.0f * PI_CONST);

			if (over_pi_)
			{
				u = 1.0f - u;
			}
		}

	private:
		Vec3f mPos;
		Color3f mColor;
		Vec3f mNormal;
		float mOuterRadius;
		float mInnerRadius;

		float mSquareOuterRadius;
		float mSquareInnerRadius;

		float mArea;
	};

	// 
    //
    class SimpleRectangle : public GeometricObject
    {
    public:
        SimpleRectangle(Vec3f const& vPos, Vec3f const& vW, Vec3f const& vH)
                : mPos(vPos), mDirW(vW), mDirH(vH), mColor(1.0f, 0.0f, 0.0f)
        {
            update_rectangle();
        }
        virtual ~SimpleRectangle() { }

    public:
        virtual void *Clone() { return (GeometricObject *)(new SimpleRectangle(*this)); }

    public:
        virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
        {
        /*    Vec3f OA = inRay.O() - mPos;
            float A = -Dot(OA, mNormal);
            float B = Dot(inRay.D(), mNormal);
            float t = A / B;

            if ((t > tmin + KEpsilon()) && (t < tmax))
            {
                Vec3f vH = inRay.O() + inRay.D() * t;
                Vec3f vHP = vH - mPos;

                float fDotW = Dot(vHP, mDirW);
                if (fDotW < 0.0f || fDotW > mSqaureW)
                {
                    rec.hit = false;
                    return false;
                }

                float fDotH = Dot(vHP, mDirH);
                if (fDotH < 0.0f || fDotH > mSqaureH)
                {
                    rec.hit = false;
                    return false;
                }

                tmax = t;
                rec.hit = true;
                rec.t = t;
                rec.pt = vH;
                rec.n = mNormal;
                rec.albedo = mColor;
                rec.pMaterial = nullptr;

                return true;
            }

            rec.hit = false;
            return false;*/

			float t = Dot((mPos - inRay.O()), mNormal) / Dot(inRay.D(), mNormal);

			if (t <= KEpsilon()) {
				return false;
			}

			Vec3f p = inRay.O() + t * inRay.D();
			Vec3f d = p - mPos;

			float ddota = Dot(d, mDirW);

			if (ddota < 0.0f || ddota > mSqaureW)
			{
				return false;
			}

			double ddotb = Dot(d, mDirH);

			if (ddotb < 0.0f || ddotb > mSqaureH)
			{
				return false;
			}
				
			tmax = t;
			rec.hit = true;
			rec.t = t;
			rec.pt = p;
			rec.n = mNormal;
			rec.albedo = mColor;
			rec.pMaterial = nullptr;

			return true;
        }

        virtual bool IntersectP(Ray const& inRay, float& tvalue) const
        {
        /*    Vec3f OA = inRay.O() - mPos;
            float A = -Dot(OA, mNormal);
            float B = Dot(inRay.D(), mNormal);
            float t = A / B;

            if (t > KEpsilon())
            {
                Vec3f vH = inRay.O() + inRay.D() * t;
                Vec3f vHP = vH - mPos;

                float fDotW = Dot(vHP, mDirW);
                if (fDotW < 0.0f || fDotW > mSqaureW)
                {
                    return false;
                }

                float fDotH = Dot(vHP, mDirH);
                if (fDotH < 0.0f || fDotH > mSqaureH)
                {
                    return false;
                }

                tvalue = t;
                return true;
            }

            return false;*/

			float t = Dot((mPos - inRay.O()), mNormal) / Dot(inRay.D(), mNormal);

			if (t <= KEpsilon()) {
				return false;
			}

			Vec3f p = inRay.O() + t * inRay.D();
			Vec3f d = p - mPos;

			float ddota = Dot(d, mDirW);

			if (ddota < 0.0f || ddota > mSqaureW)
			{
				return false;
			}

			double ddotb = Dot(d, mDirH);

			if (ddotb < 0.0f || ddotb > mSqaureH)
			{
				return false;
			}

			tvalue = t;
			return true;

        }

    public:
        virtual float Area() const
        {
            return mArea;
        }

        virtual bool GetBoundingBox(float t0, float t1, AABB& bounding)
        {
            static const float RECT_DELTA = 0.0001f;

            float dOtherSideX = mPos.X() + mDirW.X() + mDirH.X();
            float dOtherSideY = mPos.Y() + mDirW.Y() + mDirH.Y();
            float dOtherSideZ = mPos.Z() + mDirW.Z() + mDirH.Z();

            bounding = AABB(std::min<float>(mPos.X(), dOtherSideX) - RECT_DELTA, std::max<float>(mPos.X(), dOtherSideX) + RECT_DELTA,
                            std::min<float>(mPos.Y(), dOtherSideY) - RECT_DELTA, std::max<float>(mPos.Y(), dOtherSideY) + RECT_DELTA,
                            std::min<float>(mPos.Z(), dOtherSideZ) - RECT_DELTA, std::max<float>(mPos.Z(), dOtherSideZ) + RECT_DELTA);

            return true;
        }

        virtual Vec3f GetNormal(const HitRecord& rec) const
        {
            return mNormal;
        }

        virtual Vec3f RandomSamplePoint() const
        {
            Vec3f sample_point = mPos + Random::frand48() * mDirW + Random::frand48() * mDirH;
            return sample_point;
        }

        virtual void Update(float t)
        {
            update_rectangle();
        }

        virtual bool IsCompound() const
        {
            return false;
        }

        virtual float PDFValue(Vec3f const& o, Vec3f const& v) const
        {
            Ray testRay(o, v, 0.0f);
            float tvalue = 0.0f;
            if (SimpleRectangle::IntersectP(testRay, tvalue))
            {
                float len = v.Length();
                float dist_squd = tvalue * tvalue * len * len;
                float cosine_ = std::fabs(Dot(v, mNormal) / len);

                return dist_squd / (cosine_ * mArea);
            }

            return 0.0f;
        }

        virtual Vec3f RandomSampleDirection(Vec3f const& v) const
        {
            Vec3f pt = RandomSamplePoint();
            return (pt - v);
        }

    public:
        inline void SetColor(Color3f const& c) { mColor = c; }
        inline Color3f GetColor() const { return mColor; }
        inline void SetPosition(Vec3f const& v) { mPos = v; }
        inline Vec3f GetPosition() const { return mPos; }
        inline void SetDirW(Vec3f const& w) { mDirW = w; }
        inline void SetDirH(Vec3f const& h) { mDirH = h; }
        inline Vec3f GetDirW() const { return mDirW; }
        inline Vec3f GetDirH() const { return mDirH; }
        inline void SetNormal(Vec3f const& n) { mNormal = n; }

    public:
        inline static float KEpsilon() { return 0.001f; }

    private:
        inline void update_rectangle()
        {
            mNormal = Cross(mDirW, mDirH);
            mNormal.MakeUnit();

            mSqaureW = Dot<float>(mDirW, mDirW);
            mSqaureH = Dot<float>(mDirH, mDirH);

            mLenOfW = mDirW.Length();
            mLenOfH = mDirH.Length();

            mArea = mLenOfW * mLenOfH;
        }

    protected:
        Vec3f mPos;
        Vec3f mNormal;
        Vec3f mDirW;
        Vec3f mDirH;
        Color3f mColor;

        float mSqaureW;
        float mSqaureH;
        float mLenOfW;
        float mLenOfH;

        float mArea;

    };

    class XYRect : public SimpleRectangle
    {
    public:
        XYRect(Vec3f const& vPos, bool flip, int w, int h)
                : SimpleRectangle(vPos, Vec3f(w, 0, 0), Vec3f(0, h, 0))
        {
            mNormal = (flip ? Vec3f(0, 0, -1) : Vec3f(0, 0, 1));
        }
        virtual ~XYRect() { }

    public:
        virtual void *Clone() { return (GeometricObject *)(new XYRect(*this)); }

    public:
        virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
        {
            if (SimpleRectangle::HitTest(inRay, tmin, tmax, rec))
            {
                rec.u = (rec.pt.X() - mPos.X()) / mLenOfW;
                rec.v = 1.0f - (rec.pt.Y() - mPos.Y()) / mLenOfH;

                return true;
            }

            return false;
        }

    public:
        virtual Vec3f RandomSamplePoint() const
        {
            Vec3f sample_point;
            sample_point.Set(
                    mPos.X() + Random::frand48() * mLenOfW,
                    mPos.Y() + Random::frand48() * mLenOfH,
                    mPos.Z());
            return sample_point;
        }

        virtual float PDFValue(Vec3f const& o, Vec3f const& v) const
        {
            return SimpleRectangle::PDFValue(o, v);
        }

        virtual Vec3f RandomSampleDirection(Vec3f const& v) const
        {
            Vec3f pt = XYRect::RandomSamplePoint();
            return (pt - v);
        }
    };

    class XZRect : public SimpleRectangle
    {
    public:
        XZRect(Vec3f const& vPos, bool flip, int w, int h)
                : SimpleRectangle(vPos, Vec3f(w, 0, 0), Vec3f(0, 0, h))
        {
            mNormal = (flip ? Vec3f(0, -1, 0) : Vec3f(0, 1, 0));
        }
        virtual ~XZRect() { }

    public:
        virtual void *Clone() { return (GeometricObject *)(new XZRect(*this)); }

    public:
        virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
        {
            if (SimpleRectangle::HitTest(inRay, tmin, tmax, rec))
            {
                rec.u = (rec.pt.X() - mPos.X()) / mLenOfW;
                rec.v = 1.0f - (rec.pt.Z() - mPos.Z()) / mLenOfH;

                return true;
            }

            return false;
        }

    public:
        virtual Vec3f RandomSamplePoint() const
        {
            Vec3f sample_point;
            sample_point.Set(
                    mPos.X() + Random::frand48() * mLenOfW,
                    mPos.Y(),
                    mPos.Z() + Random::frand48() * mLenOfH);
            return sample_point;
        }

        virtual float PDFValue(Vec3f const& o, Vec3f const& v) const
        {
            return SimpleRectangle::PDFValue(o, v);
        }

        virtual Vec3f RandomSampleDirection(Vec3f const& v) const
        {
            Vec3f pt = XZRect::RandomSamplePoint();
            return (pt - v);
        }
    };

    class YZRect : public SimpleRectangle
    {
    public:
        YZRect(Vec3f const& vPos, bool flip, int w, int h)
                : SimpleRectangle(vPos, Vec3f(0, 0, w), Vec3f(0, h, 0))
        {
            mNormal = (flip ? Vec3f(-1, 0, 0) : Vec3f(1, 0, 0));
        }
        virtual ~YZRect() { }

    public:
        virtual void *Clone() { return (GeometricObject *)(new YZRect(*this)); }

    public:
        virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
        {
            if (SimpleRectangle::HitTest(inRay, tmin, tmax, rec))
            {
                rec.u = (rec.pt.Z() - mPos.Z()) / mLenOfW;
                rec.v = 1.0f - (rec.pt.Y() - mPos.Y()) / mLenOfH;

                return true;
            }

            return false;
        }

    public:
        virtual Vec3f RandomSamplePoint() const
        {
            Vec3f sample_point;
            sample_point.Set(
                    mPos.X(),
                    mPos.Y() + Random::frand48() * mLenOfW,
                    mPos.Z() + Random::frand48() * mLenOfH);
            return sample_point;
        }

        virtual float PDFValue(Vec3f const& o, Vec3f const& v) const
        {
            return SimpleRectangle::PDFValue(o, v);
        }

        virtual Vec3f RandomSampleDirection(Vec3f const& v) const
        {
            Vec3f pt = YZRect::RandomSamplePoint();
            return (pt - v);
        }
    };

	//
	class SimpleTriangle : public GeometricObject
	{
	public:
		SimpleTriangle(Vec3f const& a, Vec3f const& b, Vec3f const& c)
			: v0(a), v1(b), v2(c), mColor(1.0f, 0.0f, 0.0f)
		{
			update_triangle();
		}
		virtual ~SimpleTriangle() { }

	public:
		virtual void *Clone() { return (GeometricObject *)(new SimpleTriangle(*this)); }

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
		{
			float beta, gamma;
			bool is_hit = SimpleTriangle::HitTestImpl(v0, v1, v2, vNormal, mColor, beta, gamma, inRay, tmin, tmax, rec);
			return is_hit;
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const
		{
			bool isHit = SimpleTriangle::IntersectPImpl(v0, v1, v2, inRay, tvalue);
			return isHit;
		}

	public:
		virtual float Area() const
		{
			return mArea;
		}

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding)
		{
			SimpleTriangle::GetBoundingBoxImpl(v0, v1, v2, bounding);
			return true;
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const
		{
			return vNormal;
		}

		virtual Vec3f RandomSamplePoint() const
		{
            return (SimpleTriangle::RandomSamplePointImpl(v0, v1, v2));
		}

		virtual void Update(float t)
		{
			this->ComputeNormal();
			this->ComputeArea();
		}

		virtual bool IsCompound() const
		{
			return false;
		}

		virtual float PDFValue(Vec3f const& o, Vec3f const& v) const
		{
            return (1.0f / mArea);
		}

		virtual Vec3f RandomSampleDirection(Vec3f const& v) const
		{
            Vec3f pt = RandomSamplePoint();
            return (pt - v);
		}

	public:
		void SetColor(Color3f const& c) { mColor = c; }
		Color3f GetColor() const { return mColor; }

		void SetVertex(Vec3f const& v0, Vec3f const& v1, Vec3f const& v2)
		{
			this->v0 = v0;
			this->v1 = v1;
			this->v2 = v2;
		}
		void GetVertex(Vec3f& v0, Vec3f& v1, Vec3f& v2) const
		{
			v0 = this->v0;
			v1 = this->v1;
			v2 = this->v2;
		}

	public:
		inline void ComputeNormal()
		{
			SimpleTriangle::ComputeNormalImpl(v0, v1, v2, vNormal);
		}

		inline void ComputeArea()
		{
			SimpleTriangle::ComputeAreaImpl(v0, v1, v2, mArea);
		}
		inline float GetArea() const { return mArea; }

	public:
		inline static void GetBoundingBoxImpl(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, AABB& bounding) {
			static const float TRI_DELTA = 0.000001f;

			bounding = AABB(std::min<float>(std::min<float>(v0.X(), v1.X()), v2.X()) - TRI_DELTA, std::max<float>(std::max<float>(v0.X(), v1.X()), v2.X()) + TRI_DELTA,
				std::min<float>(std::min<float>(v0.Y(), v1.Y()), v2.Y()) - TRI_DELTA, std::max<float>(std::max<float>(v0.Y(), v1.Y()), v2.Y()) + TRI_DELTA,
				std::min<float>(std::min<float>(v0.Z(), v1.Z()), v2.Z()) - TRI_DELTA, std::max<float>(std::max<float>(v0.Z(), v1.Z()), v2.Z()) + TRI_DELTA);
		}

		inline static bool HitTestImpl(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2,
			const Vec3f& normal, const Color3f& color, float& out_beta, float& out_gamma,
			Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec) {

			float a = v0.X() - v1.X(), b = v0.X() - v2.X(), c = inRay.D().X(), d = v0.X() - inRay.O().X();
			float e = v0.Y() - v1.Y(), f = v0.Y() - v2.Y(), g = inRay.D().Y(), h = v0.Y() - inRay.O().Y();
			float i = v0.Z() - v1.Z(), j = v0.Z() - v2.Z(), k = inRay.D().Z(), l = v0.Z() - inRay.O().Z();

			float m = f * k - g * j, n = h * k - g * l, p = f * l - h * j;
			float q = g * i - e * k, s = e * j - f * i;

			float inv_denom = 1.0f / (a * m + b * q + c * s);

			float e1 = d * m - b * n - c * p;
			float beta = e1 * inv_denom;

			if (beta < 0.0f)
				return (false);

			float r = r = e * l - h * i;
			float e2 = a * n + d * q + c * r;
			float gamma = e2 * inv_denom;

			if (gamma < 0.0f)
				return (false);

			if (beta + gamma > 1.0f)
				return (false);

			float e3 = a * p - b * r + d * s;
			float t = e3 * inv_denom;

			if (t < tmin + KEpsilon() || t >= tmax)
				return (false);

			out_beta = beta;
			out_gamma = gamma;

			tmax = t;
			rec.hit = true;
			rec.t = t;
			rec.n = normal;
			rec.pt = inRay.O() + t * inRay.D();
			rec.albedo = color;
			rec.pMaterial = nullptr;

			return (true);
		}

		inline static bool IntersectPImpl(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, Ray const& inRay, float& tvalue)
		{
			float a = v0.X() - v1.X(), b = v0.X() - v2.X(), c = inRay.D().X(), d = v0.X() - inRay.O().X();
			float e = v0.Y() - v1.Y(), f = v0.Y() - v2.Y(), g = inRay.D().Y(), h = v0.Y() - inRay.O().Y();
			float i = v0.Z() - v1.Z(), j = v0.Z() - v2.Z(), k = inRay.D().Z(), l = v0.Z() - inRay.O().Z();

			float m = f * k - g * j, n = h * k - g * l, p = f * l - h * j;
			float q = g * i - e * k, s = e * j - f * i;

			float inv_denom = 1.0f / (a * m + b * q + c * s);

			float e1 = d * m - b * n - c * p;
			float beta = e1 * inv_denom;

			if (beta < 0.0f)
				return (false);

			float r = r = e * l - h * i;
			float e2 = a * n + d * q + c * r;
			float gamma = e2 * inv_denom;

			if (gamma < 0.0f)
				return (false);

			if (beta + gamma > 1.0f)
				return (false);

			float e3 = a * p - b * r + d * s;
			float t = e3 * inv_denom;

			if (t < KEpsilon())
				return (false);

			tvalue = t;
			return (true);
		}

		inline static void ComputeNormalImpl(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, Vec3f& normal) {
			normal = Cross(v1 - v0, v2 - v0);
			normal.MakeUnit();
		}

		inline static void ComputeAreaImpl(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, float& area) {
			area = Cross(v1 - v2, v2 - v0).Length() * 0.5f;
		}

        inline static Vec3f RandomSamplePointImpl(Vec3f const& v0, Vec3f const& v1, Vec3f const& v2) {
            float u = Random::frand48();
            float v = Random::frand48();

            float su = (float)std::sqrt(u);
            Vec3f pt = (1.0f - su) * v0 + (1.0f - v) * su * v1 + v * su * v2;
            // u: 1-su
            // v: v*su
            // so, 1 - u - v = 1 - (1 - su) - v*su == su - v*su == (1 - v)*su that is exactly v1's factor

            return pt;
        }

	public:
		inline static float KEpsilon() { return 0.001f; }

	private:
		inline void update_triangle()
		{
			this->ComputeNormal();
			this->ComputeArea();
		}

	private:
		Vec3f v0, v1, v2;
		Vec3f vNormal;
		Color3f mColor;
		float mArea;

	};

	//
	class SimpleBox : public GeometricObject
	{
	public:
		SimpleBox(const float _x0, const float _x1,
			const float _y0, const float _y1,
			const float _z0, const float _z1)
			: x0(_x0), x1(_x1), y0(_y0), y1(_y1), z0(_z0), z1(_z1)
		{

		}

		virtual ~SimpleBox() { }

	public:
		virtual void *Clone() { return (GeometricObject *)(new SimpleBox(*this)); }

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
		{
			float ox = inRay.O().X(); float oy = inRay.O().Y(); float oz = inRay.O().Z();
			float dx = inRay.D().X(); float dy = inRay.D().Y(); float dz = inRay.D().Z();

			float tx_min, ty_min, tz_min;
			float tx_max, ty_max, tz_max;

			float a = 1.0f / dx;
			if (a >= 0) {
				tx_min = (x0 - ox) * a;
				tx_max = (x1 - ox) * a;
			}
			else {
				tx_min = (x1 - ox) * a;
				tx_max = (x0 - ox) * a;
			}

			float b = 1.0f / dy;
			if (b >= 0) {
				ty_min = (y0 - oy) * b;
				ty_max = (y1 - oy) * b;
			}
			else {
				ty_min = (y1 - oy) * b;
				ty_max = (y0 - oy) * b;
			}

			float c = 1.0f / dz;
			if (c >= 0) {
				tz_min = (z0 - oz) * c;
				tz_max = (z1 - oz) * c;
			}
			else {
				tz_min = (z1 - oz) * c;
				tz_max = (z0 - oz) * c;
			}

			float t0, t1;

			int face_in, face_out;
			// find largest entering t value

			if (tx_min > ty_min) {
				t0 = tx_min;
				face_in = (a >= 0.0f) ? 0 : 3;
			}
			else {
				t0 = ty_min;
				face_in = (b >= 0.0f) ? 1 : 4;
			}

			if (tz_min > t0) {
				t0 = tz_min;
				face_in = (c >= 0.0f) ? 2 : 5;
			}

			// find smallest exiting t value

			if (tx_max < ty_max) {
				t1 = tx_max;
				face_out = (a >= 0.0f) ? 3 : 0;
			}
			else {
				t1 = ty_max;
				face_out = (b >= 0.0f) ? 4 : 1;
			}

			if (tz_max < t1) {
				t1 = tz_max;
				face_out = (c >= 0.0f) ? 5 : 2;
			}

			bool is_hit = (t0 < t1 && t1 > KEpsilon());
			if (is_hit)
			{
				bool ret = false;
				if (t0 > KEpsilon()) {
					tmax = t0;
					get_normal(face_in, rec.n);
					ret = true;
				}
				else {
				//	if (t1 < tmax) {
						tmax = t1;
						get_normal(face_out, rec.n);
						ret = true;
				//	}
				}

				rec.hit = ret;
				if (rec.hit) {
					rec.t = tmax;
					rec.pt = inRay.O() + rec.t * inRay.D();
					rec.pMaterial = nullptr;
					rec.albedo = mColor;
				}

				return ret;
			}

			rec.hit = false;
			return false;
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const
		{
			float ox = inRay.O().X(); float oy = inRay.O().Y(); float oz = inRay.O().Z();
			float dx = inRay.D().X(); float dy = inRay.D().Y(); float dz = inRay.D().Z();

			float tx_min, ty_min, tz_min;
			float tx_max, ty_max, tz_max;

			float a = 1.0f / dx;
			if (a >= 0) {
				tx_min = (x0 - ox) * a;
				tx_max = (x1 - ox) * a;
			}
			else {
				tx_min = (x1 - ox) * a;
				tx_max = (x0 - ox) * a;
			}

			float b = 1.0f / dy;
			if (b >= 0) {
				ty_min = (y0 - oy) * b;
				ty_max = (y1 - oy) * b;
			}
			else {
				ty_min = (y1 - oy) * b;
				ty_max = (y0 - oy) * b;
			}

			float c = 1.0f / dz;
			if (c >= 0) {
				tz_min = (z0 - oz) * c;
				tz_max = (z1 - oz) * c;
			}
			else {
				tz_min = (z1 - oz) * c;
				tz_max = (z0 - oz) * c;
			}

			float t0, t1;
			if (tx_min > ty_min) {
				t0 = tx_min;
			}
			else {
				t0 = ty_min;
			}

			if (tz_min > t0) {
				t0 = tz_min;
			}

			if (tx_max < ty_max) {
				t1 = tx_max;
			}
			else {
				t1 = ty_max;
			}

			if (tz_max < t1) {
				t1 = tz_max;
			}

			bool is_hit = (t0 < t1 && t1 > KEpsilon());
			if (is_hit)
			{
				bool ret = false;
				if (t0 > KEpsilon()) {
					tvalue = t0;
					ret = true;
				}
				else {
					//	if (t1 < tmax) {
					tvalue = t1;
					ret = true;
					//	}
				}

				return ret;
			}

			return is_hit;
		}

	public:
		virtual float Area() const
		{
			return mArea;
		}

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding)
		{
			bounding = AABB(x0, x1, y0, y1, z0, z1);
			return true;
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const
		{
			return WORLD_ORIGIN;
		}

		virtual Vec3f RandomSamplePoint() const
		{
			return WORLD_ORIGIN;
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
		void SetColor(Color3f const& c) { mColor = c; }
		Color3f GetColor() const { return mColor; }

		void SetBoundingBox(const AABB& aabb)
		{
			x0 = aabb.mX0;
			y0 = aabb.mY0;
			z0 = aabb.mZ0;

			x1 = aabb.mX1;
			y1 = aabb.mY1;
			z1 = aabb.mZ1;
		}
		
	public:
		inline static float KEpsilon() { return 0.001f; }

	private:
		inline bool is_inside(const Vec3f& p) const
		{
			return ((p.X() > x0 && p.X() < x1) && (p.Y() > y0 && p.Y() < y1) && (p.Z() > z0 && p.Z() < z1));
		}

		inline void get_normal(const int face_hit, Vec3f& vNormal)
		{
			switch (face_hit)
			{
			case 0:
				vNormal.Set(-1, 0, 0);
				break;
			case 1:
				vNormal.Set(0, -1, 0);
				break;
			case 2:
				vNormal.Set(0, 0, -1);
				break;
			case 3:
				vNormal.Set(1, 0, 0);
				break;
			case 4:
				vNormal.Set(0, 1, 0);
				break;
			case 5:
				vNormal.Set(0, 0, 1);
				break;
			default:
				break;
			}
		}

	private:
		float x0, x1, y0, y1, z0, z1;
		Color3f mColor;
		float mArea;

	};

	//
	class SimpleCylinder : public GeometricObject // Y-axis aligned cylinder.
	{
	public:
		SimpleCylinder()
			: mCenter(0.0f, 0.0f, 0.0f), mRadius(30.0f), mY0(0.0f), mY1(60.0f), mColor(1.0f, 0.0f, 0.0f),
			  mPart(false), mTheta0(0.0f), mTheta1(0.0f) { }
		SimpleCylinder(Vec3f const& center, float r, float y0, float y1, Color3f const& c = Color3f(1.0f, 0.0f, 0.0f))
			: mCenter(center), mRadius(r), mY0(y0), mY1(y1), mColor(c),
              mPart(false), mTheta0(0.0f), mTheta1(0.0f) { }
		virtual ~SimpleCylinder() { }

	public:
		virtual void *Clone() { return (SimpleCylinder *)(new SimpleCylinder(*this)); }

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

			float A = (dx * dx) + (dz * dz);
			float B = 2.0f * (x0 * dx + z0 * dz);
			float C = x0 * x0 + z0 * z0 - mRadius * mRadius;

			bool is_hit = false;
            bool check_part = true;
			float t0 = (-B - std::sqrt(B * B - 4.0f * A * C)) / (2.0f * A);
			if (t0 > tmin + KEpsilon() && t0 < tmax)
			{
				rec.pt = inRay.O() + t0 * inRay.D();
				if (rec.pt.Y() >= mY0 && rec.pt.Y() <= mY1)
				{
				    if (mPart)
                    {
				        if (!this->check_in_theta_range(rec.pt))
                        {
				            check_part = false;
                        }
                    }

				    if (check_part)
                    {
                        is_hit = true;
                        tmax = t0;

                        rec.hit = true;
                        rec.t = t0;
                        rec.n = GetNormal(rec);
                        if (Dot(rec.n, inRay.D()) > 0.0f)
                        {
                            rec.n = -rec.n;
                        }
                        rec.albedo = mColor;
                        rec.pMaterial = nullptr;
                        this->calc_uv(rec.pt, rec.u, rec.v);
                    }
				}
				else
				{
					is_hit = false;
				}
			}
			
			if (!is_hit)
			{
				float t1 = (-B + std::sqrt(B * B - 4.0f * A * C)) / (2.0f * A);
				if (t1 > tmin + KEpsilon() && t1 < tmax)
				{
					rec.pt = inRay.O() + t1 * inRay.D();
					if (rec.pt.Y() >= mY0 && rec.pt.Y() <= mY1)
					{
                        if (mPart)
                        {
                            if (!this->check_in_theta_range(rec.pt))
                            {
                                return false;
                            }
                        }

                        tmax = t1;
                        is_hit = true;

						rec.hit = true;
						rec.t = t1;
						rec.n = GetNormal(rec);
						if (Dot(rec.n, inRay.D()) > 0.0f)
						{
							rec.n = -rec.n;
						}
						rec.albedo = mColor;
						rec.pMaterial = nullptr;
						this->calc_uv(rec.pt, rec.u, rec.v);
					}
					else
					{
						is_hit = false;
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

            float A = (dx * dx) + (dz * dz);
            float B = 2.0f * (x0 * dx + z0 * dz);
            float C = x0 * x0 + z0 * z0 - mRadius * mRadius;

			bool is_hit = false;
			bool check_part = true;
			float t0 = (-B - std::sqrt(B * B - 4.0f * A * C)) / (2.0f * A);
			if (t0 > KEpsilon())
			{
                Vec3f pt = inRay.O() + t0 * inRay.D();
                if (mPart)
                {
                    if (!this->check_in_theta_range(pt))
                    {
                    //    return false;
                    //    goto L0;
                        check_part = false;
                    }
                }

                if (check_part)
                {
                    is_hit = true;
                    if (pt.Y() >= mY0 && pt.Y() <= mY1)
                    {
                        tvalue = t0;
                    }
                    else
                    {
                        is_hit = false;
                    }
                }
			}
			if (!is_hit)
			{
				float t1 = (-B + std::sqrt(B * B - 4.0f * A * C)) / (2.0f * A);
				if (t1 > KEpsilon())
				{
                    Vec3f pt = inRay.O() + t1 * inRay.D();
                    if (mPart)
                    {
                        if (!this->check_in_theta_range(pt))
                        {
                            return false;
                        }
                    }

					is_hit = true;
					if (pt.Y() >= mY0 && pt.Y() <= mY1)
					{
						tvalue = t1;
					}
					else
					{
						is_hit = false;
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
			Vec3f vmin_(-mRadius, mY0, -mRadius);
			Vec3f vmax_(mRadius, mY1, mRadius);
			bounding = AABB(vmin_, vmax_);

			return true;
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const
		{
		    Vec3f pt = rec.pt - mCenter;
			return Vec3f(pt.X() / mRadius, 0.0f, pt.Z() / mRadius);
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
		inline void SetRadius(float r) { mRadius = r; }
		inline void SetYRange(float y0, float y1) { mY0 = y0; mY1 = y1; }
		inline void SetColor(float r, float g, float b)
		{
			mColor.Set(r, g, b);
		}

		inline void SetPartParams(bool part, float theta0, float theta1)
        {
            mPart = part;
            if (mPart)
            {
                mTheta0 = ANG2RAD(theta0);
                mTheta1 = ANG2RAD(theta1);
            }
        }

	public:
		inline static float KEpsilon() { return 0.001f; }

	private:
		inline void calc_uv(const Vec3f& pt, float& u, float& v)
		{
//			v = (pt.Y() - mY0) / (mY1 - mY0);
//
//			Vec3f po(pt.X(), 0.0f, pt.Y());
//			Vec3f sz(0.0f, 0.0f, 1.0f);
//			float d = po.Length();
//			if (RTMath::IsZero(d)) {
//			    d = 0.001f;
//			}
//			u = std::acos(Dot(po, sz) / d) / TWO_PI_CONST;
//
//			if (pt.X() < 0.0f)
//			{
//				u = 1.0f - u;
//			}
//			v = 1.0f - v;

            float phi = std::acos(pt.X() * (1.0f / mRadius));
            if (RTMath::IsNan(phi)) {
                phi = 1e-4;
            }
            if (pt.Z() < 0.0f) {
                phi = TWO_PI_CONST - phi;
            }
            u = phi * INV_TWO_PI_CONST;
            v = (pt.Y() - mY0) / (mY1 - mY0);
            v = 1.0f - v;

		//	std::cout << phi << std::endl;
		}

		inline bool check_in_theta_range(const Vec3f& pt) const
        {
		    Vec3f po(pt.X(), 0.0f, pt.Z());
		    Vec3f ox(1.0f, 0.0f, 0.0f);
            float cos_theta = Dot(po, ox)/(po.Length());
            float theta = std::acos(cos_theta);

            if (pt.Z() > 0.0f)
            {
                theta = TWO_PI_CONST - theta;
            }

            if (theta >= mTheta0 && theta <= mTheta1)
            {
                return true;
            }

            return false;
        }

	private:
		Vec3f mCenter;
		float mRadius;
		float mY0;
		float mY1;
		Color3f mColor;

		bool mPart;
		float mTheta0;
		float mTheta1;
	};

	//
	class SimpleCone : public GeometricObject
	{
	public:
		SimpleCone()
			: mCenter(0.0f, 0.0f, 0.0f), mRadius(30.0f), mY0(0.0f), mY1(60.0f), mColor(1.0f, 0.0f, 0.0f),
              mPart(false), mTheta0(0.0f), mTheta1(0.0f), mInterceptionH(0.0f)
		{
			calc_H();
		}
		SimpleCone(Vec3f const& center, float r, float y0, float y1, const Color3f& c = Color3f(1.0f, 0.0f, 0.0f))
			: mCenter(center), mRadius(r), mY0(y0), mY1(y1), mColor(c),
              mPart(false), mTheta0(0.0f), mTheta1(0.0f), mInterceptionH(0.0f)
		{
			calc_H();
		}
		virtual ~SimpleCone() { }

	public:
		virtual void *Clone() { return (SimpleCone *)(new SimpleCone(*this)); }

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
			float hdivr = mH / mRadius;

			float A = hdivr*hdivr*dx*dx - dy*dy + hdivr*hdivr*dz*dz;
			float B = 2.0f*hdivr*hdivr*x0*dx - 2.0f*y0*dy + 2.0f*mH*dy + 2.0f*hdivr*hdivr*z0*dz;
			float C = hdivr*hdivr*x0*x0 - y0*y0 + 2.0f*mH*y0 - mH*mH + hdivr*hdivr*z0*z0;

			bool is_hit = false;
            bool check_part = true;
			float t0 = (-B - std::sqrt(B * B - 4.0f * A * C)) / (2.0f * A);
			if (t0 > tmin + KEpsilon() && t0 < tmax)
			{
				rec.pt = inRay.O() + t0 * inRay.D();
				if (rec.pt.Y() >= mY0 && rec.pt.Y() <= mY1)
				{
                    if (mPart)
                    {
                        if (!this->check_in_theta_range(rec.pt))
                        {
                            check_part = false;
                        }
                    }

                    if (check_part)
                    {
                        tmax = t0;
                        is_hit = true;

                        rec.hit = true;
                        rec.t = t0;
                        rec.n = GetNormal(rec);
                        if (mPart)
                        {
                            if (Dot(rec.n, inRay.D()) > 0.0f)
                            {
                                rec.n = -rec.n;
                            }
                        }
                        rec.albedo = mColor;
                        rec.pMaterial = nullptr;
                        this->calc_uv(rec.pt, rec.u, rec.v);
                    }
				}
				else
				{
					is_hit = false;
				}
			}

			if (!is_hit)
			{
				float t1 = (-B + std::sqrt(B * B - 4.0f * A * C)) / (2.0f * A);
				if (t1 > tmin + KEpsilon() && t1 < tmax)
				{
					rec.pt = inRay.O() + t1 * inRay.D();
					if (rec.pt.Y() >= mY0 && rec.pt.Y() <= mY1)
					{
                        if (mPart)
                        {
                            if (!this->check_in_theta_range(rec.pt))
                            {
                                return false;
                            }
                        }

                        tmax = t1;
                        is_hit = true;

						rec.hit = true;
						rec.t = t1;
						rec.n = GetNormal(rec);
                        if (mPart)
                        {
                            if (Dot(rec.n, inRay.D()) > 0.0f)
                            {
                                rec.n = -rec.n;
                            }
                        }
						rec.albedo = mColor;
						rec.pMaterial = nullptr;
						this->calc_uv(rec.pt, rec.u, rec.v);
					}
					else
					{
						is_hit = false;
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
			float hdivr = mH / mRadius;

			float A = hdivr*hdivr*dx*dx - dy*dy + hdivr*hdivr*dz*dz;
			float B = 2.0f*hdivr*hdivr*x0*dx - 2.0f*y0*dy + 2.0f*mH*dy + 2.0f*hdivr*hdivr*z0*dz;
			float C = hdivr*hdivr*x0*x0 - y0*y0 + 2.0f*mH*y0 - mH*mH + hdivr*hdivr*z0*z0;

			bool is_hit = false;
            bool check_part = true;
			float t0 = (-B - std::sqrt(B * B - 4.0f * A * C)) / (2.0f * A);
			if (t0 > KEpsilon())
			{
				Vec3f pt = inRay.O() + t0 * inRay.D();
				if (pt.Y() >= mY0 && pt.Y() <= mY1)
				{
                    if (mPart)
                    {
                        if (!this->check_in_theta_range(pt))
                        {
                            check_part = false;
                        }
                    }

                    if (check_part)
                    {
                        is_hit = true;
                        tvalue = t0;
                    }
				}
				else
				{
					is_hit = false;
				}
			}
            if (!is_hit)
			{
				float t1 = (-B + std::sqrt(B * B - 4.0f * A * C)) / (2.0f * A);
				if (t1 > KEpsilon())
				{
					Vec3f pt = inRay.O() + t1 * inRay.D();
					if (pt.Y() >= mY0 && pt.Y() <= mY1)
					{
                        if (mPart)
                        {
                            if (!this->check_in_theta_range(pt))
                            {
                                return false;
                            }
                        }

                        is_hit = true;
						tvalue = t1;
					}
					else
					{
						is_hit = false;
					}
				}
			}

			return is_hit;
		}

	public:
		virtual float Area() const { return 0.0f; }

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding)
		{
			Vec3f vmin_(-mRadius, mY0, -mRadius);
			Vec3f vmax_(mRadius, mY1, mRadius);
			bounding = AABB(vmin_, vmax_);

			return true;
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const
		{
		    Vec3f pt = rec.pt - mCenter;
			Vec3f norm;
			norm.Set((2.0f * mH * rec.pt.X()) / mRadius, -2.0f*(rec.pt.Y() - mH), (2.0f * mH * rec.pt.Z()) / mRadius);
			norm.MakeUnit();
			return norm;
		}

		virtual Vec3f RandomSamplePoint() const
		{
			return Vec3f(0.0f, 0.0f, 0.0f);
		}

		virtual void Update(float t) { }

		virtual bool IsCompound() const { return false; }

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
		inline void SetRadius(float r) { mRadius = r; }
		inline void SetYRange(float y0, float y1) { mY0 = y0; mY1 = y1; }
		inline void SetColor(float r, float g, float b)
		{
			mColor.Set(r, g, b);
		}

        inline void SetPartParams(bool part, float theta0, float theta1, float interceptionH)
        {
            mPart = part;
            if (mPart)
            {
                mTheta0 = ANG2RAD(theta0);
                mTheta1 = ANG2RAD(theta1);

				mInterceptionH = interceptionH;
            }
        }

	public:
		inline static float KEpsilon() { return 0.001f; }

	private:
		inline void calc_H() { mH = mY1 - mY0; }
		inline void calc_uv(const Vec3f& pt, float& u, float& v)
		{
//			v = (pt.Y() - mY0) / (mY1 - mY0);
//
//			Vec3f po(pt.X(), 0.0f, pt.Y());
//			Vec3f sz(0.0f, 0.0f, 1.0f);
//			float d = po.Length();
//			if (RTMath::IsZero(d)) {
//			    d = 0.001f;
//			}
//			u = std::acos(Dot(po, sz) / d) / (2.0f*PI_CONST);
//
//			if (pt.X() < 0.0f)
//			{
//				u = 1.0f - u;
//			}
//			v = 1.0f - v;

            float phi = std::acos(pt.X() * (1.0f / mRadius));
            if (RTMath::IsNan(phi)) {
                phi = 1e-4;
            }
            if (pt.Z() < 0.0f) {
                phi = TWO_PI_CONST - phi;
            }
            u = phi * INV_TWO_PI_CONST;
            v = (pt.Y() - mY0) / (mY1 - mY0);
            v = 1.0f - v;
		}

        inline bool check_in_theta_range(const Vec3f& pt) const
        {
			//
			//if (mInterceptionH > 0.0f)
			{
				if (pt.Y() - mY0 > mInterceptionH)
				{
					return false;
				}
			}

			//
            Vec3f po(pt.X(), 0.0f, pt.Z());
            Vec3f ox(1.0f, 0.0f, 0.0f);
            float cos_theta = Dot(po, ox)/(po.Length());
            float theta = std::acos(cos_theta);

            if (pt.Z() > 0.0f)
            {
                theta = TWO_PI_CONST - theta;
            }

            if (theta >= mTheta0 && theta <= mTheta1)
            {
                return true;
            }

            return false;
        }

	private:
		Vec3f mCenter;
		float mRadius;
		float mY0;
		float mY1;
		Color3f mColor;

		float mH;

        bool mPart;
        float mTheta0;
        float mTheta1;
		float mInterceptionH;

	};

	//
	class SimpleTorus : public GeometricObject
	{
	public:
		SimpleTorus()
			: mCenter(0.0f, 0.0f, 0.0f), mSweptRadius(30.0f), mTubeRadius(10.0f), mColor(1.0f, 0.0f, 0.0f),
              mPartTorus(false), mTheta0(0.0f), mTheta1(0.0f), mPhi0(0.0f), mPhi1(0.0f)
		{

		}
		SimpleTorus(Vec3f const& center, float sweptRadius, float tubeRadius, const Color3f c = Color3f(1.0f, 0.0f, 0.0f))
			: mCenter(center), mSweptRadius(sweptRadius), mTubeRadius(tubeRadius), mColor(c),
              mPartTorus(false), mTheta0(0.0f), mTheta1(0.0f), mPhi0(0.0f), mPhi1(0.0f)
		{

		}
		virtual ~SimpleTorus()
		{

		}

	public:
		virtual void *Clone() { return (SimpleTorus *)(new SimpleTorus(*this)); }

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
		{
// 			float x0 = inRay.O().X();
// 			float y0 = inRay.O().Y();
// 			float z0 = inRay.O().Z();
// 			float dx = inRay.D().X();
// 			float dy = inRay.D().Y();
// 			float dz = inRay.D().Z();
// 
// 			//
// 			float sum_d_sqrd = dx * dx + dy * dy + dz * dz;
// 			float e = x0 * x0 + y0 * y0 + z0 * z0 - mSweptRadius * mSweptRadius - mTubeRadius * mTubeRadius;
// 			float f = x0 * dx + y0 * dy + z0 * dz;
// 			float four_a_sqrd = 4.0f * mSweptRadius * mSweptRadius;
// 
// 			// get the coefficients
// 			float A = sum_d_sqrd * sum_d_sqrd;
// 			float B = 4.0f * sum_d_sqrd * f;
// 			float C = 2.0f * sum_d_sqrd * e + 4.0f * f * f + four_a_sqrd * dy * dy;
// 			float D = 4.0f * f * e + 2.0f * four_a_sqrd * y0 * dy;
// 			float E = e * e - four_a_sqrd * (mTubeRadius * mTubeRadius - dy * dy);

			Vec3f OC = inRay.O() - mCenter;

			float x1 = OC.X(); float y1 = OC.Y(); float z1 = OC.Z();
			float d1 = inRay.D().X(); float d2 = inRay.D().Y(); float d3 = inRay.D().Z();

//			float sum_d_sqrd = d1 * d1 + d2 * d2 + d3 * d3;
//			float e = x1 * x1 + y1 * y1 + z1 * z1 - mSweptRadius * mSweptRadius - mTubeRadius * mTubeRadius;
//			float f = x1 * d1 + y1 * d2 + z1 * d3;
//			float four_a_sqrd = 4.0 * mSweptRadius * mSweptRadius;

			//
			float coefficients[5] = { 0.0f };
			float roots[4] = { 0.0f };

            float A = Dot(OC, OC);
            float B = 2.0f*Dot(OC, inRay.D());
            float C = Dot(inRay.D(), inRay.D());
            float Rr_square_p = mSweptRadius*mSweptRadius+mTubeRadius*mTubeRadius;
            float Rr_square_s_square = (mSweptRadius*mSweptRadius-mTubeRadius*mTubeRadius)*(mSweptRadius*mSweptRadius-mTubeRadius*mTubeRadius);
            float R_square = mSweptRadius*mSweptRadius;
            coefficients[4] = C*C;
            coefficients[3] = 2*B*C;
            coefficients[2] = B*B+2*A*C-2*Rr_square_p*C+4.0f*R_square*inRay.D().Z()*inRay.D().Z();
            coefficients[1] = 2.0f*A*B-2.0f*Rr_square_p*B+8.0f*R_square*OC.Z()*inRay.D().Z();
            coefficients[0] = A*A-2.0f*Rr_square_p*A+4.0f*R_square*OC.Z()*OC.Z()+Rr_square_s_square;

//          coefficients[4] = sum_d_sqrd * sum_d_sqrd;
//			coefficients[3] = 4.0 * sum_d_sqrd * f;
//			coefficients[2] = 2.0 * sum_d_sqrd * e + 4.0 * f * f + four_a_sqrd * d2 * d2;
//			coefficients[1] = 4.0 * f * e + 2.0 * four_a_sqrd * y1 * d2;
//			coefficients[0] = e * e - four_a_sqrd * (mTubeRadius * mTubeRadius - y1 * y1); 	// constant term

			//
// 			int num_roots = 0;
// 			float roots[4] = { 0.0f };
// 			RTMath::SolveQuaraticEquation(A, B, C, D, E, num_roots, roots);
			int num_real_roots = RTMath::SolveQuartic(coefficients, roots);
			
			bool is_hit = false;
			if (num_real_roots == 0)
			{
		//		g_Console.Write("num_roots is zero!\n");
				return is_hit;
			}
			else
			{
				// sort the roots
				float temp = FLT_MAX;
				for (int i = 0; i < num_real_roots; ++i)
				{
				    for (int j = i + 1; j < num_real_roots; ++j)
                    {
				        if (roots[i] > roots[j])
                        {
				            temp = roots[i];
				            roots[i] = roots[j];
				            roots[j] = temp;
                        }
                    }
				}

                for (int i = 0; i < num_real_roots; ++i)
                {
                    temp = roots[i];
                    if ((temp > tmin + KEpsilon()) && (temp < tmax))
                    {
                        bool test = true;
                        if (mPartTorus)
                        {
                            Vec3f hitPt = inRay.O() + temp * inRay.D();
                            hitPt = hitPt - mCenter;
                            bool in_theta_range = this->check_in_theta_range(hitPt);
                            bool in_phi_range = this->check_in_phi_range(hitPt);
                            if (!in_theta_range || !in_phi_range)
                            {
                                test = false;
                            }
                        }

                        if (test)
                        {
                            tmax = temp;

                            is_hit = true;

                            rec.hit = true;
                            rec.t = temp;
                            rec.pt = inRay.O() + temp * inRay.D();
                            rec.n = GetNormal(rec);
                            rec.albedo = mColor;
                            rec.pMaterial = nullptr;

                            break;
                        }
                    }
                }
			}

			return is_hit;
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const
		{
            Vec3f OC = inRay.O() - mCenter;

            float x1 = OC.X(); float y1 = OC.Y(); float z1 = OC.Z();
            float d1 = inRay.D().X(); float d2 = inRay.D().Y(); float d3 = inRay.D().Z();

//			float sum_d_sqrd = d1 * d1 + d2 * d2 + d3 * d3;
//			float e = x1 * x1 + y1 * y1 + z1 * z1 - mSweptRadius * mSweptRadius - mTubeRadius * mTubeRadius;
//			float f = x1 * d1 + y1 * d2 + z1 * d3;
//			float four_a_sqrd = 4.0 * mSweptRadius * mSweptRadius;

            //
            float coefficients[5] = { 0.0f };
            float roots[4] = { 0.0f };

            float A = Dot(OC, OC);
            float B = 2.0f*Dot(OC, inRay.D());
            float C = Dot(inRay.D(), inRay.D());
            float Rr_square_p = mSweptRadius*mSweptRadius+mTubeRadius*mTubeRadius;
            float Rr_square_s_square = (mSweptRadius*mSweptRadius-mTubeRadius*mTubeRadius)*(mSweptRadius*mSweptRadius-mTubeRadius*mTubeRadius);
            float R_square = mSweptRadius*mSweptRadius;
            coefficients[4] = C*C;
            coefficients[3] = 2*B*C;
            coefficients[2] = B*B+2*A*C-2*Rr_square_p*C+4.0f*R_square*inRay.D().Z()*inRay.D().Z();
            coefficients[1] = 2.0f*A*B-2.0f*Rr_square_p*B+8.0f*R_square*OC.Z()*inRay.D().Z();
            coefficients[0] = A*A-2.0f*Rr_square_p*A+4.0f*R_square*OC.Z()*OC.Z()+Rr_square_s_square;

//          coefficients[4] = sum_d_sqrd * sum_d_sqrd;
//			coefficients[3] = 4.0 * sum_d_sqrd * f;
//			coefficients[2] = 2.0 * sum_d_sqrd * e + 4.0 * f * f + four_a_sqrd * d2 * d2;
//			coefficients[1] = 4.0 * f * e + 2.0 * four_a_sqrd * y1 * d2;
//			coefficients[0] = e * e - four_a_sqrd * (mTubeRadius * mTubeRadius - y1 * y1); 	// constant term

            //
// 			int num_roots = 0;
// 			float roots[4] = { 0.0f };
// 			RTMath::SolveQuaraticEquation(A, B, C, D, E, num_roots, roots);
            int num_real_roots = RTMath::SolveQuartic(coefficients, roots);

            bool is_hit = false;
            if (num_real_roots == 0)
            {
                //		g_Console.Write("num_roots is zero!\n");
                return is_hit;
            }
            else
            {
                // sort the roots
                float temp = FLT_MAX;
                for (int i = 0; i < num_real_roots; ++i)
                {
                    for (int j = i + 1; j < num_real_roots; ++j)
                    {
                        if (roots[i] > roots[j])
                        {
                            temp = roots[i];
                            roots[i] = roots[j];
                            roots[j] = temp;
                        }
                    }
                }

                for (int i = 0; i < num_real_roots; ++i)
                {
                    temp = roots[i];
                    if (temp > KEpsilon())
                    {
                        bool test = true;
                        if (mPartTorus)
                        {
                            Vec3f hitPt = inRay.O() + temp * inRay.D();
                            hitPt = hitPt - mCenter;
                            bool in_theta_range = this->check_in_theta_range(hitPt);
                            bool in_phi_range = this->check_in_phi_range(hitPt);
                            if (!in_theta_range || !in_phi_range)
                            {
                                test = false;
                            }
                        }

                        if (test)
                        {
                            is_hit = true;
                            tvalue = temp;
                            break;
                        }
                    }
                }
            }

            return is_hit;
		}

	public:
		virtual float Area() const { return 0.0f; }

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding)
		{
			Vec3f vmin(-mSweptRadius-mTubeRadius, -mSweptRadius-mTubeRadius, -mSweptRadius-mTubeRadius);
			Vec3f vmax(mSweptRadius+mTubeRadius, mSweptRadius+mTubeRadius, mSweptRadius+mTubeRadius);
			bounding = AABB(vmin, vmax);

			return true;
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const
		{
//			Vec3f norm;
//			float x = rec.pt.X();
//			float y = rec.pt.Y();
//			float z = rec.pt.Z();
//
//			float sx = x*x;
//			float sy = y*y;
//			float sz = z*z;
//			float souter = mSweptRadius*mSweptRadius;
//			float sinner = mTubeRadius*mTubeRadius;
//
//			float nx = 4.0f*x*(sx+sy+sz-(souter+sinner));
//			float ny = 4.0f*y*((sx+sy+sz)-(souter+sinner)+2.0f*souter);
//			float nz = 4.0f*z*(sx+sy+sz-(souter+sinner));
//
//			norm.Set(nx, ny, nz);
//			norm.MakeUnit();

            Vec3f hitPT = rec.pt - mCenter;
            float x = hitPT.X();
            float y = hitPT.Y();
            float z = hitPT.Z();
            float nx = 4.0f*x*x*x+4*x*(y*y+z*z-(mSweptRadius*mSweptRadius+mTubeRadius*mTubeRadius));
            float ny = 4.0f*y*y*y+4.0f*y*(x*x+z*z-(mSweptRadius*mSweptRadius+mTubeRadius*mTubeRadius));
            float nz = 4.0f*z*z*z+4.0f*z*(x*x+y*y+mSweptRadius*mSweptRadius-mTubeRadius*mTubeRadius);
            Vec3f norm;
            norm.Set(nx, ny, nz);
            norm.MakeUnit();

// 			float param_squared = mSweptRadius * mSweptRadius + mTubeRadius * mTubeRadius;
// 
// 			float x = rec.pt.X();
// 			float y = rec.pt.Y();
// 			float z = rec.pt.Z();
// 			float sum_squared = x * x + y * y + z * z;
// 
// 			norm[0] = 4.0 * x * (sum_squared - param_squared);
// 			norm[1] = 4.0 * y * (sum_squared - param_squared + 2.0 * mSweptRadius * mSweptRadius);
// 			norm[2] = 4.0 * z * (sum_squared - param_squared);
// 			norm.MakeUnit();

			return norm;
		}

		virtual Vec3f RandomSamplePoint() const { return Vec3f(0.0f, 0.0f, 0.0f); }

		virtual void Update(float t) { }

		virtual bool IsCompound() const { return false; }

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
		inline void SetSweptRadius(float outerRadius) { mSweptRadius = outerRadius; }
		inline void SetTubeRadius(float innerRadius) { mTubeRadius = innerRadius; }
		inline void SetColor(float r, float g, float b) { mColor.Set(r, g, b); }

        inline void SetPartParams(bool partTours, float theta0, float theta1, float phi0, float phi1)
        {
		    mPartTorus = partTours;

		    mTheta0 = ANG2RAD(theta0);
		    mTheta1 = ANG2RAD(theta1);

		    mPhi0 = ANG2RAD(phi0);
		    mPhi1 = ANG2RAD(phi1);
		}

	public:
		inline static float KEpsilon() { return 0.001f; }

	private:
	    inline bool check_in_theta_range(Vec3f const& hitPt) const
	    {
		//    Vec3f z_axis(0.0f, 0.0f, 1.0f);
            Vec3f x_axis(1.0f, 0.0f, 0.0f);
		    float cos_theta = Dot(hitPt, x_axis) / (hitPt.Length() * x_axis.Length());
		    float theta = std::acos(cos_theta);
		    if (hitPt.Y() < mCenter.Y())
		    //if (hitPt.X() < mCenter.X())
            {
		        theta = 2.0f * PI_CONST - theta;
            }
		    if ((theta > mTheta0) && (theta < mTheta1))
            {
		        return true;
            }

		    return false;
		}

		inline bool check_in_phi_range(Vec3f const& hitPt) const
        {
		    float phi = std::acos(
                    (hitPt.Length() * hitPt.Length() - mSweptRadius * mSweptRadius - mTubeRadius * mTubeRadius)
                    / (2.0f * mSweptRadius * mTubeRadius));
		    if (hitPt.Z() < mCenter.Z())
            {
		        phi = 2.0f * PI_CONST - phi;
            }
		    if ((phi > mPhi0) && (phi < mPhi1))
            {
		        return true;
            }

		    return false;
        }

	private:
		Vec3f mCenter;
		float mSweptRadius;
		float mTubeRadius;
		Color3f mColor;

		// two theta and phi angle to define part torus
		// both -1 means the full torus
		// both are in radius.
		// theta is used to chop off torus
		// phi is used to open up torus
		bool mPartTorus;
		float mTheta0;
		float mTheta1;
		float mPhi0;
		float mPhi1;
	};

	//
	class SimpleEllipsoid : public GeometricObject
	{
	public:
		SimpleEllipsoid()
			: mCenter(0.0f, 0.0f, 0.0f), mInteceptA(30.0f), mInteceptB(20.0f), mInteceptC(10.0f), mColor(1.0f, 0.0f, 0.0f)
		{

		}

		SimpleEllipsoid(Vec3f const& center, float A, float B, float C, Color3f const& color = Color3f(1.0f, 0.0f, 0.0f))
			: mCenter(center), mInteceptA(A), mInteceptB(B), mInteceptC(C), mColor(color)
		{

		}

		virtual ~SimpleEllipsoid()
		{

		}

	public:
		virtual void *Clone() { return (SimpleEllipsoid *)(new SimpleEllipsoid(*this)); }

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
		{
			float x0 = inRay.O().X();
			float y0 = inRay.O().Y();
			float z0 = inRay.O().Z();
			float dx = inRay.D().X();
			float dy = inRay.D().Y();
			float dz = inRay.D().Z();

			float aabb = mInteceptA * mInteceptA * mInteceptB * mInteceptB;
			float aacc = mInteceptA * mInteceptA * mInteceptC * mInteceptC;
			float bbcc = mInteceptB * mInteceptB * mInteceptC * mInteceptC;
			float aabbcc = mInteceptA * mInteceptA * mInteceptB * mInteceptB * mInteceptC * mInteceptC;

			float A = bbcc * dx * dx + aacc * dy * dy + aabb * dz * dz;
			float B = 2.0f * bbcc * dx * (x0 - mCenter.X()) + 2.0f * aacc * dy * (y0 - mCenter.Y()) + 2.0f * aabb * dz * (z0 - mCenter.Z());
			float C = bbcc * (x0 - mCenter.X()) * (x0 - mCenter.X()) + aacc * (y0 - mCenter.Y()) * (y0 - mCenter.Y()) + aabb * (z0 - mCenter.Z()) * (z0 - mCenter.Z()) - aabbcc;
			float D = B*B - 4.0f*A*C;

			if (D < 0.0f)
			{
				return false;
			}

			bool is_hit = false;
			float root0 = (-B - std::sqrt(D)) / (2.0f * A);
			if ((root0 > tmin + KEpsilon()) && (root0 < tmax))
			{
				tmax = root0;

				is_hit = true;

				rec.hit = true;
				rec.t = root0;
				rec.pt = inRay.O() + root0 * inRay.D();
				rec.n = GetNormal(rec);
				rec.albedo = mColor;
				rec.pMaterial = nullptr;
			}

			if (!is_hit)
			{
				float root1 = (-B + std::sqrt(D)) / (2.0f * A);
				if ((root1 > tmin + KEpsilon()) && (root1 < tmax))
				{
					tmax = root1;

					is_hit = true;

					rec.hit = true;
					rec.t = root1;
					rec.pt = inRay.O() + root1 * inRay.D();
					rec.n = GetNormal(rec);
					rec.albedo = mColor;
					rec.pMaterial = nullptr;
				}
			}
 
			return is_hit;
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const
		{
			float x0 = inRay.O().X();
			float y0 = inRay.O().Y();
			float z0 = inRay.O().Z();
			float dx = inRay.D().X();
			float dy = inRay.D().Y();
			float dz = inRay.D().Z();

			float aabb = mInteceptA * mInteceptA * mInteceptB * mInteceptB;
			float aacc = mInteceptA * mInteceptA * mInteceptC * mInteceptC;
			float bbcc = mInteceptB * mInteceptB * mInteceptC * mInteceptC;
			float aabbcc = mInteceptA * mInteceptA * mInteceptB * mInteceptB * mInteceptC * mInteceptC;

			float A = bbcc * dx * dx + aacc * dy * dy + aabb * dz * dz;
			float B = 2.0f * bbcc * dx * (x0 - mCenter.X()) + 2.0f * aacc * dy * (y0 - mCenter.Y()) + 2.0f * aabb * dz * (z0 - mCenter.Z());
			float C = bbcc * (x0 - mCenter.X()) * (x0 - mCenter.X()) + aacc * (y0 - mCenter.Y()) * (y0 - mCenter.Y()) + aabb * (z0 - mCenter.Z()) * (z0 - mCenter.Z()) - aabbcc;
			float D = B*B - 4.0f*A*C;

			if (D < 0.0f)
			{
				return false;
			}

			bool is_hit = false;
			float root0 = (-B - std::sqrt(D)) / (2.0f * A);
			if (root0 > KEpsilon())
			{
				is_hit = true;
				tvalue = root0;
			}

			if (!is_hit)
			{
				float root1 = (-B + std::sqrt(D)) / (2.0f * A);
				if (root1 > KEpsilon())
				{
					is_hit = true;
					tvalue = root1;
				}
			}

			return is_hit;
		}

	public:
		virtual float Area() const { return 0.0f; }

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding)
		{
			Vec3f vec_A(mInteceptA, 0.0f, 0.0f);
			Vec3f vec_B(0.0f, mInteceptB, 0.0f);
			Vec3f vec_C(0.0f, 0.0f, mInteceptC);

			Vec3f vmin = mCenter - vec_A - vec_B - vec_C;
			Vec3f vmax = mCenter + vec_A + vec_B + vec_C;
			bounding = AABB(vmin, vmax);

			return true;
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const
        {
			Vec3f norm;

			float nx = 2.0f * mInteceptB * mInteceptB * mInteceptC * mInteceptC * (rec.pt.X() - mCenter.X());
			float ny = 2.0f * mInteceptA * mInteceptA * mInteceptC * mInteceptC * (rec.pt.Y() - mCenter.Y());
			float nz = 2.0f * mInteceptA * mInteceptA * mInteceptB * mInteceptB * (rec.pt.Z() - mCenter.Z());

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
		inline void SetPosition(const Vec3f& center) { mCenter = center; }
		inline void SetInteceptLen(float _A, float _B, float _C)
		{
			mInteceptA = _A;
			mInteceptB = _B;
			mInteceptC = _C;
		}
		inline void SetColor(const Color3f& color)
		{
			mColor = color;
		}

	public:
		inline static float KEpsilon() { return 0.001f; }

	private:
		Vec3f mCenter;
		float mInteceptA;
		float mInteceptB;
		float mInteceptC;
		Color3f mColor;

	};

	//
	class SimpleQuadratic : public GeometricObject
	{
	public:
		SimpleQuadratic()
			: mCenter(0.0f, 0.0f, 0.0f), mInteceptA(30.0f), mInteceptB(20.0f), mInteceptC(10.0f), mSign0(-1), mSign1(1), mHalfHeight(30.0f), mColor(1.0f, 0.0f, 0.0f)
		{

		}

		SimpleQuadratic(Vec3f const& center, float A, float B, float C, int sign0, int sign1, float halfHeight, const Color3f& color = Color3f(1.0f, 0.0f, 0.0f))
			: mCenter(center), mInteceptA(A), mInteceptB(B), mInteceptC(C), mSign0(sign0), mSign1(sign1), mHalfHeight(halfHeight), mColor(color)
		{

		}

		virtual ~SimpleQuadratic()
		{

		}

	public:
		virtual void *Clone() { return (SimpleQuadratic *)(new SimpleQuadratic(*this)); }

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
		{
			float x0 = inRay.O().X();
			float y0 = inRay.O().Y();
			float z0 = inRay.O().Z();
			float dx = inRay.D().X();
			float dy = inRay.D().Y();
			float dz = inRay.D().Z();

			float aabb = mInteceptA * mInteceptA * mInteceptB * mInteceptB;
			float aacc = mInteceptA * mInteceptA * mInteceptC * mInteceptC;
			float bbcc = mInteceptB * mInteceptB * mInteceptC * mInteceptC;
			float aabbcc = mInteceptA * mInteceptA * mInteceptB * mInteceptB * mInteceptC * mInteceptC;

			float A = bbcc * dx * dx + mSign0 * aacc * dy * dy + aabb * dz * dz;
			float B = 2.0f * bbcc * dx * (x0 - mCenter.X()) + 2.0f * mSign0 * aacc * dy * (y0 - mCenter.Y()) + 2.0f * aabb * dz * (z0 - mCenter.Z());
			float C = bbcc * (x0 - mCenter.X()) * (x0 - mCenter.X()) + mSign0 * aacc * (y0 - mCenter.Y()) * (y0 - mCenter.Y()) + aabb * (z0 - mCenter.Z()) * (z0 - mCenter.Z()) - mSign1 * aabbcc;
			float D = B*B - 4.0f*A*C;

			if (D < 0.0f)
			{
				return false;
			}

			float root0 = (-B - std::sqrt(B * B - 4.0f * A * C)) / (2.0f * A);
			float root1 = (-B + std::sqrt(B * B - 4.0f * A * C)) / (2.0f * A);

			float roots[2];
			if (root0 < root1)
			{
				roots[0] = root0;
				roots[1] = root1;
			}
			else
			{
				roots[0] = root1;
				roots[1] = root0;
			}

			bool is_hit = false;
			if ((roots[0] > tmin + KEpsilon()) && (roots[0] < tmax))
			{
				Vec3f pt = inRay.O() + roots[0] * inRay.D();
				if ((pt.Y() - mCenter.Y() > -mHalfHeight) && (pt.Y() - mCenter.Y() < mHalfHeight))
				{
					tmax = roots[0];

					is_hit = true;

					rec.hit = true;
					rec.t = roots[0];
					rec.pt = inRay.O() + roots[0] * inRay.D();
					rec.n = GetNormal(rec);
					if (Dot(rec.n, inRay.D()) > 0.0f)
					{
						rec.n = -rec.n;
					}
					rec.albedo = mColor;
					rec.pMaterial = nullptr;
				}
			}

			if (!is_hit)
			{
				if ((roots[1] > tmin + KEpsilon()) && (roots[1] < tmax))
				{
					Vec3f pt = inRay.O() + roots[1] * inRay.D();
					if ((pt.Y() - mCenter.Y() > -mHalfHeight) && (pt.Y() - mCenter.Y() < mHalfHeight))
					{
						tmax = roots[1];

						is_hit = true;

						rec.hit = true;
						rec.t = roots[1];
						rec.pt = inRay.O() + roots[1] * inRay.D();
						rec.n = GetNormal(rec);
						if (Dot(rec.n, inRay.D()) > 0.0f)
						{
							rec.n = -rec.n;
						}
						rec.albedo = mColor;
						rec.pMaterial = nullptr;
					}
				}
			}

			return is_hit;
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const
		{
			float x0 = inRay.O().X();
			float y0 = inRay.O().Y();
			float z0 = inRay.O().Z();
			float dx = inRay.D().X();
			float dy = inRay.D().Y();
			float dz = inRay.D().Z();

			float aabb = mInteceptA * mInteceptA * mInteceptB * mInteceptB;
			float aacc = mInteceptA * mInteceptA * mInteceptC * mInteceptC;
			float bbcc = mInteceptB * mInteceptB * mInteceptC * mInteceptC;
			float aabbcc = mInteceptA * mInteceptA * mInteceptB * mInteceptB * mInteceptC * mInteceptC;

			float A = bbcc * dx * dx + mSign0 * aacc * dy * dy + aabb * dz * dz;
			float B = 2.0f * bbcc * dx * (x0 - mCenter.X()) + 2.0f * mSign0 * aacc * dy * (y0 - mCenter.Y()) + 2.0f * aabb * dz * (z0 - mCenter.Z());
			float C = bbcc * (x0 - mCenter.X()) * (x0 - mCenter.X()) + mSign0 * aacc * (y0 - mCenter.Y()) * (y0 - mCenter.Y()) + aabb * (z0 - mCenter.Z()) * (z0 - mCenter.Z()) - mSign1 * aabbcc;
			float D = B*B - 4.0f*A*C;

			if (D < 0.0f)
			{
				return false;
			}

			float root0 = (-B - std::sqrt(B * B - 4.0f * A * C)) / (2.0f * A);
			float root1 = (-B + std::sqrt(B * B - 4.0f * A * C)) / (2.0f * A);

			float roots[2];
			if (root0 < root1)
			{
				roots[0] = root0;
				roots[1] = root1;
			}
			else
			{
				roots[0] = root1;
				roots[1] = root0;
			}

			bool is_hit = false;
			if (roots[0] > KEpsilon())
			{
				Vec3f pt = inRay.O() + roots[0] * inRay.D();
				if ((pt.Y() - mCenter.Y() > -mHalfHeight) && (pt.Y() - mCenter.Y() < mHalfHeight))
				{
					is_hit = true;
					tvalue = roots[0];
				}
			}

			if (!is_hit)
			{
				if (roots[1] > KEpsilon())
				{
					Vec3f pt = inRay.O() + roots[1] * inRay.D();
					if ((pt.Y() - mCenter.Y() > -mHalfHeight) && (pt.Y() - mCenter.Y() < mHalfHeight))
					{
						is_hit = true;
						tvalue = roots[1];
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

			float nx = 2.0f * mInteceptB * mInteceptB * mInteceptC * mInteceptC * (rec.pt.X() - mCenter.X());
			float ny = 2.0f * mSign0 * mInteceptA * mInteceptA * mInteceptC * mInteceptC * (rec.pt.Y() - mCenter.Y());
			float nz = 2.0f * mInteceptA * mInteceptA * mInteceptB * mInteceptB * (rec.pt.Z() - mCenter.Z());
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
		inline void SetIntecept(float _A, float _B, float _C) { mInteceptA = _A; mInteceptB = _B; mInteceptC = _C; }
		inline void SetSign(int s0, int s1) { mSign0 = s0; mSign1 = s1; }
		inline void SetClipRange(float halfHeight) { mHalfHeight = halfHeight; }

	public:
		inline static float KEpsilon() { return 0.001f; }

	private:
		Vec3f mCenter;
		float mInteceptA;
		float mInteceptB;
		float mInteceptC;
		int mSign0;		// -1 1, -1 -1, -1 0, 0 1
		int mSign1;
		float mHalfHeight;
		Color3f mColor;

	};

	//
	class SimpleParaboloid : public GeometricObject
	{
	public:
		SimpleParaboloid()
			: mCenter(0.0f, 0.0f, 0.0f), mP(15.0f), mQ(5.0f), mHalfHeight(10.0f), mColor(1.0f, 0.0f, 0.0f)
		{

		}

		SimpleParaboloid(Vec3f const& center, float p, float q, float halfHeight, const Color3f& color = Color3f(1.0f, 0.0f, 0.0f))
			: mCenter(center), mP(p), mQ(q), mHalfHeight(halfHeight), mColor(color)
		{

		}

		virtual ~SimpleParaboloid()
		{

		}

	public:
		virtual void *Clone() { return (SimpleParaboloid *)(new SimpleParaboloid(*this)); }

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
		{
			float x0 = inRay.O().X();
			float y0 = inRay.O().Y();
			float z0 = inRay.O().Z();
			float dx = inRay.D().X();
			float dy = inRay.D().Y();
			float dz = inRay.D().Z();

			float A = (mQ * dx * dx + mP * dz *dz);
			float B = 2.0f * mQ * dx * (x0 - mCenter.X()) + 2.0f * mP * dz * (z0 - mCenter.Z()) - 2.0f * mP * mQ * dy;
			float C = mQ * (x0 - mCenter.X()) * (x0 - mCenter.X()) + mP * (z0 - mCenter.Z()) * (z0 - mCenter.Z()) - 2.0f * mP * mQ * (y0 - mCenter.Y());
			float D = B * B - 4.0f * A * C;

			if (D < 0.0f)
			{
				return false;
			}

			float root0 = (-B - std::sqrt(B * B - 4.0f * A * C)) / (2.0f * A);
			float root1 = (-B + std::sqrt(B * B - 4.0f * A * C)) / (2.0f * A);

			float roots[2];
			if (root0 < root1)
			{
				roots[0] = root0;
				roots[1] = root1;
			}
			else
			{
				roots[0] = root1;
				roots[1] = root0;
			}

			bool is_hit = false;
			if ((roots[0] > tmin + KEpsilon()) && (roots[0] < tmax))
			{
				Vec3f pt = inRay.O() + roots[0] * inRay.D();
				if ((pt.Y() - mCenter.Y() > -mHalfHeight) && (pt.Y() - mCenter.Y() < mHalfHeight))
				{
					tmax = roots[0];

					is_hit = true;

					rec.hit = true;
					rec.t = roots[0];
					rec.pt = inRay.O() + roots[0] * inRay.D();
					rec.n = GetNormal(rec);
					if (Dot(rec.n, inRay.D()) > 0.0f)
					{
						rec.n = -rec.n;
					}
					rec.albedo = mColor;
					rec.pMaterial = nullptr;
				}
			}

			if (!is_hit)
			{
				if ((roots[1] > tmin + KEpsilon()) && (roots[1] < tmax))
				{
					Vec3f pt = inRay.O() + roots[1] * inRay.D();
					if ((pt.Y() - mCenter.Y() > -mHalfHeight) && (pt.Y() - mCenter.Y() < mHalfHeight))
					{
						tmax = roots[1];

						is_hit = true;

						rec.hit = true;
						rec.t = roots[1];
						rec.pt = inRay.O() + roots[1] * inRay.D();
						rec.n = GetNormal(rec);
						if (Dot(rec.n, inRay.D()) > 0.0f)
						{
							rec.n = -rec.n;
						}
						rec.albedo = mColor;
						rec.pMaterial = nullptr;
					}
				}
			}

			return is_hit;
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const
		{
			float x0 = inRay.O().X();
			float y0 = inRay.O().Y();
			float z0 = inRay.O().Z();
			float dx = inRay.D().X();
			float dy = inRay.D().Y();
			float dz = inRay.D().Z();

			float A = (mQ * dx * dx + mP * dz *dz);
			float B = 2.0f * mQ * dx * (x0 - mCenter.X()) + 2.0f * mP * dz * (z0 - mCenter.Z()) - 2.0f * mP * mQ * dy;
			float C = mQ * (x0 - mCenter.X()) * (x0 - mCenter.X()) + mP * (z0 - mCenter.Z()) * (z0 - mCenter.Z()) - 2.0f * mP * mQ * (y0 - mCenter.Y());
			float D = B * B - 4.0f * A * C;

			if (D < 0.0f)
			{
				return false;
			}

			float root0 = (-B - std::sqrt(B * B - 4.0f * A * C)) / (2.0f * A);
			float root1 = (-B + std::sqrt(B * B - 4.0f * A * C)) / (2.0f * A);

			float roots[2];
			if (root0 < root1)
			{
				roots[0] = root0;
				roots[1] = root1;
			}
			else
			{
				roots[0] = root1;
				roots[1] = root0;
			}

			bool is_hit = false;
			if (roots[0] > KEpsilon())
			{
				Vec3f pt = inRay.O() + roots[0] * inRay.D();
				if ((pt.Y() - mCenter.Y() > -mHalfHeight) && (pt.Y() - mCenter.Y() < mHalfHeight))
				{
					is_hit = true;
					tvalue = roots[0];
				}
			}

			if (!is_hit)
			{
				if (roots[1] > KEpsilon())
				{
					Vec3f pt = inRay.O() + roots[1] * inRay.D();
					if ((pt.Y() - mCenter.Y() > -mHalfHeight) && (pt.Y() - mCenter.Y() < mHalfHeight))
					{
						is_hit = true;
						tvalue = roots[1];
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

			float nx = 2.0f * mQ * (rec.pt.X() - mCenter.X());
			float ny = -2.0f * mP * mQ;
			float nz = 2.0f * mP * (rec.pt.Z() - mCenter.Z());
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
		inline void SetParam_PQ(float p, float q) { mP = p; mQ = q; }
		inline void SetClipRange(float halfHeight) { mHalfHeight = halfHeight; }
		inline void SetColor(Color3f const& color) { mCenter = color; }

	public:
		inline static float KEpsilon() { return 0.001f; }

	private:
		Vec3f mCenter;
		float mP;
		float mQ;
		float mHalfHeight;
		Color3f mColor;

	};

	//
	class SimpleHyperboloid : public GeometricObject
	{
	public:
		SimpleHyperboloid()
			: mCenter(0.0f, 0.0f, 0.0f), mP(15.0f), mQ(5.0f), mHalfWidth(10.0f), mHalfHeight(10.0f), mColor(1.0f, 0.0f, 0.0f)
		{

		}

		SimpleHyperboloid(Vec3f const& center, float p, float q, float halfWidth, float halfHeight, const Color3f color = Color3f(1.0f, 0.0f, 0.0f))
			: mCenter(center), mP(p), mQ(q), mHalfWidth(halfWidth), mHalfHeight(halfHeight), mColor(color)
		{

		}

		virtual ~SimpleHyperboloid()
		{

		}

	public:
		virtual void *Clone() { return (SimpleHyperboloid *)(new SimpleHyperboloid(*this)); }

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
		{
			float x0 = inRay.O().X();
			float y0 = inRay.O().Y();
			float z0 = inRay.O().Z();
			float dx = inRay.D().X();
			float dy = inRay.D().Y();
			float dz = inRay.D().Z();

			float A = ((-mQ) * dx * dx + mP * dz *dz);
			float B = 2.0f * (-mQ) * dx * (x0 - mCenter.X()) + 2.0f * mP * dz * (z0 - mCenter.Z()) - 2.0f * mP * (-mQ) * dy;
			float C = (-mQ) * (x0 - mCenter.X()) * (x0 - mCenter.X()) + mP * (z0 - mCenter.Z()) * (z0 - mCenter.Z()) - 2.0f * mP * (-mQ) * (y0 - mCenter.Y());
			float D = B * B - 4.0f * A * C;

			if (D < 0.0f)
			{
				return false;
			}

			float root0 = (-B - std::sqrt(B * B - 4.0f * A * C)) / (2.0f * A);
			float root1 = (-B + std::sqrt(B * B - 4.0f * A * C)) / (2.0f * A);

			float roots[2];
			if (root0 < root1)
			{
				roots[0] = root0;
				roots[1] = root1;
			}
			else
			{
				roots[0] = root1;
				roots[1] = root0;
			}

			bool is_hit = false;
			if ((roots[0] > tmin + KEpsilon()) && (roots[0] < tmax))
			{
				Vec3f pt = inRay.O() + roots[0] * inRay.D();
				bool in_x_range = ((pt.X() - mCenter.X() > -mHalfWidth) && (pt.X() - mCenter.X() < mHalfWidth));
				bool in_y_range = ((pt.Y() - mCenter.Y() > -mHalfHeight) && (pt.Y() - mCenter.Y() < mHalfHeight));
				if (in_x_range && in_y_range)
				{
					tmax = roots[0];

					is_hit = true;

					rec.hit = true;
					rec.t = roots[0];
					rec.pt = inRay.O() + roots[0] * inRay.D();
					rec.n = GetNormal(rec);
					if (Dot(rec.n, inRay.D()) > 0.0f)
					{
						rec.n = -rec.n;
					}
					rec.albedo = mColor;
					rec.pMaterial = nullptr;
				}
			}

			if (!is_hit)
			{
				if ((roots[1] > tmin + KEpsilon()) && (roots[1] < tmax))
				{
					Vec3f pt = inRay.O() + roots[1] * inRay.D();
					bool in_x_range = ((pt.X() - mCenter.X() > -mHalfWidth) && (pt.X() - mCenter.X() < mHalfWidth));
					bool in_y_range = ((pt.Y() - mCenter.Y() > -mHalfHeight) && (pt.Y() - mCenter.Y() < mHalfHeight));
					if (in_x_range && in_y_range)
					{
						tmax = roots[1];

						is_hit = true;

						rec.hit = true;
						rec.t = roots[1];
						rec.pt = inRay.O() + roots[1] * inRay.D();
						rec.n = GetNormal(rec);
						if (Dot(rec.n, inRay.D()) > 0.0f)
						{
							rec.n = -rec.n;
						}
						rec.albedo = mColor;
						rec.pMaterial = nullptr;
					}
				}
			}

			return is_hit;
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const
		{
			float x0 = inRay.O().X();
			float y0 = inRay.O().Y();
			float z0 = inRay.O().Z();
			float dx = inRay.D().X();
			float dy = inRay.D().Y();
			float dz = inRay.D().Z();

			float A = ((-mQ) * dx * dx + mP * dz *dz);
			float B = 2.0f * (-mQ) * dx * (x0 - mCenter.X()) + 2.0f * mP * dz * (z0 - mCenter.Z()) - 2.0f * mP * (-mQ) * dy;
			float C = (-mQ) * (x0 - mCenter.X()) * (x0 - mCenter.X()) + mP * (z0 - mCenter.Z()) * (z0 - mCenter.Z()) - 2.0f * mP * (-mQ) * (y0 - mCenter.Y());
			float D = B * B - 4.0f * A * C;

			if (D < 0.0f)
			{
				return false;
			}

			float root0 = (-B - std::sqrt(B * B - 4.0f * A * C)) / (2.0f * A);
			float root1 = (-B + std::sqrt(B * B - 4.0f * A * C)) / (2.0f * A);

			float roots[2];
			if (root0 < root1)
			{
				roots[0] = root0;
				roots[1] = root1;
			}
			else
			{
				roots[0] = root1;
				roots[1] = root0;
			}

			bool is_hit = false;
			if (roots[0] > KEpsilon())
			{
				Vec3f pt = inRay.O() + roots[0] * inRay.D();
				bool in_x_range = ((pt.X() - mCenter.X() > -mHalfWidth) && (pt.X() - mCenter.X() < mHalfWidth));
				bool in_y_range = ((pt.Y() - mCenter.Y() > -mHalfHeight) && (pt.Y() - mCenter.Y() < mHalfHeight));
				if (in_x_range && in_y_range)
				{
					is_hit = true;
					tvalue = roots[0];
				}
			}

			if (!is_hit)
			{
				if (roots[1] > KEpsilon())
				{
					Vec3f pt = inRay.O() + roots[1] * inRay.D();
					bool in_x_range = ((pt.X() - mCenter.X() > -mHalfWidth) && (pt.X() - mCenter.X() < mHalfWidth));
					bool in_y_range = ((pt.Y() - mCenter.Y() > -mHalfHeight) && (pt.Y() - mCenter.Y() < mHalfHeight));
					if (in_x_range && in_y_range)
					{
						is_hit = true;
						tvalue = roots[1];
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

			float nx = 2.0f * (-mQ) * (rec.pt.X() - mCenter.X());
			float ny = -2.0f * mP * (-mQ);
			float nz = 2.0f * mP * (rec.pt.Z() - mCenter.Z());
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
		inline void SetParam_PQ(float p, float q) { mP = p; mQ = q; }
		inline void SetClipRange(float halfWidth, float halfHeight)
		{
			mHalfWidth = halfWidth;
			mHalfHeight = halfHeight;
		}
		inline void SetColor(Color3f const& color) { mColor = color; }

	public:
		inline static float KEpsilon() { return 0.001f; }

	private:
		Vec3f mCenter;
		float mP;
		float mQ;
		float mHalfWidth;
		float mHalfHeight;
		Color3f mColor;

	};

	//
	class CompoundObject : public GeometricObject
	{
	public:
		CompoundObject()
			: mnLastHitIdx(-1), mbAutoDelete(true)
		{

		}

		CompoundObject(CompoundObject const& other)
		{
			CopyObjects(other.mvecObjects);
		}

		CompoundObject& operator=(CompoundObject const& other)
		{
			if (this == &other) { return *this; }

			CopyObjects(other.mvecObjects);

			return *this;
		}

		virtual ~CompoundObject()
		{
			DeleteObjects();
		}

	public:
		virtual void *Clone() { return (CompoundObject *)(new CompoundObject(*this)); }

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
		{
			Vec3f n;
			Vec3f pt;
			float tmax_copy = FLT_MAX;
			float t = FLT_MAX;
			bool bHitAnything = false;
			Material *material = nullptr;
			Color3f albedo;

			int numObject = GetCount();
			mnLastHitIdx = 0;
			for (int i = 0; i < numObject; ++i)
			{
				if (At(i)->HitTest(inRay, tmin, t, rec) && (t < tmax_copy))
				{
					mnLastHitIdx = i;

					bHitAnything = true;
					tmax_copy = t;
					n = rec.n;
					pt = rec.pt;
					albedo = rec.albedo;
					material = rec.pMaterial;
				}
			}

			if (bHitAnything && (tmax_copy > tmin && tmax_copy < tmax))
			{
				tmax = tmax_copy;

				rec.hit = true;
				rec.t = tmax;
				rec.pt = pt;
				rec.n = n;
			//	rec.pMaterial = nullptr;
				rec.pMaterial = material;
				rec.albedo = albedo;
			}

			return bHitAnything;
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const
		{
			// if two objects are close to light including one of them represents the light,
			// it's better not to add them into compound objects, because
			// once the shadow ray hits an object, it concludes the test process,
			// actually the object been hit is just a light source.
			// here is a single loop, you need a double loop to solve this problem.
			// but also it's very time-cosuming.
			bool isHit = false;
			int numObject = GetCount();
			for (int i = 0; i < numObject; ++i)
			{
				if (At(i)->IntersectP(inRay, tvalue))
				{
					isHit = true;
					break;
				}
			}

			return isHit;
		}

	public:
		virtual float Area() const
		{
			return 0.0f;
		}

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding)
		{
			if (mvecObjects.size() < 1)
				return false;

			AABB temp;
			bool firstProb = mvecObjects[0]->GetBoundingBox(t0, t1, temp);
			if (!firstProb)
				return false;
			else
				bounding = temp;

			for (int i = 1; i < mvecObjects.size(); ++i)
			{
				if (mvecObjects[i]->GetBoundingBox(t0, t1, temp))
				{
					bounding = AABB::SurroundingBox(bounding, temp);
				}
				else
				{
					return false;
				}
			}

			return true;
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const
		{
            return WORLD_ORIGIN;
		}

		virtual Vec3f RandomSamplePoint() const
		{
			int index = static_cast<int>(Random::drand48() * this->GetCount());
			return mvecObjects[index]->RandomSamplePoint();
		}

		virtual void Update(float t)
		{

		}

		virtual bool IsCompound() const
		{
			return true;
		}

		virtual float PDFValue(Vec3f const& o, Vec3f const& v) const
		{
			unsigned long count = this->GetCount();
			float weight = 1.0f / (float)count;
			float sum = 0.0f;
			
			for (int i = 0; i < count; ++i)
			{
				sum += weight * (mvecObjects[i]->PDFValue(o, v));
			}

			return sum;
		}

		virtual Vec3f RandomSampleDirection(Vec3f const& v) const
		{
			int index = static_cast<int>(Random::drand48() * this->GetCount());
			return mvecObjects[index]->RandomSampleDirection(v);
		}

	public:
		inline void CopyObjects(vector<GeometricObject * > const& objects)
		{
			DeleteObjects();

			for (auto iter : objects)
			{
				if (mbAutoDelete) // deep copy
				{
					mvecObjects.push_back((GeometricObject *)iter->Clone());
				}
				else // shallow copy, only the pointer
				{
					mvecObjects.push_back((GeometricObject *)iter);
				}
			}

		}

		inline void DeleteObjects()
		{
			if (mbAutoDelete)
			{
				for (auto iter : mvecObjects)
				{
					delete iter;
				}
			}
			
			mvecObjects.clear();
		}

		inline unsigned long GetCount() const { return (unsigned long)mvecObjects.size(); }

		inline vector<GeometricObject * >& GetObjects() { return mvecObjects; }
		inline GeometricObject *At(unsigned long idx) { return mvecObjects[idx]; }
		inline const GeometricObject *At(unsigned long idx) const { return mvecObjects[idx]; }
		inline void AddObject(GeometricObject *obj) {
			if (obj != nullptr) {
				mvecObjects.push_back(obj);
			}
		}
		inline void SetObject(int idx, GeometricObject *obj) {
			mvecObjects[idx] = obj;
		}
		inline int LastHit() const { return mnLastHitIdx; }

		inline void EnableAutoDelete(bool enable) { mbAutoDelete = enable; }

	protected:
		vector<GeometricObject * >		mvecObjects;
		int		mnLastHitIdx;
		bool	mbAutoDelete;
	};
}
