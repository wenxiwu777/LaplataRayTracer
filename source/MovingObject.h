#pragma once

#include "GeometricObject.h"

namespace LaplataRayTracer {

	class MovingShpere : public SimpleSphere {
	public:
		//
		MovingShpere(Vec3f const& pos0, float fRadius, Vec3f const& pos1, float time0, float time1)
			: SimpleSphere(pos0, fRadius), mPos1(pos1), mTime0(time0), mTime1(time1)
		{

		}

		~MovingShpere() {

		}

	public:
		virtual void *Clone() {
			return (MovingShpere *)(new MovingShpere(*this));
		}

	public:
		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding)
		{
			AABB aabb0(mPos - Vec3f(mfRadius, mfRadius, mfRadius), mPos + Vec3f(mfRadius, mfRadius, mfRadius));
			AABB aabb1(mPos1 - Vec3f(mfRadius, mfRadius, mfRadius), mPos1 + Vec3f(mfRadius, mfRadius, mfRadius));

			bounding = AABB::SurroundingBox(aabb0, aabb1);

			return true;
		}

	public:
		inline void SetPos1(Vec3f const& pos1) { mPos1 = pos1; }
		inline void SetMovingTime(float time0, float time1) {
			mTime0 = time0;
			mTime1 = time1;
		}

	protected:
		virtual Vec3f getPos(float t) const {
			Vec3f pos = mPos + ((t - mTime0) / (mTime1 - mTime0)) * (mPos1 - mPos);
			return pos;
		}

	private:
		Vec3f	mPos1;
		float	mTime0;
		float	mTime1;

	};

}
