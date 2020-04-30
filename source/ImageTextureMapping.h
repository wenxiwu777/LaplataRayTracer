#pragma once

#include "Common.h"
#include "Vec3.h"
#include "HitRecord.h"

namespace LaplataRayTracer
{
	//
	class ImageTextureMapping {
	public:
		virtual ~ImageTextureMapping() { }

	public:
		virtual void DoInverseMapping(HitRecord& hitRec) = 0;

	};

	//
	class ShpereicalTextureMapping : public ImageTextureMapping {
	public:
		ShpereicalTextureMapping() { }
		virtual ~ShpereicalTextureMapping() { }

	public:
		virtual void DoInverseMapping(HitRecord& hitRec) {
			Vec3f pole_(0.0f, 1.0f, 0.0f);
			Vec3f equator_(0.0f, 0.0f, 1.0f);
			
			float phi_ = std::acos(-Dot(hitRec.n, pole_));
			hitRec.v = phi_ / PI_CONST;

			float theta_ = std::acos((Dot(equator_, hitRec.n)) / std::sin(phi_)) / (2.0f * PI_CONST);
			float temp_ = Dot(Cross(pole_, equator_), hitRec.n);
			if (temp_ > 0.0f) {
				hitRec.u = theta_;
			}
			else {
				hitRec.u = 1.0f - theta_;
			}
		}

	};

	//
	// ... ...

}
