#pragma once

#include "ICloneable.h"
#include "Common.h"
#include "Sampling.h"
#include "Vec3.h"
#include "HitRecord.h"
#include "Surface.h"
#include "Transform.h"
#include "Utility.h"
#include "Texture.h"
#include "Reflection.h"

//
namespace LaplataRayTracer
{
	//
	class BRDF : public ICloneable {
	public:
		BRDF() : mpSampler(nullptr) { }

		BRDF(BRDF const& rhs) {
			mpSampler = nullptr;

			if (rhs.mpSampler != nullptr) {
				mpSampler = (SamplerBase *)rhs.mpSampler->Clone();
			}
		}

		BRDF& operator=(BRDF const& rhs) {
			if (this == &rhs) {
				return *this;
			}

			if (mpSampler) {
				delete mpSampler;
				mpSampler = nullptr;
			}

			if (rhs.mpSampler != nullptr) {
				mpSampler = (SamplerBase *)rhs.mpSampler->Clone();
			}

			return *this;
		}

		virtual ~BRDF() {
			if (mpSampler) {
				delete mpSampler;
				mpSampler = nullptr;
			}
		}

	public:
		virtual void *Clone() {
			return new BRDF(*this);
		}

	public:
		// think twice, I was torn between f() and F()
		virtual Color3f F(const HitRecord& hitRec, const Vec3f& wo, const Vec3f& wi) const {
			return Color3f(0.0f, 0.0f, 0.0f);
		}

		virtual Color3f Sample_f(const HitRecord& hitRec, const Vec3f& wo, Vec3f& wi) const {
			wi.Set(0.0f, 0.0f, 0.0f);
			return Color3f(0.0f, 0.0f, 0.0f);
		}

		virtual Color3f Sample_f_pdf(const HitRecord& hitRec, const Vec3f& wo, Vec3f& wi, float& pdf) const {
		    Vec3f w = hitRec.n;
			//w.MakeUnit();
			Vec3f v = Cross(Vec3f(0.0034f, 1.0f, 0.0071f), w);
			v.MakeUnit();
			Vec3f u = Cross(v, w);

			Vec3f sample_dir = mpSampler->SampleFromHemishpere();
			wi = sample_dir.X() * u + sample_dir.Y() * v + sample_dir.Z() * w;
			wi.MakeUnit();

			float cos_theta = Dot(hitRec.n, wi);
			pdf = cos_theta * INV_PI_CONST;

			return F(hitRec, wo, wi); // like pbrt does.
		}

		virtual Color3f Rho(const HitRecord& hitRec, const Vec3f& wo) const {
			return Color3f(0.0f, 0.0f, 0.0f);
		}

	public:
		inline void SetSampler(SamplerBase *sampler) {
			if (mpSampler != nullptr) {
				delete mpSampler;
				mpSampler = nullptr;
			}

			mpSampler = sampler; // no cares about what if sampler is NULL.
		}

	protected:
		SamplerBase *		mpSampler;	// for indirect illumination, XX and the outer should be response for releasing its memory XX ??.

	};

	//
	class Lambertian : public BRDF {
	public:
		Lambertian() 
			: BRDF(), mKd(0.3f), mCd(Color3f(0.01f, 0.01f, 0.01f)) {

		}

		Lambertian(const Lambertian& rhs) 
			: BRDF(rhs), mKd(rhs.mKd), mCd(rhs.mCd) {

		}

		Lambertian& operator=(const Lambertian& rhs) {
			if (this == &rhs) {
				return *this;
			}

			BRDF::operator=(rhs);
			this->mKd = rhs.mKd;
			this->mCd = rhs.mCd;

			return *this;
		}

		virtual ~Lambertian() {

		}

	public:
		virtual void *Clone() {
			return new Lambertian(*this);
		}

	public:
		virtual Color3f F(const HitRecord& hitRec, const Vec3f& wo, const Vec3f& wi) const {
			return INV_PI_CONST * (mKd * mCd);
		}

		virtual Color3f Sample_f_pdf(const HitRecord& hitRec, const Vec3f& wo, Vec3f& wi, float& pdf) const {
			return BRDF::Sample_f_pdf(hitRec, wo, wi, pdf);
		}

		virtual Color3f Rho(const HitRecord& hitRec, const Vec3f& wo) const {
			return (mKd * mCd);
		}

	public:
		inline void SetKa(float ka) {
			mKd = ka;
		}

		inline void SetKd(float kd) {
			mKd = kd;
		}

		inline void SetCd(Color3f const& cd) {
			mCd = cd;
		}

		inline void SetCd2(float r, float g, float b) {
			mCd.Set(r, g, b);
		}

		inline void SetCd3(float c) {
			mCd.Set(c, c, c);
		}

	private:
		float		mKd;
		Color3f		mCd;

	};

	//
	class TextureLambertian : public BRDF {
	public:
		TextureLambertian()
			: BRDF(), mKd(0.3f), mpCd(nullptr) {

		}

		TextureLambertian(const TextureLambertian& rhs)
			: BRDF(rhs), mKd(rhs.mKd), mpCd(rhs.mpCd) {

		}

		TextureLambertian& operator=(const TextureLambertian& rhs) {
			if (this == &rhs) {
				return *this;
			}

			BRDF::operator=(rhs);
			this->mKd = rhs.mKd;
			this->mpCd = (Texture *)rhs.mpCd->Clone();;

			return *this;
		}

		virtual ~TextureLambertian() {
			if (mpCd) { delete mpCd; mpCd = nullptr; }
		}

	public:
		virtual void *Clone() {
			return new TextureLambertian(*this);
		}

	public:
		virtual Color3f F(const HitRecord& hitRec, const Vec3f& wo, const Vec3f& wi) const {
			return INV_PI_CONST * (mKd * mpCd->GetTextureColor((HitRecord&)hitRec));
		}

		virtual Color3f Sample_f_pdf(const HitRecord& hitRec, const Vec3f& wo, Vec3f& wi, float& pdf) const {
			return BRDF::Sample_f_pdf(hitRec, wo, wi, pdf);
		}

		virtual Color3f Rho(const HitRecord& hitRec, const Vec3f& wo) const {
			return mKd * mpCd->GetTextureColor((HitRecord&)hitRec);
		}

	public:
		inline void SetKa(float ka) {
			mKd = ka;
		}

		inline void SetKd(float kd) {
			mKd = kd;
		}

		inline void SetCd(Texture *tex) {
			mpCd = tex;
		}

	private:
		float		mKd;
		Texture *	mpCd;

	};

	class OrenNayar : public BRDF {
	public:
		OrenNayar(float sigma)
            : BRDF(), mA(0.0f), mB(0.0f) {
                sigma = ANG2RAD(sigma);
                float sigma2 = sigma * sigma;
                mA = 1.0f - (sigma2 / (2.0f * (sigma2 + 0.33f)));
                mB = 0.45f * sigma2 / (sigma2 + 0.09f);
            }
        
		virtual ~OrenNayar() { }

	public:
		virtual void *Clone() {
			return (void *)(new OrenNayar(*this));
		}

	public:
		virtual Color3f F(const HitRecord& hitRec, const Vec3f& wo, const Vec3f& wi) const {
            float cos_delta_phi = (wi.X() * wo.X() + wi.Y() * wo.Y()) /
            std::sqrt(((wi.X() * wi.X()) + (wi.Y() * wi.Y())) * ((wo.X() * wo.X()) + (wo.Y() * wo.Y())));
            cos_delta_phi = RTMath::Clamp(cos_delta_phi, 0.0f, 1.0f);
            
            float D = std::sqrt((1.0 - (wi.Z() * wi.Z())) * (1.0 - (wo.Z() * wo.Z()))) / std::max(wi.Z(), wo.Z());
            
            return (INV_PI_CONST * (mA + mB * cos_delta_phi * D));
		}
        
        virtual Color3f Sample_f_pdf(const HitRecord& hitRec, const Vec3f& wo, Vec3f& wi, float& pdf) const {
            // 1.get wi reflectance direction
            Point2f uv = SamplerBase::URand2();
            wi = SamplerBase::WeightedSampleHemishpere(uv);
            if (wo.Z() < 0.0f) {
                wi[2] *= -1.0f;
            }
            
            // 2.get pdf of generated direction
            pdf = (std::abs(wi.Z()) * INV_PI_CONST);
            
            // 3.get reflectance albedo
            return this->F(hitRec, wo, wi);
        }

	private:
        float mA, mB;
	};

	//
	class PhongGlossySpecular : public BRDF {
	public:
		PhongGlossySpecular()
			: BRDF(), mKs(1.0f), mCs(Color3f(1.0f, 1.0f, 1.0f)), mEXP(200.0f)
		{

		}

		PhongGlossySpecular(const PhongGlossySpecular& rhs)
			: BRDF(rhs), mKs(rhs.mKs), mCs(rhs.mCs), mEXP(rhs.mEXP) {

		}

		PhongGlossySpecular& operator=(const PhongGlossySpecular& rhs) {
			if (this == &rhs) {
				return *this;
			}

			BRDF::operator=(rhs);
			this->mKs = rhs.mKs;
			this->mCs = rhs.mCs;
			this->mEXP = rhs.mEXP;

			return *this;
		}

		virtual ~PhongGlossySpecular() {

		}

	public:
		virtual void *Clone() {
			return (new PhongGlossySpecular(*this));
		}

	public:
		virtual Color3f F(const HitRecord& hitRec, const Vec3f& wo, const Vec3f& wi) const {
			Color3f 	L;
			Vec3f 		r = Surface::Reflect(-wi, hitRec.n);
			float 		rdotwo = Dot(r, wo);

			if (rdotwo > 0.0f) {
				L = (mKs * std::pow(rdotwo, mEXP)) * mCs;
			}

			return L;
		}

		virtual Color3f Sample_f(const HitRecord& hitRec, const Vec3f& wo, Vec3f& wi) const {
			return BLACK;
		}

		virtual Color3f Sample_f_pdf(const HitRecord& hitRec, const Vec3f& wo, Vec3f& wi, float& pdf) const {
			// 1. Get the reflect direction vector - r
			Vec3f r = Surface::Reflect(-wo, hitRec.n);

			// 2. Set up the normal plane bias along the r direction
			UVM		uvw;
			uvw.BuildFromW(r, Vec3f(0.00424f, 1.0f, 0.00764f));

			// 3. Sample one random point upon the hemisphere
			Vec3f sample_point = mpSampler->SampleFromHemishpere();

			// 4. Convert the random point in the normal plane bias to get its world direction vector
			wi = uvw.World(sample_point);
			float ndotwi = Dot(hitRec.n, wi);
			if (ndotwi < 0.0f) {
				wi = -sample_point.X() * uvw.U() - sample_point.Y() * uvw.V() + sample_point.Z() * uvw.W();
			}

			// 5. Get the pdf value
			float rdotwi = Dot(r, wi);
			float phong_lobe = std::pow(rdotwi, mEXP);
			pdf = phong_lobe * ndotwi;

			// 6. Return the final color for indrect illumination
			return mKs * mCs; // * phong_lobe;
		}

	public:
		inline void SetKs(float ks) {
			mKs = ks;
		}

		inline void SetCs(const Color3f& cs) {
			mCs = cs;
		}

		inline void SetCs2(float r, float g, float b) {
			mCs.Set(r, g, b);
		}

		inline void SetCs3(float c) {
			mCs.Set(c, c, c);
		}

		inline void SetEXP(float exp) {
			mEXP = exp;
		}

	protected:
		float		mKs;
		Color3f		mCs;
		float		mEXP;

	};

	//
	class BlinnPhongGlossySpecular : public PhongGlossySpecular {
	public:
		BlinnPhongGlossySpecular()
			: PhongGlossySpecular() {

		}

		BlinnPhongGlossySpecular(const BlinnPhongGlossySpecular& rhs)
			: PhongGlossySpecular(rhs) {

		}

		BlinnPhongGlossySpecular& operator=(const BlinnPhongGlossySpecular& rhs) {
			if (this == &rhs) {
				return *this;
			}

			PhongGlossySpecular::operator=(rhs);

			return *this;
		}

		virtual ~BlinnPhongGlossySpecular() {

		}

	public:
		virtual void *Clone() {
			return (new BlinnPhongGlossySpecular(*this));
		}

	public:
		virtual Color3f F(const HitRecord& hitRec, const Vec3f& wo, const Vec3f& wi) const {
			Color3f		L;
			Vec3f		h = (wi + wo);
			h.MakeUnit();
			float		ndoth = Dot(hitRec.n, h);

			if (ndoth > 0.0f) {
				L = mKs * std::pow(ndoth, mEXP) * mCs;
			}

			return L;
		}

	};

	//
	class PerfectSpecular : public BRDF {
	public:
		PerfectSpecular()
			: BRDF(), mKr(1.0f), mCr(WHITE) {

		}

		PerfectSpecular(PerfectSpecular const& rhs)
			: BRDF(rhs), mKr(rhs.mKr), mCr(rhs.mCr) {

		}

		PerfectSpecular& operator=(PerfectSpecular const& rhs) {
			if (this == &rhs) {
				return *this;
			}

			BRDF::operator=(rhs);
			this->mKr = rhs.mKr;
			this->mCr = rhs.mCr;

			return *this;
		}

		virtual ~PerfectSpecular() {

		}

	public:
		virtual void *Clone() {
			return (new PerfectSpecular(*this));
		}

	public:
		virtual Color3f Sample_f(const HitRecord& hitRec, const Vec3f& wo, Vec3f& wi) const {
			Color3f L;

		//	float ndotwo = Dot(hitRec.n, wo);
			wi = Surface::Reflect(-wo, hitRec.n);
			// This term is only used for fitting the light transport equation, L = F(r)*L(i)*cos(theta), 
			// where cos(theta) is equal to ndotwo
			L = mKr * mCr; /* / ndotwo */

			return L;
		}

		virtual Color3f Sample_f_pdf(const HitRecord& hitRec, const Vec3f& wo, Vec3f& wi, float& pdf) const {
			Color3f L = Sample_f(hitRec, wo, wi);

			pdf = std::fabs(Dot(hitRec.n, wi));

			return L;
		}

	public:
		inline void SetKr(float kr) {
			mKr = kr;
		}

		inline void SetCr(Color3f const& cr) {
			mCr = cr;
		}

		inline void SetCr2(float r, float g, float b) {
			mCr.Set(r, g, b);
		}

		inline void SetCr3(float c) {
			mCr.Set(c, c, c);
		}

	private:
		float mKr;
		Color3f mCr;

	};

	//
	class FresnelReflective : public BRDF {
	public:
		FresnelReflective()
			: BRDF(), mEtaIn(1.5f), mEtaOut(1.0f) {

		}

		FresnelReflective(float etaIn, float etaOut)
			: BRDF(), mEtaIn(etaIn), mEtaOut(etaOut) {

		}

		FresnelReflective(const FresnelReflective& rhs)
			:BRDF(rhs), mEtaIn(rhs.mEtaIn), mEtaOut(rhs.mEtaOut) {

		}

		FresnelReflective& operator=(const FresnelReflective& rhs) {
			if (this == &rhs) {
				return *this;
			}

			BRDF::operator=(rhs);
			this->mEtaIn = rhs.mEtaIn;
			this->mEtaOut = rhs.mEtaOut;

			return *this;
		}

		virtual ~FresnelReflective() {

		}

	public:
		virtual void *Clone() { return (void *)(new FresnelReflective(*this)); }

	public:
		virtual Color3f Sample_f(const HitRecord& hitRec, const Vec3f& wo, Vec3f& wi) const {
			Color3f L;
			wi = Surface::Reflect(-wo, hitRec.n);
			float kr = Surface::FresnelDielectric(hitRec.n, wo, mEtaIn, mEtaOut);
			L = kr * WHITE / std::fabs(Dot(hitRec.n, wi));
			return L;
		}

	public:
		inline void SetEtaIn(float etaIn) { mEtaIn = etaIn; }
		inline void SetEtaOut(float etaOut) { mEtaOut = etaOut; }

	private:
		// eta = in / out
		float mEtaIn;
		float mEtaOut;

	};

	// Transparent
	class TransparentSpecular : public BRDF {
	public:
		TransparentSpecular()
		: BRDF(), mRefIdx(1.5f), mCTIn(WHITE), mCTOut(WHITE), mReflect(true) {

		}

		TransparentSpecular(float refIdx, const Color3f& ctin, const Color3f& ctout, bool reflected = true)
		: BRDF(), mRefIdx(refIdx), mCTIn(ctin), mCTOut(ctout), mReflect(reflected) {

		}

		virtual ~TransparentSpecular() {

		}

	public:
		virtual void *Clone() {
			return (void *)(new TransparentSpecular(*this));
		}

	public:
		virtual Color3f Sample_f_pdf(const HitRecord& hitRec, const Vec3f& wo, Vec3f& wi, float& pdf) const {
			Vec3f outfaced_normal;
			Vec3f dir = -wo;
			Vec3f reflected = Surface::Reflect(dir, hitRec.n);
			float ni_over_nt;
			Vec3f refracted;
			float reflected_prob;
			float consine;
			Color3f alebdo;
			if (Dot(dir, hitRec.n) > 0.0f)
			{
				outfaced_normal = -hitRec.n;
				ni_over_nt = mRefIdx;
				consine = mRefIdx * Dot(dir, hitRec.n) / dir.Length();
			}
			else
			{
				outfaced_normal = hitRec.n;
				ni_over_nt = 1.0f / mRefIdx;
				consine = -Dot(dir, hitRec.n) / dir.Length();
			}

			if (Surface::Refract(dir, outfaced_normal, ni_over_nt, refracted))
			{
				reflected_prob = Surface::Schlick(consine, mRefIdx);
			}
			else
			{
				reflected_prob = 1.0f;
			}
		
			if (mReflect || reflected_prob >= 1.0f)
			{
				float prob_value = Random::frand48();
				if (prob_value < reflected_prob)
				{
					wi = reflected;
					alebdo = mCTOut;
				}
				else
				{
					wi = refracted;
					alebdo = mCTIn;
				}

				pdf = prob_value;
			}
			else
			{
				wi = refracted;
				alebdo = mCTIn;

				pdf = 1.0f;
			}

			return alebdo;
		}

	public:
		inline void SetRefIdx(float refIdx) {
			mRefIdx = refIdx;
		}

		inline void SetTransparentColor(const Color3f& ctin, const Color3f& ctout) {
			mCTIn = ctin;
			mCTOut = ctout;
		}

		inline void EnableReflect(bool reflect) {
			mReflect = reflect;
		}

	private:
		float mRefIdx;
		Color3f mCTIn;
		Color3f mCTOut;
		bool mReflect;

	};

	// MicrofacetSpecular
	class MicrofacetSpecular : public BRDF {
	public:
		MicrofacetSpecular()
			: mK(0.001f), mF(1.0f), mRoughness(0.25f), mReflection(0.9f, 0.9f, 0.9f) {
		//	mpFresnel = new FresnelConductor(Color3f(1.0, 1.0, 1.0), Color3f(0.200438, 0.924033, 1.102212), Color3f(3.912949, 2.452848, 2.142188));
		//	mpFresnel = new FresnelDielectric(1.5, 1.0);
		}

		MicrofacetSpecular(float k, float f, float roughness, Color3f const& reflection)
			: mK(k), mF(f), mRoughness(roughness), mReflection(reflection) {
		//	mpFresnel = new FresnelConductor(Color3f(1.0, 1.0, 1.0), Color3f(0.200438, 0.924033, 1.102212), Color3f(3.912949, 2.452848, 2.142188));
		//	mpFresnel = new FresnelDielectric(1.5, 1.0);
		}

		virtual ~MicrofacetSpecular() {

		}

	public:
		virtual void *Clone() {
			return (void *)(new MicrofacetSpecular(*this));
		}

	public:
		virtual Color3f F(const HitRecord& hitRec, const Vec3f& wo, const Vec3f& wi) const {
			Color3f L;

			// it's a suffering.
			// vector h: half vector between v(wo) and l(wi)
			// vector v: view vector(wo)
			// vector l: light vector(wi)
			// vector n: normal vector at hit point on the surface
			Vec3f v = wo;
			Vec3f l = wi;
			Vec3f n = hitRec.n;
			v.MakeUnit();
			l.MakeUnit();
			Vec3f h = (wo + wi);
			h.MakeUnit();

			float hdotv = Dot(h, v);
			float ndoth = Dot(n, h);
			float ndotv = Dot(n, v);
			float ndotl = Dot(n, l);
			float ldoth = Dot(l, h);
			float square_roughness = mRoughness * mRoughness;

			float calc_F = mF + (1.0f - mF) * std::pow(1.0f - hdotv, 5.0f);
			//Color3f calc_F = mpFresnel->Evaluate(hdotv);
			//Color3f calc_F = mpFresnel->Evaluate(hdotv);
			
			float exp = (ndoth * ndoth - 1.0f) / (square_roughness * (ndoth * ndoth));
			float calc_D = std::exp(exp) / (4.0f * square_roughness * std::pow(ndoth, 4.0f));

			float calc_Gb = (2.0f * ndoth * ndotv) / hdotv;
			float calc_Gc = (2.0f * ndoth * ndotl) / hdotv;
			float calc_G = std::min<float>(1.0f, std::min<float>(calc_Gb, calc_Gc));

			float rs = (calc_F * calc_D * calc_G) / (PI_CONST * ndotl * ndotv);
		//	float rs = (calc_D * calc_G) / (PI_CONST * ndotl * ndotv);
			L = (mK + (1.0f - mK) * rs);
		//	L = calc_F * rs;

			return L;
		}

		virtual Color3f Sample_f_pdf(const HitRecord& hitRec, const Vec3f& wo, Vec3f& wi, float& pdf) const {
			Vec3f r = Surface::Reflect(-wo, hitRec.n);

			UVM		uvw;
			uvw.BuildFromW(r, Vec3f(0.00424f, 1.0f, 0.00764f));

			Vec3f sample_point = mpSampler->SampleFromHemishpere();

			wi = uvw.World(sample_point);
			float ndotwi = Dot(hitRec.n, wi);
			if (ndotwi < 0.0f) {
				wi = -sample_point.X() * uvw.U() - sample_point.Y() * uvw.V() + sample_point.Z() * uvw.W();
			}

			//pdf = ndotwi / PI_CONST;
			pdf = 1.0f;

			return mReflection;
		}

	public:
		inline void SetK(float k) {
			mK = k;
		}

		inline void SetF(float f) {
			mF = f;
		}

		inline void SetRoughness(float roughness) {
			mRoughness = roughness;
		}

		inline void SetReflection(const Color3f& reflection) {
			mReflection = reflection;
		}

	private:
		float mK;
		float mF;
		float mRoughness;
		Color3f mReflection;
		Fresnel *mpFresnel;

	};

	//
	class FresnelBlend : public BRDF {
	public:
		FresnelBlend(const Color3f& rd, const Color3f& rs, float roughness, float factor)
			: BRDF(), mRd(rd), mRs(rs), mRoughness(roughness), mFactor(factor) {
			mpDiffuse = nullptr;
		}

		FresnelBlend(FresnelBlend const& rhs)
			: BRDF(rhs), mRd(rhs.mRd), mRs(rhs.mRs), mRoughness(rhs.mRoughness), mFactor(rhs.mFactor)
		{
			mpDiffuse = nullptr;
			if (rhs.mpDiffuse != nullptr) {
				mpDiffuse = (Texture *)(rhs.mpDiffuse->Clone());
			}
		}

		FresnelBlend& operator=(FresnelBlend const& rhs) {
			if (this == &rhs) {
				return *this;
			}

			BRDF::operator=(rhs);
			mRd = rhs.mRd;
			mRs = rhs.mRs;
			mRoughness = rhs.mRoughness;
			mFactor = rhs.mFactor;
			if (rhs.mpDiffuse != nullptr) {
				mpDiffuse = (Texture *)(rhs.mpDiffuse->Clone());
			}
			else {
				mpDiffuse = nullptr;
			}

			return *this;
		}

		virtual ~FresnelBlend() {
			if (mpDiffuse != nullptr) {
				delete mpDiffuse;
				mpDiffuse = nullptr;
			}
		}

	public:
		virtual void *Clone() {
			return (void *)(new FresnelBlend(*this));
		}

	public:
		virtual Color3f F(const HitRecord& hitRec, const Vec3f& wo, const Vec3f& wi) const {
			Color3f diffuse = (28.f / (23.f*PI_CONST)) * mRd *
				(WHITE - mRs) *
				(1.0f - powf(1.0f - 0.5f * std::abs(Dot(wi, hitRec.n)), 5)) *
				(1.0f - powf(1.0f - 0.5f * std::abs(Dot(wo, hitRec.n)), 5));
			Vec3f wh = wi + wo;
			if (wh.X() == 0. && wh.Y() == 0. && wh.Z() == 0.) return BLACK;
			wh.MakeUnit();

			Vec3f n = hitRec.n;
			Vec3f h = (wo + wi);
			h.MakeUnit();
		
			float ndoth = Dot(n, h);
			float square_roughness = mRoughness * mRoughness;

			float exp = (ndoth * ndoth - 1.0f) / (square_roughness * (ndoth * ndoth));
			float calc_D = std::exp(exp) / (4.0f * square_roughness * std::pow(ndoth, 4.0f));

			Color3f specular = calc_D /
				(4.0f * std::abs(Dot(wi, wh)) * std::max<float>(std::abs(Dot(wo, hitRec.n)), std::abs(Dot(wi, hitRec.n)))) *
				Surface::SchlickFresnel(mRs, Dot(wi, wh));

			// get shading result
			Color3f tex_diff = WHITE;
			if (mpDiffuse != nullptr) {
				tex_diff = mpDiffuse->GetTextureColor((HitRecord&)hitRec);
			}
			return tex_diff * diffuse + specular;
		}

		virtual Color3f Sample_f_pdf(const HitRecord& hitRec, const Vec3f& wo, Vec3f& wi, float& pdf) const {
			Vec3f r = Surface::Reflect(-wo, hitRec.n);

			UVM		uvw;
			uvw.BuildFromW(r, Vec3f(0.00424f, 1.0f, 0.00764f));

			Vec3f sample_point = mpSampler->SampleFromHemishpere();

			wi = uvw.World(sample_point);
			float ndotwi = Dot(hitRec.n, wi);
			if (ndotwi < 0.0f) {
				wi = -sample_point.X() * uvw.U() - sample_point.Y() * uvw.V() + sample_point.Z() * uvw.W();
			}

			//pdf = Dot(hitRec.n, wi) / PI_CONST;
			pdf = 1.0f;

			return mRd * mFactor;
		}

	public:
		inline void SetDiffuseEx(Texture *tex) {
			if (mpDiffuse != nullptr) {
				delete mpDiffuse;
			}
			mpDiffuse = tex;
		}

	private:
		Color3f mRd;
		Color3f mRs;
		float mRoughness;
		float mFactor;
		Texture *mpDiffuse;

	};

	//
	class MicrofacetSmithGGX : public BRDF {
	public:
		MicrofacetSmithGGX()
			: BRDF(), mEtaT(1.5f), mEtaI(1.0f), mRoughness(0.25f), mReflection(Color3f(0.9f, 0.9f, 0.9f)) {

		}

		MicrofacetSmithGGX(float etaT, float etaI, float roughness, const Color3f& reflection)
			: BRDF(), mEtaT(etaT), mEtaI(etaI), mRoughness(roughness), mReflection(reflection) {

		}

		virtual ~MicrofacetSmithGGX() {

		}

	public:
		virtual void *Clone() {
			return (void *)(new MicrofacetSmithGGX(*this));
		}

	public:
		virtual Color3f F(const HitRecord& hitRec, const Vec3f& wo, const Vec3f& wi) const {
			Vec3f wm = (wo + wi);
			wm.MakeUnit();

			float F = fresnel(wi, wm);
			float D = GGX_D(wm, hitRec.n);
			float G = Smith_G(wo, wi, hitRec.n, wm);

			float widotn = std::abs(Dot(wi, hitRec.n));
			float wodotn = std::abs(Dot(wo, hitRec.n));

			float fr = (F * D * G) / (4.0f * widotn * wodotn);
			return (mReflection * fr);
		}

		virtual Color3f Sample_f_pdf(const HitRecord& hitRec, const Vec3f& wo, Vec3f& wi, float& pdf) const {
			Vec3f outfaced_normal;
			Vec3f dir = -wo;
			float ni_over_nt;
			Vec3f refracted;

			float ref_idx = mEtaI / mEtaT;
			if (Dot(dir, hitRec.n) > 0.0f)
			{
				outfaced_normal = -hitRec.n;
				ni_over_nt = ref_idx;
			}
			else
			{
				outfaced_normal = hitRec.n;
				ni_over_nt = 1.0f / ref_idx;
			}
			pdf = 1.0f;
			if (Surface::Refract(dir, outfaced_normal, ni_over_nt, refracted))
			{
				wi = refracted;
				//	wi.MakeUnit();

				Vec3f ht = -(mEtaT * wo + mEtaI * wi);
				ht.MakeUnit();

				float wodotht = std::abs(Dot(wo, ht));
				float widotht = std::abs(Dot(wi, ht));
				float wodotn = std::abs(Dot(wo, hitRec.n));
				float widotn = std::abs(Dot(wi, hitRec.n));

				float ft1 = (wodotht * widotht) / (wodotn * widotn);

				float F = fresnel(wo, ht);
				float D = GGX_D(ht, hitRec.n);
				float G = Smith_G(wo, wi, hitRec.n, ht);

				float ft2 = mEtaT * Dot(wo, ht) + mEtaI * (Dot(wi, ht));
				ft2 *= ft2;

				float ft3 = ((mEtaI * mEtaI) * (1.0f - F) * G * D);

				float ft = (ft1 * ft3) / ft2;

				//	return (mReflection * ft);
				//	return WHITE * ft;
				//	return Color3f(1, 0, 0);
				return WHITE;
			}

			return BLACK;
		
		}

	private:
		inline float fresnel(const Vec3f& wi, const Vec3f& n) const {
			float c = std::abs(Dot(wi, n));
			float g = std::sqrt((mEtaT*mEtaT) / (mEtaI*mEtaI) - 1.0f + c*c);
			float g_plus_c = g + c;
			float g_minus_c = g - c;
			float f1 = 0.5f * ((g_minus_c * g_minus_c) / (g_plus_c * g_plus_c));
			float f2 = 1.0f + ((c * g_plus_c - 1.0f) * (c * g_plus_c - 1.0f)) 
				/ ((c * g_minus_c - 1.0f) * (c * g_minus_c - 1.0f));

			return (f1 * f2);
		}

		inline float GGX_D(const Vec3f& wm, const Vec3f& n) const {
			float cos_theta = Dot(wm, n);
			if (cos_theta <= 0.0f) {
				return 0.0f;
			}
			float sin_theta = std::sqrt(1.0f - cos_theta * cos_theta);
			float tan_theta = sin_theta / cos_theta;

			float tan_theta2 = tan_theta * tan_theta;
			float cos_theta2 = cos_theta * cos_theta;

			float D = (mRoughness * mRoughness) 
				/ ((PI_CONST * cos_theta2 * cos_theta2) * ((mRoughness * mRoughness + tan_theta2) * (mRoughness * mRoughness + tan_theta2)));

//			float root = mRoughness / (cos_theta2 * (mRoughness * mRoughness + tan_theta2));

//			return (INV_PI_CONST * (root * root));
			return D;
		}

		inline float Smith_G(const Vec3f& wo, const Vec3f& wi, const Vec3f& n, const Vec3f& wm) const {
			float g1 = Smith_G1(wo, wm, n);
			float g2 = Smith_G1(wi, wm, n);
			return (g1 * g2);
		}

		inline float Smith_G1(const Vec3f& v, const Vec3f& wm, const Vec3f& n) const {
			float cos_theta = Dot(v, n);
			float sin_theta = std::sqrt(1.0f - cos_theta * cos_theta);
			float tan_theta = sin_theta / cos_theta;

			if (RTMath::IsZero(tan_theta)) {
				return 1.0f;
			}

			if (Dot(v, wm) * cos_theta <= 0.0f) {
				return 0.0f;
			}

			float root = mRoughness  * tan_theta;
			return (2.0f / (1.0f + std::sqrt(1.0f + root*root)));
		}

	public:
		inline void SetEtaT(float etaT) {
			mEtaT = etaT;
		}

		inline void SetEtaI(float etaI) {
			mEtaI = etaI;
		}

		inline void SetRoughness(float roughness) {
			mRoughness = roughness;
		}

		inline void SetReflection(const Color3f& reflection) {
			mReflection = reflection;
		}

	private:
		float mEtaT; // eta in
		float mEtaI; // eta out
		float mRoughness;
		Color3f mReflection;

	};
}
