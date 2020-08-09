#pragma once

#include "Random.h"
#include "GeometricObject.h"
#include "Material.h"

namespace LaplataRayTracer
{
	class ConstantMedium : public GeometricObject {
	public:
		ConstantMedium(GeometricObject *boundary, float density, Material *phaseFunc)
			: mpBoundary(boundary), mfDensity(density), mpPhaseFunction(phaseFunc) {

		}

		virtual ~ConstantMedium() {
			if (mpBoundary) {
				delete mpBoundary;
				mpBoundary = nullptr;
			}

			if (mpPhaseFunction) {
				delete mpPhaseFunction;
				mpPhaseFunction = nullptr;
			}
		}

	public:
		virtual void *Clone() {
			return (ConstantMedium *)(new ConstantMedium((GeometricObject *)mpBoundary->Clone(), mfDensity, (Material *)mpPhaseFunction->Clone()));
		}

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec) {
			HitRecord rec1, rec2;
			float TMAX = FLT_MAX;
			if (mpBoundary->HitTest(inRay, -TMAX, TMAX, rec1)) {
				TMAX = FLT_MAX;
				float tmin2 = rec1.t + 0.0001f;
				if (mpBoundary->HitTest(inRay, tmin2, TMAX, rec2)) {
					if (rec1.t < tmin) { rec1.t = tmin; }
					if (rec2.t > tmax) { rec2.t = tmax; }
					if (rec1.t > tmax) { rec2.t = tmax; }
					if (rec1.t >= rec2.t) { return false; }
					if (rec1.t < 0.0f) { rec1.t = 0.0f; }

					float distance_inside_boundary = (rec2.t - rec1.t) * inRay.D().Length();
					float hit_distance = -(1 / mfDensity) * std::log(Random::frand48());
					if (hit_distance < distance_inside_boundary) {
						rec.hit = true;
						rec.t = rec1.t + hit_distance / inRay.D().Length();
						rec.pt = inRay.O() + rec.t * inRay.D();
						rec.n = Vec3f(1.0f, 0.0f, 0.0f);
						rec.pMaterial = mpPhaseFunction;
						tmax = rec.t;

						return true;
					}
				}
			}

			return false;
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const {
			return false;
		}

	public:
		virtual float Area() const
		{
			return 0.0f;
		}

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding) {
			bool get_bounding = mpBoundary->GetBoundingBox(t0, t1, bounding);
			return get_bounding;
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const
		{
			return WORLD_ORIGIN;
		}

        virtual Vec3f SampleRandomPoint() const
		{
			return WORLD_ORIGIN;
		}

		virtual void Update(float t) {
			// to do nothing.
		}

		virtual bool IsCompound() const {
			return false;
		}

		virtual float PDFValue(Vec3f const& o, Vec3f const& v) const {
			return 0.0f;
		}

        virtual Vec3f SampleRandomDirection(Vec3f const& v) const {
			return Vec3f(0.0f, 0.0f, 0.0f);
		}

	private:
		GeometricObject *		mpBoundary;
		float					mfDensity;
		Material *				mpPhaseFunction;

	};

}
