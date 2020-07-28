#pragma once

#include "ICloneable.h"
#include "Common.h"
#include "Vec3.h"
#include "HitRecord.h"
#include "Surface.h"

#define GLASS_COLOR		Color3f(0.95f, 0.95f, 1.0f)
#define DIMAOND_COLOR	Color3f(1.0f, 1.0f, 0.8f)
#define WATER_COLOR		Color3f(1.0f, 0.5f, 1.0f)

namespace LaplataRayTracer {

	//
	class BTDF : public ICloneable {
	public:
		BTDF() { }
		virtual ~BTDF() { }

	public:
		virtual void *Clone() = 0; // Not allowed us to create the instance.

	public:
		virtual Color3f F(const HitRecord& hitRec, const Vec3f& wo, const Vec3f& wi) const {
			return BLACK;
		}

		virtual Color3f Sample_f(const HitRecord& hitRec,  const Vec3f& wo, Vec3f& wi) const {
			return BLACK;
		}

		virtual Color3f Rho(const HitRecord& hitRec, const Vec3f& wo) const {
			return BLACK;
		}

	};

	//
	class FresnelTransmitter : public BTDF {
	public:
		FresnelTransmitter()
			: BTDF(), mEtaIn(1.5f), mEtaOut(1.0f) {

		}

		FresnelTransmitter(float etaIn, float etaOut)
			: BTDF(), mEtaIn(etaIn), mEtaOut(etaOut) {

		}

		virtual ~FresnelTransmitter() {

		}

	public:
		virtual void *Clone() { return (void *)(new FresnelTransmitter(*this)); }

	public:
		virtual Color3f Sample_f(const HitRecord& hitRec, const Vec3f& wo, Vec3f& wi /*tw*/) const {
			Vec3f n = hitRec.n;
			float cos_theta_i = Dot(n, wo);
			float eta = mEtaIn / mEtaOut;

			if (cos_theta_i < 0.0f) { // ray is passing from in-side to out-side
				cos_theta_i = -cos_theta_i;
				n = -n;
				eta = 1.0f / eta;
			}

			// now get the transmit output ray.
			Color3f L = BLACK;
//			if (Surface::Refract(-wo, n, eta, wi)) {
//				float kr = Surface::Fresnel(hitRec.n, wo, mEtaIn, mEtaOut);
//				L = (1.0f - kr) * (eta * eta) /* * WHITE */ / std::fabs(Dot(hitRec.n, wi));
//			}
            float temp = 1.0f - (1.0f - cos_theta_i * cos_theta_i) / (eta * eta);
            float cos_theta2 = std::sqrt(temp);
            wi = -wo / eta - (cos_theta2 - cos_theta_i / eta) * n;
            float kr = Surface::Fresnel(hitRec.n, wo, mEtaIn, mEtaOut);
            L = (1.0f - kr) / (eta * eta) * WHITE / std::fabs(Dot(hitRec.n, wi));

			return L;
		}

	public:
		inline bool IsTotalInternalReflection(const HitRecord& hitRec, const Vec3f& dir) const {
			float cos_theta_i = Dot(hitRec.n, dir);
			float eta = mEtaIn / mEtaOut;

			if (cos_theta_i < 0.0f) {
				eta = 1.0f / eta;
			}

			return (1.0f - (1.0f - cos_theta_i * cos_theta_i) / (eta * eta) < 0.0f);
		}

	public:
		inline void SetEtaIn(float etaIn) { mEtaIn = etaIn; }
		inline void SetEtaOut(float etaOut) { mEtaOut = etaOut; }

	private:
		float mEtaIn;
		float mEtaOut;

	};

}
