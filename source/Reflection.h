#ifndef LAPLATARAYTRACER_REFLECTION_H
#define LAPLATARAYTRACER_REFLECTION_H

#pragma once

#include "Common.h"
#include "Vec3.h"
#include "ICloneable.h"

namespace LaplataRayTracer {
/*
    //
    class MicrofacetDistribution : public ICloneable {
    public:
        virtual ~MicrofacetDistribution() { }
        virtual float D(const Vec3f &wh, const Vec3f& n) const = 0;
        virtual float Lambda(const Vec3f &w, const Vec3f& n) const = 0;
        float G1(const Vec3f &w, const Vec3f& n) const {
            //    if (Dot(w, wh) * CosTheta(w) < 0.) return 0.;
            return 1.0f / (1.0f + Lambda(w, n));
        }
        virtual float G(const Vec3f &wo, const Vec3f &wi, const Vec3f& n) const {
            return 1.0f / (1.0f + Lambda(wo, n) + Lambda(wi, n));
        }
        virtual Vec3f Sample_wh(const Vec3f &wo, const Point2f &u, const Vec3f& n) const = 0;
        float Pdf(const Vec3f &wo, const Vec3f &wh, const Vec3f& n) const;

    protected:
        MicrofacetDistribution(bool sampleVisibleArea)
                : mSampleVisibleArea(sampleVisibleArea) {}

        protected:
            const bool mSampleVisibleArea;
    };

    class BeckmannDistribution : public MicrofacetDistribution {
    public:
        static float RoughnessToAlpha(float roughness) {
            roughness = std::max<float>(roughness, (float)1e-3);
            float x = std::log(roughness);
            return 1.62142f + 0.819955f * x + 0.1734f * x * x +
                   0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
        }
        BeckmannDistribution(float alphax, float alphay, bool samplevis = true)
                : MicrofacetDistribution(samplevis), mAlphax(alphax), mAlphay(alphay) {}
        float D(const Vec3f &wh, const Vec3f& n) const;
        Vec3f Sample_wh(const Vec3f &wo, const Point2f &u, const Vec3f& n) const;

    public:
        virtual void *Clone() {
            return (void *)(new BeckmannDistribution(*this));
        }

    private:
        float Lambda(const Vec3f &w, const Vec3f& n) const;

        const float mAlphax;
        const float mAlphay;
    };

    class TrowbridgeReitzDistribution : public MicrofacetDistribution {
    public:
        static inline float RoughnessToAlpha(float roughness);
        TrowbridgeReitzDistribution(float alphax, float alphay,
                                    bool samplevis = true)
                : MicrofacetDistribution(samplevis), mAlphax(alphax), mAlphay(alphay) {}
        float D(const Vec3f &wh, const Vec3f& n) const;
        Vec3f Sample_wh(const Vec3f &wo, const Point2f &u, const Vec3f& n) const;

    public:
        virtual void *Clone() {
            return (void *)(new TrowbridgeReitzDistribution(*this));
        }

    private:
        float Lambda(const Vec3f &w, const Vec3f& n) const;

        const float mAlphax;
        const float mAlphay;
    };

    inline float TrowbridgeReitzDistribution::RoughnessToAlpha(float roughness) {
        roughness = std::max<float>(roughness, (float)1e-3);
        float x = std::log(roughness);
        return 1.62142f + 0.819955f * x + 0.1734f * x * x + 0.0171201f * x * x * x +
               0.000640711f * x * x * x * x;
    }
*/
    //
    class Fresnel : public ICloneable {
    public:
        virtual ~Fresnel() { }
        virtual Color3f Evaluate(float cosI) const = 0;
    };

    class FresnelConductor : public Fresnel {
    public:
        Color3f Evaluate(float cosThetaI) const;
        FresnelConductor(const Color3f &etaI, const Color3f &etaT, const Color3f &k)
                : mEtaI(etaI), mEtaT(etaT), mK(k) {}

    public:
        virtual void *Clone() {
            return (void *)(new FresnelConductor(*this));
        }

    public:
        static Color3f FrConductor(float cosThetaI, const Color3f &etai, const Color3f &etat, const Color3f &k);

    private:
        Color3f mEtaI;
        Color3f mEtaT;
        Color3f mK;
    };

    class FresnelDielectric : public Fresnel {
    public:
        Color3f Evaluate(float cosThetaI) const;
        FresnelDielectric(float etaI, float etaT) : mEtaI(etaI), mEtaT(etaT) {}

    public:
        virtual void *Clone() {
            return (void *)(new FresnelDielectric(*this));
        }

    public:
        static float FrDielectric(float cosThetaI, float etaI, float etaT);

    private:
        float mEtaI;
        float mEtaT;
    };

    class FresnelNoOp : public Fresnel {
    public:
        Color3f Evaluate(float dummy) const { return WHITE; }

    public:
        virtual void *Clone() {
            return (void *)(new FresnelNoOp(*this));
        }

    };
/*
    //
    class MicrofacetReflection : public BRDF {
    public:
        MicrofacetReflection() : mR(BLACK), mpDistribution(nullptr), mpFresnel(nullptr) { }
        MicrofacetReflection(Color3f const& r, MicrofacetDistribution *distribution, Fresnel *fresnel)
        : mR(r), mpDistribution(distribution), mpFresnel(fresnel) {

        }
        MicrofacetReflection(const MicrofacetReflection& rhs)
        : mR(rhs.mR), mpDistribution(nullptr), mpFresnel(nullptr) {
            if (rhs.mpDistribution != nullptr) {
                mpDistribution = (MicrofacetDistribution *)rhs.mpDistribution->Clone();
            }
            if (rhs.mpFresnel != nullptr) {
                mpFresnel = (Fresnel *)rhs.mpFresnel->Clone();
            }
        }
        MicrofacetReflection& operator=(const MicrofacetReflection& rhs) {
            if (this == &rhs) {
                return *this;
            }

            mR = rhs.mR;
            mpDistribution = nullptr;
            mpFresnel = nullptr;
            if (rhs.mpDistribution != nullptr) {
                mpDistribution = (MicrofacetDistribution *)rhs.mpDistribution->Clone();
            }
            if (rhs.mpFresnel != nullptr) {
                mpFresnel = (Fresnel *)rhs.mpFresnel->Clone();
            }

            return *this;
        }
        virtual ~MicrofacetReflection() {
            if (mpDistribution != nullptr) {
                delete mpDistribution;
                mpDistribution = nullptr;
            }
            if (mpFresnel != nullptr) {
                delete mpFresnel;
                mpFresnel = nullptr;
            }
        }

    public:
        virtual void *Clone() {
            return (void *)(new MicrofacetReflection(*this));
        }

    public:
        virtual Color3f F(const HitRecord& hitRec, const Vec3f& wo, const Vec3f& wi) const;
        virtual Color3f Sample_f_pdf(const HitRecord& hitRec, const Vec3f& wo, Vec3f& wi, float& pdf) const;

    public:
        inline void SetR(const Color3f& r) {
            mR = r;
        }

        inline void SetDistribution(MicrofacetDistribution *distribution) {
            mpDistribution = distribution;
        }

        inline void SetFresnel(Fresnel *fresnel) {
            mpFresnel = fresnel;
        }

    private:
        Color3f mR;
        MicrofacetDistribution *mpDistribution;
        Fresnel *mpFresnel;

    };


    class FresnelBlend : public BRDF {
    public:
        FresnelBlend();
        FresnelBlend(const Spectrum &Rd, const Spectrum &Rs,
                     MicrofacetDistribution *distrib);

    public:
        virtual void *Clone() {
            return (void *)(new FresnelBlend(*this));
        }

    public:
        Spectrum f(const Vector3f &wo, const Vector3f &wi) const;
        Spectrum SchlickFresnel(Float cosTheta) const {
            auto pow5 = [](Float v) { return (v * v) * (v * v) * v; };
            return Rs + pow5(1 - cosTheta) * (Spectrum(1.) - Rs);
        }
        Spectrum Sample_f(const Vector3f &wi, Vector3f *sampled_f, const Point2f &u,
                          Float *pdf, BxDFType *sampledType) const;

    public:


    private:
        const Spectrum Rd, Rs;
        MicrofacetDistribution *distribution;
    };
*/
}

#endif //LAPLATARAYTRACER_REFLECTION_H
