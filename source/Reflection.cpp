#include "Utility.h"
#include "Reflection.h"

namespace LaplataRayTracer {

    /*static void BeckmannSample11(float cosThetaI, float U1, float U2,
                                 float *slope_x, float *slope_y) {
        if (cosThetaI > 0.9999f) {
            float r = std::sqrt(-std::log(1.0f - U1));
            float sinPhi = std::sin(TWO_PI_CONST * U2);
            float cosPhi = std::cos(TWO_PI_CONST * U2);
            *slope_x = r * cosPhi;
            *slope_y = r * sinPhi;
            return;
        }

        float sinThetaI =
                std::sqrt(std::max((float)0, (float)1 - cosThetaI * cosThetaI));
        float tanThetaI = sinThetaI / cosThetaI;
        float cotThetaI = 1.0f / tanThetaI;
        
        float a = -1.0f, c = RTMath::Erf(cotThetaI);
        float sample_x = std::max(U1, (float)1e-6f);
        
        float thetaI = std::acos(cosThetaI);
        float fit = 1.0f + thetaI * (-0.876f + thetaI * (0.4265f - 0.0594f * thetaI));
        float b = c - (1.0f + c) * std::pow(1.0f - sample_x, fit);
        
        static const float SQRT_PI_INV = 1.0f / std::sqrt(PI_CONST);
        float normalization =
                1.0f /
                (1.0f + c + SQRT_PI_INV * tanThetaI * std::exp(-cotThetaI * cotThetaI));

        int it = 0;
        while (++it < 10) {
            if (!(b >= a && b <= c)) {
                b = 0.5f * (a + c);
            }
            
            float invErf = RTMath::ErfInv(b);
            float value =
                    normalization *
                    (1.0f + b + SQRT_PI_INV * tanThetaI * std::exp(-invErf * invErf)) -
                    sample_x;
            float derivative = normalization * (1.0f - invErf * tanThetaI);

            if (std::abs(value) < 1e-5f) {
                break;
            }
            
            if (value > 0) {
                c = b;   
            }
            else {
                a = b;
            }
            
            b -= value / derivative;
        }
        
        *slope_x = RTMath::ErfInv(b);
        *slope_y = RTMath::ErfInv(2.0f * std::max(U2, (float)1e-6f) - 1.0f);
    }

    static Vec3f BeckmannSample(const Vec3f &wi, float alpha_x, float alpha_y,
                                   float U1, float U2, const Vec3f& n) {
        // 1. stretch wi
        Vec3f wiStretched = Vec3f(alpha_x * wi.X(), alpha_y * wi.Y(), wi.Z());
        wiStretched.MakeUnit();

        // 2. simulate P22_{wi}(x_slope, y_slope, 1, 1)
        float slope_x, slope_y;
        BeckmannSample11(BRDF::CosTheta(wiStretched, n), U1, U2, &slope_x, &slope_y);

        // 3. rotate
        float tmp = BRDF::CosPhi(wiStretched, n) * slope_x - BRDF::SinPhi(wiStretched, n) * slope_y;
        slope_y = BRDF::SinPhi(wiStretched, n) * slope_x + BRDF::CosPhi(wiStretched, n) * slope_y;
        slope_x = tmp;

        // 4. unstretch
        slope_x = alpha_x * slope_x;
        slope_y = alpha_y * slope_y;

        // 5. compute normal
        Vec3f vec_ret = Vec3f(-slope_x, -slope_y, 1.0f);
        vec_ret.MakeUnit();
        return vec_ret;
    }

    float BeckmannDistribution::D(const Vec3f &wh, const Vec3f& n) const {
        float tan2Theta = BRDF::Tan2Theta(wh, n);
        if (std::isinf(tan2Theta)) return 0.;
        float cos4Theta = BRDF::Cos2Theta(wh, n) * BRDF::Cos2Theta(wh, n);
        return std::exp(-tan2Theta * (BRDF::Cos2Phi(wh, n) / (mAlphax * mAlphax) +
                BRDF::Sin2Phi(wh, n) / (mAlphay * mAlphay))) /
               (PI_CONST * mAlphax * mAlphay * cos4Theta);
    }

    float TrowbridgeReitzDistribution::D(const Vec3f &wh, const Vec3f& n) const {
        float tan2Theta = BRDF::Tan2Theta(wh, n);
        if (std::isinf(tan2Theta)) {
            return 0.0f;
        }
        const float cos4Theta = BRDF::Cos2Theta(wh, n) * BRDF::Cos2Theta(wh, n);
        float e = (BRDF::Cos2Phi(wh, n) / (mAlphax * mAlphax) + BRDF::Sin2Phi(wh, n) / (mAlphay * mAlphay)) * tan2Theta;
        return 1.0f / (PI_CONST * mAlphax * mAlphay * cos4Theta * (1.0f + e) * (1.0f + e));
    }

    float BeckmannDistribution::Lambda(const Vec3f &w, const Vec3f& n) const {
        float absTanTheta = std::abs(BRDF::TanTheta(w, n));
        if (std::isinf(absTanTheta)) {
            return 0.0f;
        }
        // Compute _alpha_ for direction _w_
        float alpha = std::sqrt(BRDF::Cos2Phi(w, n) * mAlphax * mAlphax + BRDF::Sin2Phi(w, n) * mAlphay * mAlphay);
        float a = 1.0f / (alpha * absTanTheta);
        if (a >= 1.6f) {
            return 0.0f;
        }
        return (1.0f - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
    }

    float TrowbridgeReitzDistribution::Lambda(const Vec3f &w, const Vec3f& n) const {
        float absTanTheta = std::abs(BRDF::TanTheta(w, n));
        if (std::isinf(absTanTheta)) {
            return 0.0f;
        }
        // Compute _alpha_ for direction _w_
        float alpha = std::sqrt(BRDF::Cos2Phi(w, n) * mAlphax * mAlphax + BRDF::Sin2Phi(w, n) * mAlphay * mAlphay);
        float alpha2Tan2Theta = (alpha * absTanTheta) * (alpha * absTanTheta);
        return (-1.0f + std::sqrt(1.0f + alpha2Tan2Theta)) / 2.0f;
    }

    Vec3f BeckmannDistribution::Sample_wh(const Vec3f &wo, const Point2f &u, const Vec3f& n) const {
        if (!mSampleVisibleArea) {
            float tan2Theta, phi;
            if (mAlphax == mAlphay) {
                float logSample = std::log(1.0f - u.X());
                tan2Theta = -mAlphax * mAlphax * logSample;
                phi = u[1] * TWO_PI_CONST;
            } else {
                float logSample = std::log(1.0f - u.X());
                phi = std::atan(mAlphay / mAlphax * std::tan(TWO_PI_CONST * u.Y() + 0.5f * PI_CONST));
                if (u.Y() > 0.5f) {
                    phi += PI_CONST;
                }
                float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
                float alphax2 = mAlphax * mAlphax, alphay2 = mAlphay * mAlphay;
                tan2Theta = -logSample / (cosPhi * cosPhi / alphax2 + sinPhi * sinPhi / alphay2);
            }

            // Map sampled Beckmann angles to normal direction _wh_
            float cosTheta = 1.0f / std::sqrt(1.0f + tan2Theta);
            float sinTheta = std::sqrt(std::max((float)0, 1.0f - cosTheta * cosTheta));
            Vec3f wh = Vec3f(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
            if (!BRDF::SameHemisphere(wo, wh, n)) {
                wh = -wh;
            }
            return wh;

        } else {
            // Sample visible area of normals for Beckmann distribution
            Vec3f wh;
            bool flip = wo.Z() < 0.0f;
            wh = BeckmannSample(flip ? -wo : wo, mAlphax, mAlphay, u.X(), u.Y(), n);
            if (flip) {
                wh = -wh;
            }
            return wh;
        }
    }

    static void TrowbridgeReitzSample11(float cosTheta, float U1, float U2,
                                        float *slope_x, float *slope_y) {
        // special case (normal incidence)
        if (cosTheta > 0.9999f) {
            float r = std::sqrt(U1 / (1.0f - U1));
            float phi = 6.28318530718f * U2;
            *slope_x = r * std::cos(phi);
            *slope_y = r * std::sin(phi);
            return;
        }

        float sinTheta = std::sqrt(std::max((float)0, (float)1 - cosTheta * cosTheta));
        float tanTheta = sinTheta / cosTheta;
        float a = 1.0f / tanTheta;
        float G1 = 2.0f / (1.0f + std::sqrt(1.0f + 1.0f / (a * a)));

        // sample slope_x
        float A = 2.0f * U1 / G1 - 1.0f;
        float tmp = 1.0f / (A * A - 1.0f);
        if (tmp > 1e10) {
            tmp = 1e10;
        }
        float B = tanTheta;
        float D = std::sqrt(std::max(float(B * B * tmp * tmp - (A * A - B * B) * tmp), float(0)));
        float slope_x_1 = B * tmp - D;
        float slope_x_2 = B * tmp + D;
        *slope_x = (A < 0.0f || slope_x_2 > 1.0f / tanTheta) ? slope_x_1 : slope_x_2;

        // sample slope_y
        float S;
        if (U2 > 0.5f) {
            S = 1.0f;
            U2 = 2.0f * (U2 - 0.5f);
        } else {
            S = -1.0f;
            U2 = 2.0f * (0.5f - U2);
        }
        float z = (U2 * (U2 * (U2 * 0.27385f - 0.73369f) + 0.46341f)) /
                (U2 * (U2 * (U2 * 0.093073f + 0.309420f) - 1.000000f) + 0.597999f);
        *slope_y = S * z * std::sqrt(1.f + *slope_x * *slope_x);
    }

    static Vec3f TrowbridgeReitzSample(const Vec3f &wi, float alpha_x,
                                          float alpha_y, float U1, float U2, const Vec3f& n) {
        // 1. stretch wi
        Vec3f wiStretched = Vec3f(alpha_x * wi.X(), alpha_y * wi.Y(), wi.Z());
        wiStretched.MakeUnit();

        // 2. simulate P22_{wi}(x_slope, y_slope, 1, 1)
        float slope_x, slope_y;
        TrowbridgeReitzSample11(BRDF::CosTheta(wiStretched, n), U1, U2, &slope_x, &slope_y);

        // 3. rotate
        float tmp = BRDF::CosPhi(wiStretched, n) * slope_x - BRDF::SinPhi(wiStretched, n) * slope_y;
        slope_y = BRDF::SinPhi(wiStretched, n) * slope_x + BRDF::CosPhi(wiStretched, n) * slope_y;
        slope_x = tmp;

        // 4. unstretch
        slope_x = alpha_x * slope_x;
        slope_y = alpha_y * slope_y;

        // 5. compute normal
        Vec3f vec_ret(-slope_x, -slope_y, 1.0f);
        vec_ret.MakeUnit();
        return vec_ret;
    }

    Vec3f TrowbridgeReitzDistribution::Sample_wh(const Vec3f &wo, const Point2f &u, const Vec3f& n) const {
        Vec3f wh;
        if (!mSampleVisibleArea) {
            float cosTheta = 0.0f, phi = TWO_PI_CONST * u.Y();
            if (mAlphax == mAlphay) {
                float tanTheta2 = mAlphax * mAlphax * u.X() / (1.0f - u.X());
                cosTheta = 1.0f / std::sqrt(1.0f + tanTheta2);
            } else {
                phi = std::atan(mAlphay / mAlphax * std::tan(TWO_PI_CONST * u.Y() + 0.5f * PI_CONST));
                if (u.Y() > 0.5f) {
                    phi += PI_CONST;
                }
                float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
                const float alphax2 = mAlphax * mAlphax, alphay2 = mAlphay * mAlphay;
                const float alpha2 = 1 / (cosPhi * cosPhi / alphax2 + sinPhi * sinPhi / alphay2);
                float tanTheta2 = alpha2 * u.X() / (1.0f - u.X());
                cosTheta = 1.0f / std::sqrt(1.0f + tanTheta2);
            }
            float sinTheta = std::sqrt(std::max((float)0.0, (float)1.0 - cosTheta * cosTheta));
            wh = Vec3f(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
            if (!BRDF::SameHemisphere(wo, wh, n)) {
                wh = -wh;
            }
        } else {
            bool flip = wo.Z() < 0;
            wh = TrowbridgeReitzSample(flip ? -wo : wo, mAlphax, mAlphay, u.X(), u.Y(), n);
            if (flip) {
                wh = -wh;
            }
        }
        return wh;
    }

    float MicrofacetDistribution::Pdf(const Vec3f &wo, const Vec3f &wh, const Vec3f& n) const {
        if (mSampleVisibleArea) {
            return D(wh, n) * G1(wo, n) * std::abs(Dot(wo, wh)) / std::abs(Dot(wo, wh));
        }

        return D(wh, n) * BRDF::AbsCosTheta(wh, n);
    }
	*/
    //
    Color3f FresnelConductor::Evaluate(float cosThetaI) const {
        return FresnelConductor::FrConductor(std::abs(cosThetaI), mEtaI, mEtaT, mK);
    }

    // https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
    Color3f FresnelConductor::FrConductor(float cosThetaI, const Color3f &etai,
                                           const Color3f &etat, const Color3f &k) {
        cosThetaI = RTMath::Clamp(cosThetaI, -1.0f, 1.0f);
        Color3f eta = etat / etai;
        Color3f etak = k / etai;

        float cosThetaI2 = cosThetaI * cosThetaI;
        float sinThetaI2 = 1.0f - cosThetaI2;
        Color3f eta2 = eta * eta;
        Color3f etak2 = etak * etak;

        Color3f t0 = eta2 - etak2 - sinThetaI2;
        Vec3f temp = t0 * t0 + 4.0f * eta2 * etak2;
        Color3f a2plusb2 = temp.Sqrt();
        Color3f t1 = a2plusb2 + cosThetaI2;
        Vec3f temp2 = 0.5f * (a2plusb2 + t0);
        Color3f a = temp2.Sqrt();
        Color3f t2 = (float)2 * cosThetaI * a;
        Color3f Rs = (t1 - t2) / (t1 + t2);

        Color3f t3 = cosThetaI2 * a2plusb2 + sinThetaI2 * sinThetaI2;
        Color3f t4 = t2 * sinThetaI2;
        Color3f Rp = Rs * (t3 - t4) / (t3 + t4);

        return 0.5f * (Rp + Rs);
    }

    Color3f FresnelDielectric::Evaluate(float cosThetaI) const {
        return FresnelDielectric::FrDielectric(cosThetaI, mEtaI, mEtaT);
    }

    float FresnelDielectric::FrDielectric(float cosThetaI, float etaI, float etaT) {
        cosThetaI = RTMath::Clamp(cosThetaI, -1.0f, 1.0f);
        // Potentially swap indices of refraction
        bool entering = cosThetaI > 0.0f;
        if (!entering) {
            std::swap(etaI, etaT);
            cosThetaI = std::abs(cosThetaI);
        }

        // Compute _cosThetaT_ using Snell's law
        float sinThetaI = std::sqrt(std::max((float)0, 1.0f - cosThetaI * cosThetaI));
        float sinThetaT = etaI / etaT * sinThetaI;

        // Handle total internal reflection
        if (sinThetaT >= 1.0f) {
            return 1.0f;
        }

        float cosThetaT = std::sqrt(std::max((float)0, 1.0f - sinThetaT * sinThetaT));
        float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) /
                      ((etaT * cosThetaI) + (etaI * cosThetaT));
        float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) /
                      ((etaI * cosThetaI) + (etaT * cosThetaT));
        return (Rparl * Rparl + Rperp * Rperp) / 2.0f;
    }
/*
    //
    Color3f MicrofacetReflection::F(const HitRecord& hitRec, const Vec3f& wo, const Vec3f& wi) const {
//		ShadingCoord shadingCoord(hitRec.dpdu, hitRec.dpdv);
//		Vec3f lwi = shadingCoord.WorldToLocal(wi);
//		Vec3f lwo = shadingCoord.WorldToLocal(wo);

        float cosThetaO = BRDF::AbsCosTheta(wo, hitRec.n), cosThetaI = BRDF::AbsCosTheta(wi, hitRec.n);
        Vec3f wh = wi + wo;

        if (cosThetaI == 0 || cosThetaO == 0) {
            return BLACK;
        }
        if (wh.X() == 0 && wh.Y() == 0 && wh.Z() == 0) {
            return BLACK;
        }
        wh.MakeUnit();

        Color3f f = mpFresnel->Evaluate(Dot(wi, ((Dot(wh, Vec3f(0.0f,0.0f,1.0f)) < 0.0f) ? -wh : wh)));
		Color3f r = mR * mpDistribution->D(wh, hitRec.n) * mpDistribution->G(wo, wi, hitRec.n) * f / (4.0f * cosThetaI * cosThetaO);

        return r;
    }

    Color3f MicrofacetReflection::Sample_f_pdf(const HitRecord& hitRec, const Vec3f& wo, Vec3f& wi, float& pdf) const {
//		ShadingCoord shadingCoord(hitRec.dpdu, hitRec.dpdv);
//		Vec3f lwo = shadingCoord.WorldToLocal(wo);

        if (RTMath::IsZero(BRDF::CosTheta(wo, hitRec.n))) {
            return BLACK;
        }

        Point2f u = SamplerBase::SampleInUnitDisk();
        Vec3f wh = mpDistribution->Sample_wh(wo, u, hitRec.n);
        if (Dot(wo, wh) < 0.0f) {
            return 0.0f;   // Should be rare
        }
        wi = Surface::Reflect(-wo, wh);
        if (!BRDF::SameHemisphere(wo, wi, hitRec.n)) {
            return BLACK;
        }

        pdf = mpDistribution->Pdf(wo, wh, hitRec.n) / (4.0f * Dot(wo, wh));
//		wi = shadingCoord.LocalToWorld(wi);
		Color3f f = F(hitRec, wo, wi);
		return f;
    }

//    float MicrofacetReflection::Pdf(const Vec3f &wo, const Vec3f &wi) const {
//        if (!BRDF::SameHemisphere(wo, wi)) {
//            return 0.0f;
//        }
//        Vec3f wh = wo + wi;
//        wh.MakeUnit();
//        return mpDistribution->Pdf(wo, wh) / (4.0f * Dot(wo, wh));
//    }
*/
}
