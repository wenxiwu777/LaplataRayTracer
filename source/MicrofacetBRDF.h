#pragma once
// This file is separeted from BRDF.h to avoid the file going too large.
//

namespace LaplataRayTracer {
// ----------------------------------------------------------------------------
// Utility functions to compute trigonometric funtions for a vector
// ----------------------------------------------------------------------------
    static inline float CosTheta(const Vec3f& w) {
        return w.Z();
    }

    static inline float Cos2Theta(const Vec3f& w) {
        float c = CosTheta(w);
        return c * c;
    }

    static inline float SinTheta(const Vec3f& w) {
        return std::sqrt(std::max(0.0f, 1.0f - Cos2Theta(w)));
    }

    static inline float TanTheta(const Vec3f& w) {
        return SinTheta(w) / CosTheta(w);
    }

    static inline float CosPhi(const Vec3f& w) {
        if (w.Z() == 1.0f) {
            return 0.0f;
        }

        return w.X() / SinTheta(w);
    }

    static inline float Cos2Phi(const Vec3f& w) {
        double c = CosPhi(w);
        return c * c;
    }

    static inline float SinPhi(const Vec3f& w) {
        if (w.Z() == 1.0f) {
            return 0.0;
        }

        return w.Y() / SinTheta(w);
    }

    static inline float Sin2Phi(const Vec3f& w) {
        float s = SinPhi(w);
        return s * s;
    }

// Rational approximation of inverse error function
// This method is borrowed from PBRT v3
    static inline float ErfInv(float x) {
        float w, p;
        x = RTMath::Clamp(x, -0.99999f, 0.99999f);
        w = -std::log((1 - x) * (1 + x));
        if (w < 5) {
            w = w - 2.5f;
            p = 2.81022636e-08f;
            p = 3.43273939e-07f + p * w;
            p = -3.5233877e-06f + p * w;
            p = -4.39150654e-06f + p * w;
            p = 0.00021858087f + p * w;
            p = -0.00125372503f + p * w;
            p = -0.00417768164f + p * w;
            p = 0.246640727f + p * w;
            p = 1.50140941f + p * w;
        } else {
            w = std::sqrt(w) - 3;
            p = -0.000200214257f;
            p = 0.000100950558f + p * w;
            p = 0.00134934322f + p * w;
            p = -0.00367342844f + p * w;
            p = 0.00573950773f + p * w;
            p = -0.0076224613f + p * w;
            p = 0.00943887047f + p * w;
            p = 1.00167406f + p * w;
            p = 2.83297682f + p * w;
        }
        return p * x;
    }

// Rational approximation of error function
// This method is borrowed from PBRT v3
    static inline float Erf(float x) {
        // constants
        float a1 = 0.254829592f;
        float a2 = -0.284496736f;
        float a3 = 1.421413741f;
        float a4 = -1.453152027f;
        float a5 = 1.061405429f;
        float p = 0.3275911f;

        // Save the sign of x
        int sign = 1;
        if (x < 0) sign = -1;
        x = std::abs(x);

        // A&S formula 7.1.26
        float t = 1 / (1 + p * x);
        float y =
                1 -
                (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * std::exp(-x * x);

        return sign * y;
    }

// ----------------------------------------------------------------------------
// The interface for microfacet distribution
// ----------------------------------------------------------------------------
    class MicrofacetDistribution {
    public:
        MicrofacetDistribution(float alphax, float alphay, bool sampleVisible)
                : alphax_{alphax}, alphay_{alphay}, sampleVisible_{sampleVisible} {
        }

        virtual ~MicrofacetDistribution() { }

        // Sample microfacet normal
        virtual Vec3f SampleWm(const Vec3f& wo) const = 0;

        // The lambda function, which appears in the slope-space sampling
        virtual float Lambda(const Vec3f& wo) const = 0;

        // Smith's masking function
        float G1(const Vec3f& wo) const {
            return 1.0 / (1.0 + Lambda(wo));
        }

        // Smith's masking-shadowing function
        float G(const Vec3f& wo, const Vec3f& wi) const {
            return G1(wo) * G1(wi);
        }

        // Weighting function
        float Weight(const Vec3f& wo, const Vec3f& wi, const Vec3f& wm) const {
            if (!sampleVisible_) {
                return std::abs(Dot(wi, wm)) * G(wo, wi) /
                       std::max((float)1.0e-8, std::abs(CosTheta(wi) * CosTheta(wm)));
            } else {
                return G1(wo);
            }
        }

        // Private parameters
        float alphax_, alphay_;
        bool sampleVisible_;
    };

// ----------------------------------------------------------------------------
// Beckmann distribution
// ----------------------------------------------------------------------------
    class BeckmannDistribution : public MicrofacetDistribution {
    public:
        BeckmannDistribution(float alphax, float alphay, bool sampleVisible)
                : MicrofacetDistribution{alphax, alphay, sampleVisible} {
        }

        Vec3f SampleWm(const Vec3f& wo) const override {
            const float U1 = Random::frand48();
            const float U2 = Random::frand48();

            if (!sampleVisible_) {
                // Sample half-vector without taking normal visibility into consideration
                float tan2Theta, phi;
                if (alphax_ == alphay_) {
                    // Isotropic case
                    // Following smapling formula can be found in [Walter et al. 2007]
                    // "Microfacet Models for Refraction through Rough Surfaces"
                    float alpha = alphax_;
                    float logSample = std::log(1.0 - U1);
                    tan2Theta = -alpha * alpha * logSample;
                    phi = 2.0 * PI_CONST * U2;
                } else {
                    // Anisotropic case
                    // Following sampling strategy is analytically derived from
                    // P.15 of [Heitz et al. 2014]
                    // "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"
                    float logSample = std::log(1.0 - U1);
                    phi = std::atan(alphay_ / alphax_ * std::tan(2.0 * PI_CONST * U2 + 0.5 * PI_CONST));
                    if (U2 > 0.5) {
                        phi += PI_CONST;
                    }

                    float sinPhi = std::sin(phi);
                    float cosPhi = std::cos(phi);
                    float alphax2 = alphax_ * alphax_;
                    float alphay2 = alphay_ * alphay_;
                    tan2Theta = -logSample / (cosPhi * cosPhi / alphax2 + sinPhi * sinPhi / alphay2);
                }

                // Compute normal direction from angle information sampled above
                float cosTheta = 1.0 / std::sqrt(1.0 + tan2Theta);
                float sinTheta = std::sqrt(std::max(0.0, 1.0 - cosTheta * cosTheta));
                Vec3f wm(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);

                if (wm.Z() < 0.0f) {
                    wm = -wm;
                }

                return wm;
            } else {
                // Sample microfacet normals by considering only visible normals
                bool flip = wo.Z() < 0.0f;
                Vec3f wm = SampleBeckmann(flip ? -wo : wo, alphax_, alphay_);

                if (wm.Z() < 0.0) {
                    wm = -wm;
                }

                return wm;
            }
        }

        float Lambda(const Vec3f& wo) const override {
            float cosThetaO = CosTheta(wo);
            float sinThetaO = SinTheta(wo);
            float absTanThetaO = std::abs(TanTheta(wo));
            if (std::isinf(absTanThetaO)) return 0.;

            float alpha = std::sqrt(cosThetaO * cosThetaO * alphax_ * alphax_ +
                                     sinThetaO * sinThetaO * alphay_ * alphay_);
            float a = 1.0f / (alpha * absTanThetaO);
            return (Erf(a) - 1.0f) / 2.0f + std::exp(-a * a) / (2.0f * a * std::sqrt(PI_CONST) + 1.0e-6);
        }

        // Sample microfacet normal through slope distribution
        static Vec3f SampleBeckmann(const Vec3f& wi, float alphax, float alphay) {
            // 1. stretch wi
            Vec3f wiStretched = Vec3f(alphax * wi.X(), alphay * wi.Y(), wi.Z());
            wiStretched.MakeUnit();

            // 2. simulate P22_{wi}(x_slope, y_slope, 1, 1)
            float slopex, slopey;
            SampleBeckman_P22_11(CosTheta(wiStretched), &slopex, &slopey);

            // 3. rotate
            float tmp = CosPhi(wiStretched) * slopex - SinPhi(wiStretched) * slopey;
            slopey = SinPhi(wiStretched) * slopex + CosPhi(wiStretched) * slopey;
            slopex = tmp;

            // 4. unstretch
            slopex = alphax * slopex;
            slopey = alphay * slopey;

            // 5. compute normal
            Vec3f  n = Vec3f(-slopex, -slopey, 1.0f);
            n.MakeUnit();
            return n;
        }

        // Sample slope distribution with alphax and alphay equal to one
        static void SampleBeckman_P22_11(float cosThetaI, float *slopex, float *slopey) {

            const float U1 = Random::frand48();
            const float U2 = Random::frand48();

            // The special case where the ray comes from normal direction
            // The following sampling is equivarent to the sampling of
            // microfacet normals (not slopes) on isotropic rough surface
            if (cosThetaI > 0.9999f) {
                float r = std::sqrt(-std::log(1.0f - U1));
                float sinPhi = std::sin(2 * PI_CONST * U2);
                float cosPhi = std::cos(2 * PI_CONST * U2);
                *slopex = r * cosPhi;
                *slopey = r * sinPhi;
                return;
            }

            float sinThetaI = std::sqrt(std::max(0.0f, 1.0f - cosThetaI * cosThetaI));
            float tanThetaI = sinThetaI / cosThetaI;
            float cotThetaI = 1.0f / tanThetaI;
            const float erfCotThetaI = Erf(cotThetaI);

            // Initialize lower and higher bounds for bisection method
            float lower = -1.0f;
            float higher = 1.0f;

            // Normalization factor for the CDF
            // This is equivarent to (G1 / 2)^{-1}
            static const float SQRT_PI_INV = 1.0f / std::sqrt(PI_CONST);
            float normalization =
                    1.0f / (1.0f + erfCotThetaI + SQRT_PI_INV * tanThetaI * std::exp(-cotThetaI * cotThetaI));

            // The following bisection method acquires "erf(x_m)"
            int it = 0;
            float samplex = std::max(U1, (float)1e-6);
            while (std::abs(higher - lower) > 1.0e-6) {
                // Bisection
                float mid = 0.5f * (lower + higher);

                // Evaluation for current estimation
                float x_m = ErfInv(mid);
                float value = normalization * (1.0f + mid + SQRT_PI_INV * tanThetaI * std::exp(-x_m * x_m)) - samplex;

                /* Update bisection intervals */
                if (value > 0.0f) {
                    higher = mid;
                } else {
                    lower = mid;
                }
            }

            // Compute slopex from erf(x_m) given by above bisection method
            *slopex = ErfInv(lower);

            // Sample y_m
            *slopey = ErfInv(2.0f * std::max(U2, (float)1e-6) - 1.0f);
        }
    };

// ----------------------------------------------------------------------------
// GGX distribution
// ----------------------------------------------------------------------------
    class GGXDistribution : public MicrofacetDistribution {
    public:
        GGXDistribution(float alphax, float alphay, bool sampleVisible)
                : MicrofacetDistribution{alphax, alphay, sampleVisible} {
        }

        Vec3f SampleWm(const Vec3f& wo) const override {
            const float U1 = Random::frand48();
            const float U2 = Random::frand48();

            if (!sampleVisible_) {
                float tan2Theta, phi;
                if (alphax_ == alphay_) {
                    // Isotropic case
                    float alpha = alphax_;
                    tan2Theta = alpha * alpha * U1 / (1.0f - U1);
                    phi = 2.0f * PI_CONST * U2;
                } else {
                    // Anisotropic case
                    phi = std::atan(alphay_ / alphax_ * std::tan(2.0f * PI_CONST * U2 + 0.5f * PI_CONST));
                    if (U2 > 0.5f) {
                        phi += PI_CONST;
                    }

                    float sinPhi = std::sin(phi);
                    float cosPhi = std::cos(phi);
                    float alphax2 = alphax_ * alphax_;
                    float alphay2 = alphay_ * alphay_;
                    float alpha2 = 1.0f / (cosPhi * cosPhi / alphax2 + sinPhi * sinPhi / alphay2);
                    tan2Theta = U1 / (1.0f - U1) * alpha2;
                }

                // Compute normal direction from angle information sampled above
                float cosTheta = 1.0f / std::sqrt(1.0f + tan2Theta);
                float sinTheta = std::sqrt(std::max(0.0f, 1.0f - cosTheta * cosTheta));
                Vec3f wm = Vec3f(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);

                if (wm.Z() < 0.0f) {
                    wm = -wm;
                }

                return wm;
            } else {
                // Sample microfacet normals by considering only visible normals
                bool flip = wo.Z() < 0.0f;
                Vec3f wm = SampleGGX(flip ? -wo : wo, alphax_, alphay_);

                if (wm.Z() < 0.0) {
                    wm = -wm;
                }

                return wm;
            }
        }

        static Vec3f SampleGGX(const Vec3f& wi, float alphax, float alphay) {
            // 1. stretch wi
            Vec3f wiStretched = Vec3f(alphax * wi.X(), alphay * wi.Y(), wi.Z());
            wiStretched.MakeUnit();

            // 2. simulate P22_{wi}(x_slope, y_slope, 1, 1)
            float slopex, slopey;
            SampleGGX_P22_11(CosTheta(wiStretched), &slopex, &slopey);

            // 3. rotate
            float tmp = CosPhi(wiStretched) * slopex - SinPhi(wiStretched) * slopey;
            slopey = SinPhi(wiStretched) * slopex + CosPhi(wiStretched) * slopey;
            slopex = tmp;

            // 4. unstretch
            slopex = alphax * slopex;
            slopey = alphay * slopey;

            // 5. compute normal
            Vec3f n = Vec3f(-slopex, -slopey, 1.0f);
            n.MakeUnit();
            return n;
        }

        static void SampleGGX_P22_11(float cosThetaI, float *slopex, float *slopey) {
            const float U1 = Random::frand48();
            float U2 = Random::frand48();

            // The special case where the ray comes from normal direction
            // The following sampling is equivarent to the sampling of
            // microfacet normals (not slopes) on isotropic rough surface
            if (cosThetaI > 0.9999f) {
                float r = std::sqrt(U1 / (1.0 - U1));
                float sinPhi = std::sin(2 * PI_CONST * U2);
                float cosPhi = std::cos(2 * PI_CONST * U2);
                *slopex = r * cosPhi;
                *slopey = r * sinPhi;
                return;
            }

            float sinThetaI = std::sqrt(std::max(0.0f, 1.0f - cosThetaI * cosThetaI));
            float tanThetaI = sinThetaI / cosThetaI;
            float a = 1.0f / tanThetaI;
            float G1 = 2.0f / (1.0f + std::sqrt(1.0f + 1.0f / (a * a)));

            // Sample slopex
            float A = 2.0f * U1 / G1 - 1.0f;
            float B = tanThetaI;
            float tmp = std::min(1.0f / (A * A - 1.0f), (float)1.0e12);

            float D = std::sqrt(B * B * tmp * tmp - (A * A - B * B) * tmp);
            float slopex1 = B * tmp - D;
            float slopex2 = B * tmp + D;
            *slopex = (A < 0.0f || slopex2 > 1.0f / tanThetaI) ? slopex1 : slopex2;

            // Sample slopey
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
            *slopey = S * z * std::sqrt(1.0f + (*slopex) * (*slopex));
        }

        float Lambda(const Vec3f& wo) const override {
            float absTanThetaO = std::abs(TanTheta(wo));
            if (std::isinf(absTanThetaO)) {
                return 0.0f;
            }

            float alpha = std::sqrt(Cos2Phi(wo) * alphax_ * alphax_ + Sin2Phi(wo) * alphay_ * alphay_);
            float alpha2Tan2Theta = (alpha * absTanThetaO) * (alpha * absTanThetaO);
            return (-1.0f + std::sqrt(1.0f + alpha2Tan2Theta)) / 2.0f;
        }
    };

}

#define USE_GGX_MICROFACET
//#define USE_BECKMANN_MICROFACET