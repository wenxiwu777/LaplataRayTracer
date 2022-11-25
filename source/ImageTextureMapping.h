#pragma once

#include "Common.h"
#include "Utility.h"
#include "Vec3.h"
#include "HitRecord.h"

namespace LaplataRayTracer
{
	//
	class ImageTextureMapping {
	public:
		virtual ~ImageTextureMapping() { }

	public:
		virtual void DoMapping(HitRecord& hitRec) = 0;

	};

	//
	class ShpereicalTextureMapping : public ImageTextureMapping {
	public:
		ShpereicalTextureMapping() { }
		virtual ~ShpereicalTextureMapping() { }

	public:
		virtual void DoMapping(HitRecord& hitRec) {
			Vec3f pole_(0.0f, 1.0f, 0.0f);
			Vec3f equator_(0.0f, 0.0f, 1.0f);
			
			float phi_ = std::acos(-Dot(hitRec.n, pole_));
            if (RTMath::IsNan(phi_)) {
                phi_ = 1e-4;
            }
			hitRec.v = phi_ / PI_CONST;

			float theta_ = std::acos((Dot(equator_, hitRec.n)) / std::sin(phi_)) / TWO_PI_CONST;
			float temp_ = Dot(Cross(pole_, equator_), hitRec.n);
			if (temp_ > 0.0f) {
				hitRec.u = theta_;
			}
			else {
				hitRec.u = 1.0f - theta_;
			}
			hitRec.v = 1.0f - hitRec.v;
		}

	};

	//
	// ... ...
    class SphericalMapping : public ImageTextureMapping {
    public:
        SphericalMapping() { }
        virtual ~SphericalMapping() { }

    public:
        virtual void DoMapping(HitRecord& hitRec) {
            float theta = std::acos(hitRec.pt.Y());
            float phi = std::atan2(hitRec.pt.X(), hitRec.pt.Z());
            if (RTMath::IsNan(theta)) {
                theta = 1e-4;
            }
            if (RTMath::IsNan(phi)) {
                phi = 1e-4;
            }
            if (phi < 0.0f) {
                phi += 2.0f * PI_CONST;
            }

            float u = phi * INV_TWO_PI_CONST;
            float v = 1.0f - theta * INV_PI_CONST;

            hitRec.u = u;
            hitRec.v = 1.0f - v;

        }
    };

    //
    class LightProbMapping : public ImageTextureMapping {
    public:
        enum EMappingType { PROBE = 0, PANORAMIC, };

    public:
        LightProbMapping()
        : mType(EMappingType::PROBE) {

        }

        virtual ~LightProbMapping() {

        }

    public:
        virtual void DoMapping(HitRecord& hitRec) {
            float x = hitRec.pt.X();
            float y = hitRec.pt.Y();
            float z = hitRec.pt.Z();

            float d = std::sqrt(x * x + y * y);
            float sin_beta = y / d;
            float cos_beta = x / d;

            if (RTMath::IsZero(d)) {
                hitRec.u = 0.5f;
                hitRec.v = 0.5f;
                return;
            }

            float alpha = 0.0f;
            if (mType == EMappingType::PROBE) {
                alpha = std::acos(z);
            } else if (mType == EMappingType::PANORAMIC) {
                alpha = std::acos(-z);
            }
            if (RTMath::IsNan(alpha)) {
                alpha = 1e-4;
            }

            float r = alpha * INV_PI_CONST;
            float u = (1.0f + r * cos_beta) * 0.5f;
            float v = (1.0f + r * sin_beta) * 0.5f;

            hitRec.u = u;
            hitRec.v = 1.0f - v;

        }

    public:
        inline void SetMappingType(EMappingType type) {
            mType = type;
        }

    private:
        EMappingType mType;

    };

}
