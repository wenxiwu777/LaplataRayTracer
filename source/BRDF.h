#pragma once

#include "ICloneable.h"
#include "Common.h"
#include "Sampling.h"
#include "Vec3.h"
#include "HitRecord.h"
#include "Surface.h"
#include "Transform.h"

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
			wi.Set(0.0f, 0.0f, 0.0f);
			pdf = 0.0f;
			return Color3f(0.0f, 0.0f, 0.0f);
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
			: BRDF(), mKd(0.0f), mCd(Color3f(0.0f, 0.0f, 0.0f)) {

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
			: BRDF(), mKd(0.0f), mpCd(nullptr) {

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

	//
	class PhongGlossySpecular : public BRDF {
	public:
		PhongGlossySpecular()
			: BRDF(), mKs(0.0f), mCs(Color3f(0.0f, 0.0f, 0.0f)), mEXP(0.0f)
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
		//	float rdotwi = Dot(r, wi);
		//	float phong_lobe = std::pow(rdotwi, mEXP);
		//	pdf = phong_lobe * ndotwi;

			// 6. Return the final color for indrect illumination
			return mKs * mCs; // * phong_lobe
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
			: BRDF(), mKr(0.0f), mCr(BLACK) {

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
}
