#pragma once

#include "Vec3.h"
#include "Ray.h"

namespace LaplataRayTracer
{
	//---------------------------------------
	// AABB
	//---------------------------------------
	class AABB
	{
	public:
		float mX0;
		float mX1;
		float mY0;
		float mY1;
		float mZ0;
		float mZ1;

	public:
		AABB()
			: mX0(-1), mX1(1), mY0(-1), mY1(1), mZ0(-10), mZ1(1)
		{

		}

		AABB(const float x0, const float x1,
			const float y0, const float y1,
			const float z0, const float z1)
			: mX0(x0), mX1(x1), mY0(y0), mY1(y1), mZ0(z0), mZ1(z1)
		{

		}

		AABB(Vec3f const& p0, Vec3f const& p1)
			: mX0(p0.X()), mX1(p1.X()), mY0(p0.Y()), mY1(p1.Y()), mZ0(p0.Z()), mZ1(p1.Z())
		{

		}

		AABB(AABB const& other)
			: mX0(other.mX0), mX1(other.mX1), mY0(other.mY0), mY1(other.mY1), mZ0(other.mZ0), mZ1(other.mZ1)
		{

		}

		AABB& operator=(AABB const& other)
		{
			if (this == &other) { return *this; }

			this->mX0 = other.mX0;
			this->mX1 = other.mX1;
			this->mY0 = other.mY0;
			this->mY1 = other.mY1;
			this->mZ0 = other.mZ0;
			this->mZ1 = other.mZ1;

			return *this;
		}

		~AABB()
		{

		}

	public:
		inline bool HitTest(Ray const& inRay) const
		{
			float tx_min, ty_min, tz_min, tx_max, ty_max, tz_max;
			float t0;
			bool is_hit = this->HitTestImpl(inRay, tx_min, ty_min, tz_min, tx_max, ty_max, tz_max, t0);
			return is_hit;
		}

		inline bool IsInside(const Vec3f& v) const
		{
			return ((v.X() > mX0 && v.X() < mX1) && (v.Y() > mY0 && v.Y() < mY1) && (v.Z() > mZ0 && v.Z() < mZ1));
		}

	public:
		inline static AABB SurroundingBox(AABB box0, AABB box1)
		{
			Vec3f vSmall(std::min<float>(box0.mX0, box1.mX0), std::min<float>(box0.mY0, box1.mY0), std::min<float>(box0.mZ0, box1.mZ0));
			Vec3f vBig(std::max<float>(box0.mX1, box1.mX1), std::max<float>(box0.mY1, box1.mY1), std::max<float>(box0.mZ1, box1.mZ1));

			return AABB(vSmall, vBig);
		}

		inline bool HitTestImpl(Ray const& inRay, float& txmin, float& tymin, float& tzmin, float& txmax, float& tymax, float& tzmax, float& t0) const
		{
			//
			float ox = inRay.O().X();
			float oy = inRay.O().Y();
			float oz = inRay.O().Z();

			float dx = inRay.D().X();
			float dy = inRay.D().Y();
			float dz = inRay.D().Z();

		//	float tx_min, ty_min, tz_min;
		//	float tx_max, ty_max, tz_max;

			//
			float a = 1.0f / dx;
			if (a >= 0)
			{
				txmin = (mX0 - ox) * a;
				txmax = (mX1 - ox) * a;
			}
			else
			{
				txmin = (mX1 - ox) * a;
				txmax = (mX0 - ox) * a;
			}

			float b = 1.0f / dy;
			if (b >= 0)
			{
				tymin = (mY0 - oy) * b;
				tymax = (mY1 - oy) * b;
			}
			else
			{
				tymin = (mY1 - oy) * b;
				tymax = (mY0 - oy) * b;
			}

			float c = 1.0f / dz;
			if (c >= 0)
			{
				tzmin = (mZ0 - oz) * c;
				tzmax = (mZ1 - oz) * c;
			}
			else {
				tzmin = (mZ1 - oz) * c;
				tzmax = (mZ0 - oz) * c;
			}

			//
			float t1;

			if (txmin > tymin)
			{
				t0 = txmin;
			}
			else
			{
				t0 = tymin;
			}
			if (tzmin > t0)
			{
				t0 = tzmin;
			}

			if (txmax < tymax)
			{
				t1 = txmax;
			}
			else
			{
				t1 = tymax;
			}
			if (tzmax < t1)
			{
				t1 = tzmax;
			}

			return (t0 < t1 && t1 > KEpsilon());
		}

	public:
		inline static float KEpsilon() { return 0.001f; }

	};
}