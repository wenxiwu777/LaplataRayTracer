#pragma once

#include "Vec3.h"
#include "HitRecord.h"
#include "Ray.h"
#include "Random.h"
#include "Texture.h"
#include "BRDF.h"
#include "BTDF.h"
#include "PDF.h"
#include "Reflection.h"
#include "WorldObjects.h"
#include "Light.h"
#include "ICloneable.h"
#include "Sampling.h"
#include "Surface.h"
#include "Transform.h"

namespace LaplataRayTracer
{
	//
	class Material : public ICloneable
	{
	public:
		Material() { }
		virtual ~Material() { }

	public:
		virtual void *Clone() = 0;

	public:
		virtual Color3f Emissive(Ray const& inRay, HitRecord& hitRec) = 0;
		virtual Color3f Shade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) = 0;
		virtual Color3f AreaLightShade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) = 0;
		virtual bool AreaLightShade(const Ray& inRay, HitRecord& hitRec, Vec3f& wi, Color3f& CR, float& pdf) = 0;
		virtual bool WhittedShade(const Ray& inRay, HitRecord& hitRec, Vec3f& wi, Color3f& CR) = 0;
		virtual bool WhittedShade(const Ray& inRay, HitRecord& hitRec, WhittedRecord& whittedRec) = 0;
		virtual bool GlobalShade(const Ray& inRay, HitRecord& hitRec, GlobalPathRecord& globalRec) = 0;
		virtual bool PathShade(Ray const& inRay, HitRecord& hitRec, Color3f& attenunation_albedo, Ray& out_ray) = 0;
		virtual float PathShade2_pdf(Ray const& inRay, const HitRecord& hitRec, const Ray& outRay) = 0;
		virtual bool PathShade2(Ray const& inRay, HitRecord& hitRec, ScatterRecord& scatterRec) = 0;

	};

	//
	class MaterialBase : public Material
	{
	public:
		MaterialBase() { mSelfShadow = true; }
		virtual ~MaterialBase() { }

	public:
		virtual void *Clone() {
			return (new MaterialBase);
		}

	public:
		virtual Color3f Emissive(Ray const& inRay, HitRecord& hitRec) { return Color3f(0.0f, 0.0f, 0.0f); }

		virtual Color3f Shade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) { return BLACK; }

		virtual Color3f AreaLightShade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights)
		{
			return BLACK;
		}

		virtual bool AreaLightShade(const Ray& inRay, HitRecord& hitRec, Vec3f& wi, Color3f& CR, float& pdf)
		{
			return false;
		}

		virtual bool WhittedShade(const Ray& inRay, HitRecord& hitRec, Vec3f& wi, Color3f& CR)
		{
			return false;
		}

		virtual bool WhittedShade(const Ray& inRay, HitRecord& hitRec, WhittedRecord& whittedRec)
		{
			return false;
		}

		virtual bool GlobalShade(const Ray& inRay, HitRecord& hitRec, GlobalPathRecord& globalRec)
		{
			return false;
		}

		virtual bool PathShade(Ray const& inRay, HitRecord& hitRec, Color3f& attenunation_albedo, Ray& out_ray)
		{
			return false;
		}

		float PathShade2_pdf(Ray const& inRay, const HitRecord& hitRec, const Ray& outRay)
		{
			return 0.0f;
		}

		bool PathShade2(Ray const& inRay, HitRecord& hitRec, ScatterRecord& scatterRec)
		{
			return false;
		}

	public:
	    virtual void EnableSelfShadow(bool selfShadow) {
            mSelfShadow = selfShadow;
		}

    protected:
	    bool mSelfShadow;

	};

	//
	class NormalMaterial : public MaterialBase
	{
	public:
		NormalMaterial() { }
		virtual ~NormalMaterial() { }

	public:
		virtual void *Clone() { return (NormalMaterial *)(new NormalMaterial); }

	public:
	//	virtual bool PathShade(Ray const& inRay, HitRecord& hitRec, Color3f& attenunation_albedo, Ray& out_ray)
	//	{
	//		attenunation_albedo.Set(0.5f*(hitRec.n.X() + 1.0f), 0.5f*(hitRec.n.Y() + 1.0f), 0.5f*(hitRec.n.Z() + 1.0f));
	//		return true;
	//	}
		virtual Color3f Shade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights)
		{
			return Emissive(inRay, hitRec);
		}

		virtual Color3f Emissive(Ray const& inRay, HitRecord& hitRec)
		{
			Color3f col(0.5f*(hitRec.n.X() + 1.0f), 0.5f*(hitRec.n.Y() + 1.0f), 0.5f*(hitRec.n.Z() + 1.0f));
			return col;
		}

	};

	//
	class MatteMaterial : public MaterialBase
	{
	public:
		MatteMaterial() {
            mpAlbedo = nullptr;
            mpCosinePDF = new CosinePDF;
            mpOrenNayarPDF = new OrenNayarPDF;
            mSigma = 0.0f; // 0.0 means OrenNayar is disabled by default
        }
		MatteMaterial(Color3f const& albedo)
		{
			mpAlbedo = new ConstantTexture(albedo);
			mpCosinePDF = new CosinePDF;
            mpOrenNayarPDF = new OrenNayarPDF;
		}
		MatteMaterial(Texture *albedo) : mpAlbedo(albedo) {
            mpCosinePDF = new CosinePDF;
            mpOrenNayarPDF = new OrenNayarPDF;
        }
		virtual ~MatteMaterial()
		{
			release();
		}

	public:
		virtual void *Clone() { return (MatteMaterial *)(new MatteMaterial((Texture *)mpAlbedo->Clone())); }

	public:
		virtual bool PathShade(Ray const& inRay, HitRecord& hitRec, Color3f& attenunation_albedo, Ray& out_ray)
		{
            if (mSigma > 0.0f) {
                attenunation_albedo = mpAlbedo->GetTextureColor(hitRec);
                OrenNayar orenNayarDiffuse(mSigma);
                CoordinateSystem cs(hitRec.n);
                Vec3f swi;
                Vec3f swo = cs.To(-inRay.D());
                float pdf;
                attenunation_albedo *= orenNayarDiffuse.Sample_f_pdf(hitRec, swo, swi, pdf);
                attenunation_albedo /= pdf;
                Vec3f wi = cs.From(swi);
                out_ray.Set(hitRec.wpt, wi, inRay.T());
            } else {
                Vec3f sample = SamplerBase::SampleInUnitSphere();
            //    Vec3f ptInUnitShpereCenter = hitRec.wpt + hitRec.n + sample;
                Vec3f ptInUnitShpereCenter = hitRec.n + sample;
                ptInUnitShpereCenter.MakeUnit();
            //    out_ray.Set(hitRec.wpt, ptInUnitShpereCenter - hitRec.wpt, inRay.T());
                out_ray.Set(hitRec.wpt, ptInUnitShpereCenter, inRay.T());
                attenunation_albedo = mpAlbedo->GetTextureColor(hitRec);
            }

			return true;

		//	return (Dot(out_ray.D(), hitRec.n) > 0.0f);
		}

		virtual float PathShade2_pdf(Ray const& inRay, const HitRecord& hitRec, const Ray& outRay)
		{
            Vec3f norm_scatter = outRay.D();
			norm_scatter.MakeUnit();
			float cosine_ = Dot(hitRec.n, norm_scatter);
            if (cosine_ < 0.0f)
			{
				return 0.0f;
			}
			return cosine_ / PI_CONST;
		}

		virtual bool PathShade2(Ray const& inRay, HitRecord& hitRec, ScatterRecord& scatterRec)
		{
			scatterRec.is_specular = false;
            if (mSigma > 0.0f) {
                scatterRec.albedo = mpAlbedo->GetTextureColor(hitRec);
                OrenNayar orenNayarDiffuse(mSigma);
                CoordinateSystem cs(hitRec.n);
                Vec3f swi;
                Vec3f swo = cs.To(-inRay.D());
                float pdf;
                scatterRec.albedo *= orenNayarDiffuse.Sample_f_pdf(hitRec, swo, swi, pdf);
                Vec3f wi = cs.From(swi);
                mpOrenNayarPDF->SetReturnedPDF(pdf);
                mpOrenNayarPDF->SetReturnedDirection(wi);
                scatterRec.pPDF = mpOrenNayarPDF;
            } else {
                scatterRec.albedo = mpAlbedo->GetTextureColor(hitRec);
                mpCosinePDF->SetW(hitRec.n);
                scatterRec.pPDF = mpCosinePDF;
            }
			return true;
		}

	public:
		inline void Set_cd(Texture *albedo)
		{
			if (mpAlbedo) {
				release();
			}

			mpAlbedo = albedo;
		}
		
		inline void Set_cd(Color3f const& albedo)
		{
//			if (mpAlbedo) {
//				release();
//			}

//			mpAlbedo = new ConstantTexture(albedo);

			if (mpAlbedo != nullptr)
			{
				((ConstantTexture *)mpAlbedo)->Set(albedo);
			}
			else
			{
				mpAlbedo = new ConstantTexture(albedo);
			}
		}
        
        inline void Set_sigma(float sigma) {
            mSigma = sigma;
        }

	private:
		inline void release()
		{
			if (mpAlbedo)
			{
				delete mpAlbedo;
				mpAlbedo = nullptr;
			}

			if (mpCosinePDF)
			{
				delete mpCosinePDF;
				mpCosinePDF = nullptr;
			}
            
            if (mpOrenNayarPDF)
            {
                delete mpOrenNayarPDF;
                mpOrenNayarPDF = nullptr;
            }
		}

	private:
		MatteMaterial(MatteMaterial const&) { }
        MatteMaterial& operator=(MatteMaterial const&);

	private:
	//	Color3f mAlbedo;
		Texture *mpAlbedo;
		CosinePDF *mpCosinePDF;
        OrenNayarPDF *mpOrenNayarPDF;
        float mSigma; // For OrenNayar brdf

	};

	//
	class MirrorMaterial : public MaterialBase
	{
	public:
		MirrorMaterial() { }
		MirrorMaterial(Color3f const& albedo, float fuzz = 0.0f) : mAlbedo(albedo), mFuzz(fuzz < 1.0f ? fuzz : 1.0f) { }
		virtual ~MirrorMaterial() { }

	public:
		virtual void *Clone() { return (MirrorMaterial *)(new MirrorMaterial(mAlbedo, mFuzz)); }

	public:
		virtual bool PathShade(Ray const& inRay, HitRecord& hitRec, Color3f& attenunation_albedo, Ray& out_ray)
		{
			Vec3f r = Surface::Reflect(MakeUnit<float>(inRay.D()), hitRec.n);
			out_ray.Set(hitRec.wpt, r + mFuzz * SamplerBase::SampleInUnitSphere(), inRay.T());
			attenunation_albedo = mAlbedo;
			return (Dot(r, hitRec.n) > 0.0f);
		}

		virtual bool PathShade2(Ray const& inRay, HitRecord& hitRec, ScatterRecord& scatterRec)
		{
			Vec3f norm_dir = inRay.D();
			norm_dir.MakeUnit();
			Vec3f r = Surface::Reflect(norm_dir, hitRec.n);
			scatterRec.is_specular = true;
			scatterRec.specular_ray.Set(hitRec.wpt, r + mFuzz * SamplerBase::SampleInUnitSphere(), inRay.T());
			scatterRec.albedo = mAlbedo;
			scatterRec.pPDF = nullptr;
			return true;
		}

	private:
		Color3f mAlbedo;
		float mFuzz;

	};

	//
	class GlassMaterial : public MaterialBase
	{
	public:
		GlassMaterial() : mRefIdx(1.5f), mAlbedoIn(1.0f, 1.0f, 1.0f), mAlbedoOut(1.0f, 1.0f, 1.0f) 
		{ 
			mpCosinePDF = new CosinePDF; 
		}
		GlassMaterial(float ref_idx, Color3f const& albedo_in, Color3f const& albedo_out)
			: mRefIdx(ref_idx), mAlbedoIn(albedo_in), mAlbedoOut(albedo_out)
		{ 
			mpCosinePDF = new CosinePDF; 
		}
		virtual ~GlassMaterial() { }

	public:
		virtual void *Clone() { return (GlassMaterial *)(new GlassMaterial(mRefIdx, mAlbedoIn, mAlbedoOut)); }

	public:
		virtual bool PathShade(Ray const& inRay, HitRecord& hitRec, Color3f& attenunation_albedo, Ray& out_ray)
		{
            Vec3f outfaced_normal;
            Vec3f reflected = Surface::Reflect(inRay.D(), hitRec.n);
            float ni_over_nt;
            Vec3f refracted;
            float reflected_prob;
            float consine;
            //attenunation_albedo = mAlbedo;

            if (Dot(inRay.D(), hitRec.n) > 0.0f)
            {
                outfaced_normal = -hitRec.n;
                ni_over_nt = mRefIdx;
                consine = Dot(inRay.D(), hitRec.n) / inRay.D().Length();
            }
            else
            {
                outfaced_normal = hitRec.n;
                ni_over_nt = 1.0f / mRefIdx;
                consine = -Dot(inRay.D(), hitRec.n) / inRay.D().Length();
            }

            if (Surface::Refract(inRay.D(), outfaced_normal, ni_over_nt, refracted))
            {
                reflected_prob = Surface::Schlick(consine, mRefIdx);
            }
            else
            {
            //	out_ray.Set(hitRec.pt, reflected);
                reflected_prob = 1.0f;
            }

            float prob_value = Random::frand48();
            if (prob_value < reflected_prob)
            {
                out_ray.Set(hitRec.wpt, reflected, inRay.T());
                attenunation_albedo = mAlbedoOut;
            }
            else
            {
                out_ray.Set(hitRec.wpt, refracted, inRay.T());
                attenunation_albedo = mAlbedoIn;
            }

            return true;

		}

//		virtual float PathShade2_pdf(Ray const& inRay, const HitRecord& hitRec, const Ray& outRay)
//		{
//			Vec3f norm_scatter = outRay.D();
//			norm_scatter.MakeUnit();
//			float cosine_ = Dot(hitRec.n, norm_scatter);
//			if (cosine_ < 0.0f)
//			{
//				return 0.0f;
//			}
//			return cosine_ / PI_CONST;
//		}

		virtual bool PathShade2(Ray const& inRay, HitRecord& hitRec, ScatterRecord& scatterRec)
		{
		//	scatterRec.is_specular = false;
		//	scatterRec.albedo = mAlbedo;
		//	mpCosinePDF->SetW(hitRec.n);
		//	scatterRec.pPDF = mpCosinePDF;

			this->PathShade(inRay, hitRec, scatterRec.albedo, scatterRec.specular_ray);
			scatterRec.is_specular = true;

			return true;
		}

	private:
		float		mRefIdx;
		Color3f		mAlbedoIn;
		Color3f		mAlbedoOut;
		CosinePDF *mpCosinePDF;

	//	float		mSquareNIOverNT;
	};

	//
	class EmissiveMaterial : public MaterialBase
	{
	public:
		EmissiveMaterial(Texture *tex)
			: mpTex(tex)
		{

		}

		virtual ~EmissiveMaterial()
		{
			if (mpTex != nullptr)
			{
				delete mpTex;
				mpTex = nullptr;
			}
		}

	public:
		virtual void *Clone() { return (EmissiveMaterial *)(new EmissiveMaterial((Texture *)mpTex->Clone())); }

	public:
		virtual Color3f Emissive(Ray const& inRay, HitRecord& hitRec)
		{
            if (Dot(hitRec.n, inRay.D()) < 0.0f)
            {
				return mpTex->Sample(hitRec.u, hitRec.v);
            }
			
            return Color3f(0.0f, 0.0f, 0.0f);
		}

	public:
		Texture *	GetTexture() { return mpTex; }

	private:
		EmissiveMaterial(const EmissiveMaterial& rhs) { }
        EmissiveMaterial& operator=(const EmissiveMaterial& rhs);

	private:
		Texture *		mpTex;

	};

	//
	class IsotropicMaterial : public MaterialBase
	{
	public:
		IsotropicMaterial(Texture *tex)
			: mpTex(tex)
		{

		}

		virtual ~IsotropicMaterial()
		{
			if (mpTex)
			{
				delete mpTex;
				mpTex = nullptr;
			}
		}

	public:
		virtual void *Clone() {
			return (IsotropicMaterial *)(new IsotropicMaterial((Texture *)mpTex->Clone()));
		}

	public:
		virtual bool PathShade(Ray const& inRay, HitRecord& hitRec, Color3f& attenunation_albedo, Ray& out_ray)
		{
			out_ray.Set(hitRec.wpt, SamplerBase::SampleInUnitSphere(), inRay.T());
			attenunation_albedo = mpTex->Sample(hitRec.u, hitRec.v);
			return true;
		}

	private:
		IsotropicMaterial(const IsotropicMaterial& rhs) { }
        IsotropicMaterial& operator=(const IsotropicMaterial& rhs);

	private:
		Texture *	mpTex;

	};

	//
	class TextureOnlyMaterial : public MaterialBase
	{
	public:
		TextureOnlyMaterial()
			: mpTexture(nullptr)
		{

		}

		TextureOnlyMaterial(Texture *tex)
		{
			mpTexture = tex;
		}

		virtual ~TextureOnlyMaterial()
		{
			if (mpTexture)
			{
				delete mpTexture;
				mpTexture = nullptr;
			}
		}

	public:
		virtual void *Clone() { return (new TextureOnlyMaterial((Texture *)mpTexture->Clone())); }

	public:
		virtual Color3f Emissive(Ray const& inRay, HitRecord& hitRec)
		{
			Color3f color(0.0f, 0.0f, 0.0f);

			if (mpTexture)
			{
				return  mpTexture->GetTextureColor(hitRec);
			}

			return color;
		}

	public:
		inline void SetTexture(Texture *tex) { mpTexture = tex; }

	private:
		TextureOnlyMaterial(const TextureOnlyMaterial& rhs) { }
        TextureOnlyMaterial& operator=(const TextureOnlyMaterial& rhs);

	private:
		Texture *	mpTexture;

	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	class Matte2Material : public MaterialBase {
	public:
		Matte2Material() {
			mpAmbientBRDF = new Lambertian;
			mpDiffuseBRDF = new TextureLambertian;
		}

		Matte2Material(const Matte2Material& rhs) {
			copy_constructor(rhs);
		}

		Matte2Material& operator=(const Matte2Material& rhs) {
			if (this == &rhs) {
				return *this;
			}

			release();
			copy_constructor(rhs);

			return *this;
		}

		virtual ~Matte2Material() {
			release();
		}

	public:
		virtual void *Clone() {
			return (new Matte2Material(*this));
		}

	public:
		virtual Color3f Shade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			Vec3f wo = -inRay.D();
			Color3f L = mpAmbientBRDF->Rho(hitRec, wo) * sceneLights.GetAmbientLight()->Li(hitRec, sceneObjects);

			int light_count = sceneLights.Count();
			for (int i = 0; i < light_count; ++i) {
				Light *light = sceneLights.GetLight(i);
				Vec3f wi = light->GetDirection(hitRec);
				float ndotwi = Dot(hitRec.n, wi);

				if (ndotwi > 0.0f) {
					bool in_shadow = false;
					if (mSelfShadow && light->CastShadow()) {
						Ray shadowRay(hitRec.wpt, wi/*light->GetDirection(hitRec)*/, 0.0f);
						in_shadow = light->ShadowHit(shadowRay, sceneObjects);
					}
					if (!in_shadow) {
						L += mpDiffuseBRDF->F(hitRec, wo, wi) * light->Li(hitRec, sceneObjects) * ndotwi;
					}
				}
			}
			return L;
		}

		virtual Color3f AreaLightShade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			Vec3f wo = -inRay.D();
			Color3f L = mpAmbientBRDF->Rho(hitRec, wo) * sceneLights.GetAmbientLight()->Li(hitRec, sceneObjects);
		//	static int nnn = 0;
			int light_count = sceneLights.Count();
			for (int i = 0; i < light_count; ++i) {
				Light *light = sceneLights.GetLight(i);
				Vec3f wi = light->GetDirection(hitRec);
				float ndotwi = Dot(hitRec.n, wi);

				if (ndotwi > 0.0f) {
					bool in_shadow = false;
					if (mSelfShadow && light->CastShadow()) {
						Ray shadowRay(hitRec.wpt, wi/*light->GetDirection(hitRec)*/, 0.0f);
						in_shadow = light->ShadowHit(shadowRay, sceneObjects);
					}
					if (!in_shadow) {
						L += mpDiffuseBRDF->F(hitRec, wo, wi) * light->Li(hitRec, sceneObjects) * light->G(hitRec) * ndotwi
							/ light->Pdf(hitRec);
					}
				}
			}

			return L;
		}

		virtual bool GlobalShade(const Ray& inRay, HitRecord& hitRec, GlobalPathRecord& globalRec) {
			Vec3f wo = -inRay.D();
			globalRec.albedo = mpDiffuseBRDF->Sample_f_pdf(hitRec, wo, globalRec.reflected_dir, globalRec.pdf);
			globalRec.mat_type = 1;

			//std::cout << globalRec.albedo.X() << " " << globalRec.albedo.Y() << " " << globalRec.albedo.Z() << std::endl;

			return true;
		}

	public:
		virtual void SetKa(float ka) {
			mpAmbientBRDF->SetKa(ka);
		}

		virtual void SetKd(float kd) {
			((TextureLambertian *)mpDiffuseBRDF)->SetKd(kd);
		}

		virtual void SetCd(const Color3f& cd) {
			mpAmbientBRDF->SetCd(cd);
		//	mpDiffuseBRDF->SetCd(cd);
			((TextureLambertian *)mpDiffuseBRDF)->SetCd(new ConstantTexture(cd));
		}

		virtual void SetCd2(float r, float g, float b) {
			mpAmbientBRDF->SetCd2(r, g, b);
		//	mpDiffuseBRDF->SetCd2(r, g, b);
			((TextureLambertian *)mpDiffuseBRDF)->SetCd(new ConstantTexture(Color3f(r, g, b)));
		}

		virtual void SetCd3(float c) {
			mpAmbientBRDF->SetCd3(c);
		//	mpDiffuseBRDF->SetCd3(c);
			((TextureLambertian *)mpDiffuseBRDF)->SetCd(new ConstantTexture(Color3f(c, c, c)));
		}

		virtual void SetCd(const Color3f& cd, Texture *tex) {
            mpAmbientBRDF->SetCd(cd);
            ((TextureLambertian *)mpDiffuseBRDF)->SetCd(tex);
		}

	public:
        inline void SetSampler(SamplerBase *sampler)
        {
            mpAmbientBRDF->SetSampler(sampler);
            mpDiffuseBRDF->SetSampler(sampler);
        }

	public:
		inline void SetDiffuseBRDF(BRDF *diffuseBRDF)
		{
			if (mpDiffuseBRDF != nullptr)
			{
				delete mpDiffuseBRDF;
			}
			mpDiffuseBRDF = diffuseBRDF;
		}

	private:
		inline void release() {
			if (mpAmbientBRDF) {
				delete mpAmbientBRDF;
				mpAmbientBRDF = nullptr;
			}

			if (mpDiffuseBRDF) {
				delete mpDiffuseBRDF;
				mpDiffuseBRDF = nullptr;
			}
		}

		inline void copy_constructor(Matte2Material const& rhs) {
			if (rhs.mpAmbientBRDF) {
				mpAmbientBRDF = (Lambertian *)(rhs.mpAmbientBRDF->Clone());
			}
			else {
				mpAmbientBRDF = nullptr;
			}

			if (rhs.mpDiffuseBRDF) {
				mpDiffuseBRDF = (BRDF *)(rhs.mpDiffuseBRDF->Clone());
			}
			else {
				mpDiffuseBRDF = nullptr;
			}
		}

	private:
		Lambertian *	mpAmbientBRDF;
        BRDF *			mpDiffuseBRDF;

	};

	//
	class PlasticMaterial : public MaterialBase {
	public:
		PlasticMaterial() {
			mpAmbientBRDF = nullptr;
			mpDiffuseBRDF = nullptr;
			mpGlossyBRDF = nullptr;
		}

		PlasticMaterial(PlasticMaterial const& rhs) {
			copy_constructor(rhs);
		}

		PlasticMaterial& operator=(PlasticMaterial const& rhs) {
			if (this == &rhs) {
				return *this;
			}

			release();
			copy_constructor(rhs);

			return *this;
		}

		virtual ~PlasticMaterial() {
			release();
		}

	public:
		virtual void *Clone() {
			return (new PlasticMaterial(*this));
		}

	public:
		virtual Color3f Shade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			Vec3f wo = -inRay.D();
			Color3f L = mpAmbientBRDF->Rho(hitRec, wo) * sceneLights.GetAmbientLight()->Li(hitRec, sceneObjects);

			int light_count = sceneLights.Count();
			for (int i = 0; i < light_count; ++i) {
				Light *light = sceneLights.GetLight(i);
				Vec3f wi = light->GetDirection(hitRec);
				float ndotwi = Dot(hitRec.n, wi);

				if (ndotwi > 0.0f) {
					bool in_shadow = false;
					if (mSelfShadow && light->CastShadow()) {
						Ray shadowRay(hitRec.wpt, wi,/*light->GetDirection(hitRec),*/ 0.0f);
						in_shadow = light->ShadowHit(shadowRay, sceneObjects);
					}
					if (!in_shadow) {
						L += (mpDiffuseBRDF->F(hitRec, wo, wi) + mpGlossyBRDF->F(hitRec, wo, wi)) * light->Li(hitRec, sceneObjects) * ndotwi;
					}
				}
			}
			
			return L;
		}

		virtual Color3f AreaLightShade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			Vec3f wo = -inRay.D();
			Color3f L = mpAmbientBRDF->Rho(hitRec, wo) * sceneLights.GetAmbientLight()->Li(hitRec, sceneObjects);

			int light_count = sceneLights.Count();
			for (int i = 0; i < light_count; ++i) {
				Light *light = sceneLights.GetLight(i);
				Vec3f wi = light->GetDirection(hitRec);
				float ndotwi = Dot(hitRec.n, wi);

				if (ndotwi > 0.0f) {
					bool in_shadow = false;
					if (mSelfShadow && light->CastShadow()) {
						Ray shadowRay(hitRec.wpt, wi,/*light->GetDirection(hitRec),*/ 0.0f);
						in_shadow = light->ShadowHit(shadowRay, sceneObjects);
					}
					if (!in_shadow) {
						L += (mpDiffuseBRDF->F(hitRec, wo, wi) + mpGlossyBRDF->F(hitRec, wo, wi)) * light->Li(hitRec, sceneObjects) * light->G(hitRec) * ndotwi
							/ light->Pdf(hitRec);
					}
				}
			}

			return L;
		}

	public:
		inline void SetAmbientBRDF(BRDF *ambientBRDF) {
			if (mpAmbientBRDF) { delete mpAmbientBRDF; mpAmbientBRDF = nullptr; }
			mpAmbientBRDF = ambientBRDF;
		}

		inline void SetDiffuseBRDF(BRDF *diffuseBRDF) {
			if (mpDiffuseBRDF) { delete mpDiffuseBRDF; mpDiffuseBRDF = nullptr; }
			mpDiffuseBRDF = diffuseBRDF;
		}

		inline void SetGlossyBRDF(BRDF *glossyBRDF) {
			if (mpGlossyBRDF) { delete mpGlossyBRDF; mpGlossyBRDF = nullptr; }
			mpGlossyBRDF = glossyBRDF;
		}

        inline void SetSampler(SamplerBase *sampler)
        {
            mpAmbientBRDF->SetSampler(sampler);
            mpDiffuseBRDF->SetSampler(sampler);
            mpGlossyBRDF->SetSampler(sampler);
        }

	protected:
		inline void release() {
			if (mpAmbientBRDF) {
				delete mpAmbientBRDF;
				mpAmbientBRDF = nullptr;
			}

			if (mpDiffuseBRDF) {
				delete mpDiffuseBRDF;
				mpDiffuseBRDF = nullptr;
			}

			if (mpGlossyBRDF) {
				delete mpGlossyBRDF;
				mpGlossyBRDF = nullptr;
			}
		}

		inline void copy_constructor(PlasticMaterial const& rhs) {
			if (rhs.mpAmbientBRDF) {
				mpAmbientBRDF = (BRDF *)(rhs.mpAmbientBRDF->Clone());
			}
			else {
				mpAmbientBRDF = nullptr;
			}

			if (rhs.mpDiffuseBRDF) {
				mpDiffuseBRDF = (BRDF *)(rhs.mpDiffuseBRDF->Clone());
			}
			else {
				mpDiffuseBRDF = nullptr;
			}

			if (rhs.mpGlossyBRDF) {
				mpGlossyBRDF = (BRDF *)(rhs.mpGlossyBRDF->Clone());
			}
			else {
				mpGlossyBRDF = nullptr;
			}
		}

	protected:
		BRDF *	mpAmbientBRDF;
		BRDF *	mpDiffuseBRDF;
		BRDF *	mpGlossyBRDF;

	};

	//
	class PhongMaterial : public PlasticMaterial {
	public:
		PhongMaterial() {
			mpAmbientBRDF = new Lambertian;
			mpDiffuseBRDF = new Lambertian;
			mpGlossyBRDF = new PhongGlossySpecular;
		}

		PhongMaterial(PhongMaterial const& rhs)
			: PlasticMaterial(rhs) {

		}

		PhongMaterial& operator=(PhongMaterial const& rhs) {
			if (this == &rhs) {
				return *this;
			}

			PlasticMaterial::operator=(rhs);

			return *this;
		}

		virtual ~PhongMaterial() {

		}

	public:
		virtual void *Clone() {
			return (new PhongMaterial(*this));
		}

	public:
		virtual Color3f Shade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			Color3f L = PlasticMaterial::Shade(inRay, hitRec, sceneObjects, sceneLights);
			return L;
		}

		virtual Color3f AreaLightShade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			Color3f L = PlasticMaterial::AreaLightShade(inRay, hitRec, sceneObjects, sceneLights);
			return L;
		}

	public:
		virtual void SetKa(float ka) {
			((Lambertian *)mpAmbientBRDF)->SetKa(ka);
		}

		virtual void SetKd(float kd) {
			((Lambertian *)mpDiffuseBRDF)->SetKd(kd);
		}

		virtual void SetCd(const Color3f& cd) {
			((Lambertian *)mpAmbientBRDF)->SetCd(cd);
			((Lambertian *)mpDiffuseBRDF)->SetCd(cd);
		}

		virtual void SetCd2(float r, float g, float b) {
			((Lambertian *)mpAmbientBRDF)->SetCd2(r, g, b);
			((Lambertian *)mpDiffuseBRDF)->SetCd2(r, g, b);
		}

		virtual void SetCd3(float c) {
			((Lambertian *)mpAmbientBRDF)->SetCd3(c);
			((Lambertian *)mpDiffuseBRDF)->SetCd3(c);
		}

		virtual void SetKs(float ks) {
			((PhongGlossySpecular *)mpGlossyBRDF)->SetKs(ks);
		}

		virtual void SetCs(const Color3f& cs) {
			((PhongGlossySpecular *)mpGlossyBRDF)->SetCs(cs);
		}

		virtual void SetCs2(float r, float g, float b) {
			((PhongGlossySpecular *)mpGlossyBRDF)->SetCs2(r, g, b);
		}

		virtual void SetCs3(float c) {
			((PhongGlossySpecular *)mpGlossyBRDF)->SetCs3(c);
		}

		virtual void SetEXP(float exp) {
			((PhongGlossySpecular *)mpGlossyBRDF)->SetEXP(exp);
		}
	};

	//
	class BilnnPhongMaterial : public PhongMaterial {
	public:
		BilnnPhongMaterial() {
			mpAmbientBRDF = new Lambertian;
			mpDiffuseBRDF = new Lambertian;
			mpGlossyBRDF = new BlinnPhongGlossySpecular;
		}

		BilnnPhongMaterial(BilnnPhongMaterial const& rhs)
			: PhongMaterial(rhs) {

		}

		BilnnPhongMaterial& operator=(BilnnPhongMaterial const& rhs) {
			if (this == &rhs) {
				return *this;
			}

			PhongMaterial::operator=(rhs);

			return *this;
		}

		virtual ~BilnnPhongMaterial() {

		}

	public:
		virtual void *Clone() {
			return (new BilnnPhongMaterial(*this));
		}

	public:
		virtual Color3f Shade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			Color3f L = PlasticMaterial::Shade(inRay, hitRec, sceneObjects, sceneLights);
			return L;
		}

		virtual Color3f AreaLightShade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			Color3f L = PlasticMaterial::AreaLightShade(inRay, hitRec, sceneObjects, sceneLights);
			return L;
		}

	public:
		virtual void SetKs(float ks) {
			((BlinnPhongGlossySpecular *)mpGlossyBRDF)->SetKs(ks);
		}

		virtual void SetCs(const Color3f& cs) {
			((BlinnPhongGlossySpecular *)mpGlossyBRDF)->SetCs(cs);
		}

		virtual void SetCs2(float r, float g, float b) {
			((BlinnPhongGlossySpecular *)mpGlossyBRDF)->SetCs2(r, g, b);
		}

		virtual void SetCs3(float c) {
			((BlinnPhongGlossySpecular *)mpGlossyBRDF)->SetCs3(c);
		}

		virtual void SetEXP(float exp) {
			((BlinnPhongGlossySpecular *)mpGlossyBRDF)->SetEXP(exp);
		}
	};

	//
	class ReflectiveMaterial : public PhongMaterial {
	public:
		ReflectiveMaterial()
			: PhongMaterial() {
			mpPerfectSpecularBRDF = new PerfectSpecular;
		}

		ReflectiveMaterial(ReflectiveMaterial const& rhs)
			: PhongMaterial(rhs) {
			mpPerfectSpecularBRDF = (PerfectSpecular *)rhs.mpPerfectSpecularBRDF->Clone();
		}

		ReflectiveMaterial& operator=(ReflectiveMaterial const& rhs) {
			if (this == &rhs) {
				return *this;
			}

			PhongMaterial::operator=(rhs);
			release();
			mpPerfectSpecularBRDF = (PerfectSpecular *)rhs.mpPerfectSpecularBRDF->Clone();

			return *this;
		}

		virtual ~ReflectiveMaterial() {
			release();
		}

	public:
		virtual void *Clone() {
			return (new ReflectiveMaterial(*this));
		}

	public:
		virtual Color3f Shade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			Color3f L = PhongMaterial::Shade(inRay, hitRec, sceneObjects, sceneLights);
			return L;
		}

		virtual bool WhittedShade(const Ray& inRay, HitRecord& hitRec, Vec3f& wi, Color3f& CR) {
			Vec3f wo = -inRay.D();
			CR = mpPerfectSpecularBRDF->Sample_f(hitRec, wo, wi);

			return true;
		}

		virtual bool GlobalShade(const Ray& inRay, HitRecord& hitRec, GlobalPathRecord& globalRec) {
			Vec3f wo = -inRay.D();
			globalRec.albedo = mpPerfectSpecularBRDF->Sample_f_pdf(hitRec, wo, globalRec.reflected_dir, globalRec.pdf);
			globalRec.mat_type = 3;

			return true;
		}

	public:
		inline void SetKr(float kr) {
			mpPerfectSpecularBRDF->SetKr(kr);
		}

		inline void SetCr(Color3f const& cr) {
			mpPerfectSpecularBRDF->SetCr(cr);
		}

		inline void SetCr2(float r, float g, float b) {
			mpPerfectSpecularBRDF->SetCr2(r, g, b);
		}

		inline void SetCr3(float c) {
			mpPerfectSpecularBRDF->SetCr3(c);
		}

        inline void SetSampler(SamplerBase *sampler)
        {
            mpPerfectSpecularBRDF->SetSampler(sampler);
        }

	private:
		inline void release() {
			if (mpPerfectSpecularBRDF != nullptr) {
				delete mpPerfectSpecularBRDF;
				mpPerfectSpecularBRDF = nullptr;
			}
		}

	private:
		PerfectSpecular *	mpPerfectSpecularBRDF;

	};

	//
	class GlossyReflectiveMaterial : public PhongMaterial {
	public:
		GlossyReflectiveMaterial()
			: PhongMaterial() {
			mpAmbientBRDF = new Lambertian;
			mpDiffuseBRDF = new Lambertian;
			mpGlossyBRDF = new PhongGlossySpecular;
		}

		GlossyReflectiveMaterial(GlossyReflectiveMaterial const& rhs)
			: PhongMaterial(rhs) {

		}

		GlossyReflectiveMaterial& operator=(GlossyReflectiveMaterial const& rhs) {
			if (this == &rhs) {
				return *this;
			}

			PhongMaterial::operator=(rhs);

			return *this;
		}

		virtual ~GlossyReflectiveMaterial() {

		}

	public:
		virtual void *Clone() {
			return (new GlossyReflectiveMaterial(*this));
		}

	public:
		virtual bool WhittedShade(const Ray& inRay, HitRecord& hitRec, Vec3f& wi, Color3f& CR) {
			Vec3f wo = -inRay.D();

			float pdf = 1.0f;
			CR = mpGlossyBRDF->Sample_f_pdf(hitRec, wo, wi, pdf);

			return true;
		}

		virtual Color3f AreaLightShade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			Color3f L = PhongMaterial::AreaLightShade(inRay, hitRec, sceneObjects, sceneLights);
			return L;
		}

		virtual bool AreaLightShade(const Ray& inRay, HitRecord& hitRec, Vec3f& wi, Color3f& CR, float& pdf) {
			Vec3f wo = -inRay.D();
			pdf = 1.0f;

			CR = mpGlossyBRDF->Sample_f_pdf(hitRec, wo, wi, pdf);

			return true;
		}

        virtual bool GlobalShade(const Ray& inRay, HitRecord& hitRec, GlobalPathRecord& globalRec) {
            Vec3f wo = -inRay.D();
            globalRec.albedo = mpGlossyBRDF->Sample_f_pdf(hitRec, wo, globalRec.reflected_dir, globalRec.pdf);
            globalRec.mat_type = 2;

            return true;
        }

	public:
		inline void SetSampler(SamplerBase *sampler) {
			mpGlossyBRDF->SetSampler(sampler);
		}

	};

	//
	class Emissive2Material : public MaterialBase
	{
	public:
		Emissive2Material()
			: mKe(0.0f), mCe(BLACK) {

		}
		Emissive2Material(float ke, Color3f const& ce)
			: mKe(ke), mCe(ce) {

		}
		virtual ~Emissive2Material() { }

	public:
		virtual void *Clone() {
			return (new Emissive2Material(mKe, mCe));
		}

	public:
		virtual Color3f Emissive(Ray const& inRay, HitRecord& hitRec) {
			return mKe * mCe;
		}

		virtual Color3f Shade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			return mKe * mCe;
		}

		virtual Color3f AreaLightShade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			return mKe * mCe;
		}

		virtual bool GlobalShade(const Ray& inRay, HitRecord& hitRec, GlobalPathRecord& globalRec) {
		    // If there is shaped(e.g shpere) area light to be added to light objects,
		    // the follow statements should be commented out.
//			if (globalRec.depth == 1) {
//				globalRec.albedo = BLACK;
//				return true;
//			}

			Vec3f wo = -inRay.D();
			if (Dot(hitRec.n, wo) > 0.0f) {
				globalRec.albedo = mKe * mCe;
			}
			else {
				globalRec.albedo = BLACK;
			}
			globalRec.mat_type = 4;

			return true;
		}

	public:
		inline void SetRadiance(float ke) {
			mKe = ke;
		}

		inline void SetEmissiveColor(Color3f const& ce) {
			mCe = ce;
		}

	private:
		float mKe;
		Color3f mCe;
	};

	//
	class EmissiveTextureMaterial : public MaterialBase
	{
	public:
		EmissiveTextureMaterial()
			: mKe(0.0f), mpCe(nullptr) {

		}
		EmissiveTextureMaterial(float ke, Texture *ce)
			: mKe(ke), mpCe(ce) {

		}

		virtual ~EmissiveTextureMaterial() { }

	public:
		virtual void *Clone() {
			return (new EmissiveTextureMaterial(mKe, mpCe));
		}

	public:
		virtual Color3f Emissive(Ray const& inRay, HitRecord& hitRec) {
			return mKe * mpCe->GetTextureColor(hitRec);
		}

		virtual Color3f Shade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			return mKe * mpCe->GetTextureColor(hitRec);
		}

		virtual Color3f AreaLightShade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			return mKe * mpCe->GetTextureColor(hitRec);
		}

		virtual bool GlobalShade(const Ray& inRay, HitRecord& hitRec, GlobalPathRecord& globalRec) {
			Vec3f wo = -inRay.D();
			if (Dot(hitRec.n, wo) > 0.0f) {
				globalRec.albedo = mKe * mpCe->GetTextureColor(hitRec);
			}
			else {
				globalRec.albedo = BLACK;
			}
			globalRec.mat_type = 4;

			return true;
		}

	public:
		inline void SetRadiance(float ke) {
			mKe = ke;
		}

		inline void SetEmissiveColor(Texture *ce) {
			mpCe = ce;
		}

	private:
		float mKe;
		Texture *mpCe; // not be in charge of deleting.
	};

	//
	class DielectricMaterial : public PhongMaterial {
	public:
		DielectricMaterial()
			: PhongMaterial(), mCfIn(GLASS_COLOR), mCfOut(WHITE), mpFresenlReflective(nullptr), mpFresnelTransmitter(nullptr) {
			create_fresnel();
		}
		DielectricMaterial(const DielectricMaterial& rhs)
			: PhongMaterial(rhs), mCfIn(rhs.mCfIn), mCfOut(rhs.mCfOut), mpFresenlReflective(nullptr), mpFresnelTransmitter(nullptr) {
			create_fresnel();
		}
		DielectricMaterial(const Color3f& cfIn, const Color3f& cfOut, float etaIn, float etaOut) {
            mpFresenlReflective = nullptr;
            mpFresnelTransmitter = nullptr;

			create_fresnel();
			mpFresenlReflective->SetEtaIn(etaIn);
			mpFresnelTransmitter->SetEtaOut(etaOut);
			mCfIn = cfIn;
			mCfOut = cfOut;
		}
		DielectricMaterial& operator=(const DielectricMaterial& rhs) {
			if (this == &rhs) {
				return *this;
			}

			PhongMaterial::operator=(rhs);
			release_fresnel();
			if (rhs.mpFresenlReflective != nullptr) {
				this->mpFresenlReflective = (FresnelReflective *)rhs.mpFresenlReflective->Clone();
			}
			if (rhs.mpFresnelTransmitter != nullptr) {
				this->mpFresnelTransmitter = (FresnelTransmitter *)rhs.mpFresnelTransmitter->Clone();
			}
			mCfIn = rhs.mCfIn;
			mCfOut = rhs.mCfOut;

			return *this;
		}
		virtual ~DielectricMaterial() {
			release_fresnel();
		}

	public:
		virtual void *Clone() {
			return (void *)(new DielectricMaterial(*this));
		}

	public:
		virtual bool WhittedShade(const Ray& inRay, HitRecord& hitRec, WhittedRecord& whittedRec) {

			Vec3f wo = -inRay.D();
			whittedRec.fr = mpFresenlReflective->Sample_f(hitRec, wo, whittedRec.wr);

			float ndotwi = Dot(hitRec.n, whittedRec.wr);

			if (mpFresnelTransmitter->IsTotalInternalReflection(hitRec, wo)) {
				if (ndotwi < 0.0f) {
					whittedRec.cf1 = mCfIn;
					whittedRec.feature = 1;
				}
				else {
					whittedRec.cf2 = mCfOut;
					whittedRec.feature = 2;
				}
			}
			else {
				whittedRec.ft = mpFresnelTransmitter->Sample_f(hitRec, wo, whittedRec.wt);

			//	float ndotwt = Dot(hitRec.n, whittedRec.wt);

				if (ndotwi < 0.0f) {
					whittedRec.cf1 = mCfIn;
					whittedRec.cf2 = mCfOut;
					whittedRec.feature = 3;
				}
				else {
					whittedRec.cf1 = mCfIn;
					whittedRec.cf2 = mCfOut;
					whittedRec.feature = 4;
				}
			}

			return true;
		}

		virtual bool GlobalShade(const Ray& inRay, HitRecord& hitRec, GlobalPathRecord& globalRec) {
			globalRec.mat_type = 5;
			return true;
		}

	public:
		inline void SetEtaIn(float etaIn) {
			if (mpFresenlReflective) {
				mpFresenlReflective->SetEtaIn(etaIn);
			}
			if (mpFresnelTransmitter) {
				mpFresnelTransmitter->SetEtaIn(etaIn);
			}
		}
		inline void SetEtaOut(float etaOut) {
			if (mpFresenlReflective) {
				mpFresenlReflective->SetEtaOut(etaOut);
			}
			if (mpFresnelTransmitter) {
				mpFresnelTransmitter->SetEtaOut(etaOut);
			}
		}

		inline void SetColorFilter_In(const Color3f& cfIn) {
			mCfIn = cfIn;
		}
		inline void SetColorFilter_In(float r, float g, float b) {
			mCfIn.Set(r, g, b);
		}
		inline void SetColorFilter_In(float c) {
			mCfIn.Set(c, c, c);
		}

		inline void SetColorFilter_Out(const Color3f& cfOut) {
			mCfOut = cfOut;
		}
		inline void SetColorFilter_Out(float r, float g, float b) {
			mCfOut.Set(r, g, b);
		}
		inline void SetColorFilter_Out(float c) {
			mCfOut.Set(c, c, c);
		}

	private:
		inline void create_fresnel() {
			if (mpFresenlReflective) {
				delete mpFresenlReflective;
			}
			mpFresenlReflective = new FresnelReflective;

			if (mpFresnelTransmitter) {
				delete mpFresnelTransmitter;
			}
			mpFresnelTransmitter = new FresnelTransmitter;
		}

		inline void release_fresnel() {
			if (mpFresenlReflective) {
				delete mpFresenlReflective;
				mpFresenlReflective = nullptr;
			}

			if (mpFresnelTransmitter) {
				delete mpFresnelTransmitter;
				mpFresnelTransmitter = nullptr;
			}
		}

	private:
		Color3f mCfIn;
		Color3f mCfOut;

		FresnelReflective *mpFresenlReflective;
		FresnelTransmitter *mpFresnelTransmitter;

	};

	//
    class Glass2Material : public PhongMaterial {
    public:
        Glass2Material()
                : PhongMaterial() {
            mpTransparentBRDF = new TransparentSpecular(1.5f, WHITE, WHITE);
        }

        Glass2Material(float refIdx, Color3f const& ctin, Color3f const& ctout)
                : PhongMaterial() {
            mpTransparentBRDF = new TransparentSpecular(refIdx, ctin, ctout);
        }

        Glass2Material(const Glass2Material& rhs)
                : PhongMaterial(rhs) {
            if (rhs.mpTransparentBRDF != nullptr) {
                mpTransparentBRDF = (TransparentSpecular *)rhs.mpTransparentBRDF->Clone();
            } else {
                mpTransparentBRDF = nullptr;
            }
        }

        Glass2Material& operator=(const Glass2Material& rhs) {
            if (this == &rhs) {
                return *this;
            }

            PhongMaterial::operator=(rhs);
            if (rhs.mpTransparentBRDF != nullptr) {
                mpTransparentBRDF = (TransparentSpecular *)rhs.mpTransparentBRDF->Clone();
            } else {
                mpTransparentBRDF = nullptr;
            }

            return *this;
        }

        virtual ~Glass2Material() {
            if (mpTransparentBRDF != nullptr) {
                delete mpTransparentBRDF;
                mpTransparentBRDF = nullptr;
            }
        }

    public:
        virtual void *Clone() {
            return (void *)(new Glass2Material(*this));
        }

    public:
        virtual bool WhittedShade(const Ray& inRay, HitRecord& hitRec, Vec3f& wi, Color3f& CR) {
            float pdf = 1.0f;
            Vec3f wo = -inRay.D();
            CR = mpTransparentBRDF->Sample_f_pdf(hitRec, wo, wi, pdf);

            return true;
        }

        virtual bool AreaLightShade(const Ray& inRay, HitRecord& hitRec, Vec3f& wi, Color3f& CR, float& pdf) {
            Vec3f wo = -inRay.D();
            pdf = 1.0f;

            CR = mpTransparentBRDF->Sample_f_pdf(hitRec, wo, wi, pdf);

            return true;
        }

        virtual bool GlobalShade(const Ray& inRay, HitRecord& hitRec, GlobalPathRecord& globalRec) {
            Vec3f wo = -inRay.D();
            globalRec.albedo = mpTransparentBRDF->Sample_f_pdf(hitRec, wo, globalRec.reflected_dir, globalRec.pdf);
            globalRec.mat_type = 2;

            return true;
        }

    private:
        TransparentSpecular *mpTransparentBRDF;

    };

	//
	class MetalMaterial : public MaterialBase {
	public:
        MetalMaterial()
            : MaterialBase() {
            mpAmbientBRDF = new Lambertian;
            mpAmbientBRDF->SetKa(0.3f);
            mpAmbientBRDF->SetCd2(0.01f, 0.01f, 0.01f);

            mpLambertianBRDF = nullptr;
            mbDiffuse = false;
            mbOrenNayar = false;

            mpPerfectSpecularBRDF = nullptr;
            mbSpecular = false;

			mpTransparentBRDF = nullptr;
			mbTransparent = false;

#ifdef MICROFACET_SMITH_GGX
			mpGlossyBRDF = new MicrofacetSmithGGX(15, 1.0f, 0.25, Color3f(0.9, 0.9, 0.9));
#endif // MICROFACET_SMITH_GGX

#ifdef MICROFACET_COOK
            mpGlossyBRDF = new MicrofacetSpecular(0.001f, 1.0f, 0.25f, Color3f(0.9f, 0.9f, 0.9f));
            MultiJitteredSampler *pSampler = new MultiJitteredSampler(100);
            pSampler->MapSamplesToHemiShpere(180.0f);
            mpGlossyBRDF->SetSampler(pSampler);
#endif // MICROFACET_COOK

            mbDarken = false;
        }

        MetalMaterial(const MetalMaterial& rhs)
         : MaterialBase(rhs) {
            copy_object(rhs);
        }

        MetalMaterial& operator=(const MetalMaterial& rhs) {
            if (this == &rhs) {
                return *this;
            }

            MaterialBase::operator=(rhs);
            copy_object(rhs);

            return *this;
        }

        virtual ~MetalMaterial() {
            release_brdfs();
        }

	public:
	    virtual void *Clone() {
            return (void *)(new MetalMaterial(*this));
	    }

	public:
        virtual Color3f Shade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
            Vec3f wo = -inRay.D();
            Color3f L = mpAmbientBRDF->Rho(hitRec, wo) * sceneLights.GetAmbientLight()->Li(hitRec, sceneObjects);

            int light_count = sceneLights.Count();
            for (int i = 0; i < light_count; ++i) {
                Light *light = sceneLights.GetLight(i);
                Vec3f wi = light->GetDirection(hitRec);
                float ndotwi = Dot(hitRec.n, wi);

                if (ndotwi > 0.0f) {
                    bool in_shadow = false;
                    if (mSelfShadow && light->CastShadow()) {
                        Ray shadowRay(hitRec.wpt, wi,/*light->GetDirection(hitRec),*/ 0.0f);
                        in_shadow = light->ShadowHit(shadowRay, sceneObjects);
                    }
                    if (!in_shadow) {
                        if (mbDiffuse && !mbOrenNayar) {
                            L += (mpLambertianBRDF->F(hitRec, wo, wi) + mpGlossyBRDF->F(hitRec, wo, wi)) *
                                    light->Li(hitRec, sceneObjects) * ndotwi;
                        } else {
                            L += (mpGlossyBRDF->F(hitRec, wo, wi)) *
                                 light->Li(hitRec, sceneObjects) * ndotwi;
                        }
                    }
                }
            }

            return L;
        }

        virtual Color3f AreaLightShade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
            Vec3f wo = -inRay.D();
            Color3f L = mpAmbientBRDF->Rho(hitRec, wo) * sceneLights.GetAmbientLight()->Li(hitRec, sceneObjects);

            int light_count = sceneLights.Count();
            for (int i = 0; i < light_count; ++i) {
                Light *light = sceneLights.GetLight(i);
                Vec3f wi = light->GetDirection(hitRec);
                float ndotwi = Dot(hitRec.n, wi);

                if (ndotwi > 0.0f) {
                    bool in_shadow = false;
                    if (mSelfShadow && light->CastShadow()) {
                        Ray shadowRay(hitRec.wpt, wi,/*light->GetDirection(hitRec),*/ 0.0f);
                        in_shadow = light->ShadowHit(shadowRay, sceneObjects);
                    }
                    if (!in_shadow) {
                        if (mbDiffuse && !mbOrenNayar) {
                            L += (mpLambertianBRDF->F(hitRec, wo, wi) + mpGlossyBRDF->F(hitRec, wo, wi)) *
                                 light->Li(hitRec, sceneObjects) * light->G(hitRec) * ndotwi / light->Pdf(hitRec);
                        } else {
                            L += (mpGlossyBRDF->F(hitRec, wo, wi)) *
                                 light->Li(hitRec, sceneObjects) * light->G(hitRec) * ndotwi / light->Pdf(hitRec);
                        }
                    }
                }
            }

            return L;
        }

        virtual bool AreaLightShade(const Ray& inRay, HitRecord& hitRec, Vec3f& wi, Color3f& CR, float& pdf) {
            Vec3f wo = -inRay.D();
            pdf = 1.0f;

			if (mbSpecular) {
				CR = mpPerfectSpecularBRDF->Sample_f(hitRec, wo, wi);
			}
			else if (mbTransparent) {
				CR = mpTransparentBRDF->Sample_f_pdf(hitRec, wo, wi, pdf);
			}
			else {
				CR = mpGlossyBRDF->Sample_f_pdf(hitRec, wo, wi, pdf);
				if (mbDarken) {
					CR = CR * Dot(hitRec.n, wi); // optional.
				}
			}

            return true;
        }

        virtual bool WhittedShade(const Ray& inRay, HitRecord& hitRec, Vec3f& wi, Color3f& CR) {
            Vec3f wo = -inRay.D();

            float pdf = 1.0f;
            if (mbSpecular) {
                CR = mpPerfectSpecularBRDF->Sample_f(hitRec, wo, wi);
			}
			else if (mbTransparent) {
				CR = mpTransparentBRDF->Sample_f_pdf(hitRec, wo, wi, pdf);
			}
			else {
                CR = mpGlossyBRDF->Sample_f_pdf(hitRec, wo, wi, pdf);
            }

            return true;
        }

        virtual bool GlobalShade(const Ray& inRay, HitRecord& hitRec, GlobalPathRecord& globalRec) {
            Vec3f wo = -inRay.D();

            if (mbSpecular) {
                globalRec.albedo = mpPerfectSpecularBRDF->Sample_f_pdf(hitRec, wo, globalRec.reflected_dir, globalRec.pdf);
                globalRec.mat_type = 3;
			}
			else if (mbTransparent) {
				globalRec.albedo = mpTransparentBRDF->Sample_f_pdf(hitRec, wo, globalRec.reflected_dir, globalRec.pdf);
				globalRec.mat_type = 2;
			}
			else {
                globalRec.albedo = mpGlossyBRDF->Sample_f_pdf(hitRec, wo, globalRec.reflected_dir, globalRec.pdf);
                globalRec.mat_type = 2;
            }

            return true;
        }

	public:
	    inline void SetAmbient(float ka, Color3f const& ambient) {
            mpAmbientBRDF->SetKa(0.3f);
            mpAmbientBRDF->SetCd(ambient);
	    }

	    inline void SetDiffuse(float kd, Texture *diffuse) {
            mbDiffuse = true;
            mbOrenNayar = false;
            if (mpLambertianBRDF == nullptr) {
                mpLambertianBRDF = new TextureLambertian;
            }
            mpLambertianBRDF->SetKd(kd);
            mpLambertianBRDF->SetCd(diffuse);
	    }

	    inline void SetSpecular(float kr, Color3f const& cr) {
            mbSpecular = true;
			if (mpTransparentBRDF != nullptr) {
				delete mpTransparentBRDF;
				mpTransparentBRDF = nullptr;
			}
            if (mpPerfectSpecularBRDF == nullptr) {
                mpPerfectSpecularBRDF = new PerfectSpecular;
            }
            mpPerfectSpecularBRDF->SetKr(kr);
            mpPerfectSpecularBRDF->SetCr(cr);
	    }

		inline void SetTransparent(float refIdx, Color3f const& ctin, Color3f const& ctout, bool reflect = true) {
			mbTransparent = true;
			if (mpPerfectSpecularBRDF != nullptr) {
				delete mpPerfectSpecularBRDF;
				mpPerfectSpecularBRDF = nullptr;
			}
			if (mpTransparentBRDF == nullptr) {
				mpTransparentBRDF = new TransparentSpecular(refIdx, ctin, ctout, reflect);
			}
			else {
				mpTransparentBRDF->SetRefIdx(refIdx);
				mpTransparentBRDF->SetTransparentColor(ctin, ctout);
				mpTransparentBRDF->EnableReflect(reflect);
			}
		}

#ifdef MICROFACET_COOK
	    inline void SetGlossy(float k, float f, float roughness, Color3f const& reflection, int sampleNum = 100, float sampleExp = 180.0f) {
            mpGlossyBRDF->SetK(k);
            mpGlossyBRDF->SetF(f);
            mpGlossyBRDF->SetRoughness(roughness);
            mpGlossyBRDF->SetReflection(reflection);
            MultiJitteredSampler *pSampler = new MultiJitteredSampler(sampleNum);
            pSampler->MapSamplesToHemiShpere(sampleExp);
            mpGlossyBRDF->SetSampler(pSampler);
	    }
#endif // MICROFACET_COOK

#ifdef MICROFACET_COOK
	    inline void DarkenMetal(bool darken) {
            mbDarken = darken;
        }
#endif // MICROFACET_COOK

#ifdef MICROFACET_SMITH_GGX
		inline void SetGlossy(float etaT, float etaI, float roughness, const Color3f& reflection) {
			mpGlossyBRDF->SetEtaT(etaT);
			mpGlossyBRDF->SetEtaI(etaI);
			mpGlossyBRDF->SetRoughness(roughness);
			mpGlossyBRDF->SetReflection(reflection);
		}
#endif // MICROFACET_SMITH_GGX

	private:
	    inline void release_brdfs() {
	        if (mpAmbientBRDF != nullptr) { delete mpAmbientBRDF; mpAmbientBRDF = nullptr; }
            if (mpLambertianBRDF != nullptr) { delete mpLambertianBRDF; mpLambertianBRDF = nullptr; }
            if (mpPerfectSpecularBRDF != nullptr) { delete mpPerfectSpecularBRDF; mpPerfectSpecularBRDF = nullptr; }
			if (mpTransparentBRDF != nullptr) { delete mpTransparentBRDF; mpTransparentBRDF = nullptr; }
            if (mpGlossyBRDF != nullptr) { delete mpGlossyBRDF; mpGlossyBRDF = nullptr; }

	    }

	    inline void copy_object(const MetalMaterial& rhs) {
	        if (rhs.mpAmbientBRDF != nullptr) {
	            this->mpAmbientBRDF = (Lambertian *)rhs.mpAmbientBRDF->Clone();
	        } else {
	            this->mpAmbientBRDF = nullptr;
	        }

	        if (rhs.mpLambertianBRDF != nullptr) {
	            this->mpLambertianBRDF = (TextureLambertian *)rhs.mpLambertianBRDF->Clone();
	        } else {
	            this->mpLambertianBRDF = nullptr;
	        }

	        if (rhs.mpPerfectSpecularBRDF != nullptr) {
	            this->mpPerfectSpecularBRDF = (PerfectSpecular *)rhs.mpPerfectSpecularBRDF->Clone();
	        } else {
	            this->mpPerfectSpecularBRDF = nullptr;
	        }

			if (rhs.mpTransparentBRDF != nullptr) {
				this->mpTransparentBRDF = (TransparentSpecular *)rhs.mpTransparentBRDF->Clone();
			}
			else {
				this->mpTransparentBRDF = nullptr;
			}

	        if (rhs.mpGlossyBRDF != nullptr) {
#ifdef MICROFACET_COOK
	            this->mpGlossyBRDF = (MicrofacetSpecular *)rhs.mpGlossyBRDF;
#endif // MICROFACET_SMITH_GGX

#ifdef MICROFACET_SMITH_GGX
				this->mpGlossyBRDF = (MicrofacetSmithGGX *)rhs.mpGlossyBRDF;
#endif // MICROFACET_SMITH_GGX
	        } else {
	            this->mpGlossyBRDF = nullptr;
	        }

            mbDiffuse = rhs.mbDiffuse;
            mbOrenNayar = rhs.mbOrenNayar;
            mbSpecular = rhs.mbSpecular;
			mbTransparent = rhs.mbTransparent;
            mbDarken = rhs.mbDarken;
	    }

	private:
        Lambertian *	mpAmbientBRDF;
        TextureLambertian *mpLambertianBRDF;
        PerfectSpecular *	mpPerfectSpecularBRDF;
        TransparentSpecular * mpTransparentBRDF;
#ifdef MICROFACET_COOK
        MicrofacetSpecular *	mpGlossyBRDF;
#endif // MICROFACET_COOK
#ifdef MICROFACET_SMITH_GGX
		MicrofacetSmithGGX *mpGlossyBRDF;
#endif // MICROFACET_SMITH_GGX

        bool mbDiffuse;
        bool mbOrenNayar;
        bool mbSpecular;
		bool mbTransparent;
        bool mbDarken;
	};

	//
	class SubstrateMaterial : public MaterialBase {
	public:
		SubstrateMaterial() : MaterialBase() {
			create_material(Color3f(0.843117, 0.059106, 0.000849), Color3f(0.067215, 0.067215, 0.067215), 0.01f, 0.3f, 100, 180.0f);
		}

		SubstrateMaterial(Color3f const& rd, Color3f const& rs, float roughness, float factor = 0.3f, int sampleNum = 100, float exp = 180.0f)
			: MaterialBase() {
			create_material(rd, rs, roughness, factor, sampleNum, exp);
		}

		SubstrateMaterial(const SubstrateMaterial& rhs)
			: MaterialBase(rhs) {
			if (rhs.mpFresnelBlend != nullptr) {
				mpFresnelBlend = (FresnelBlend *)rhs.mpFresnelBlend->Clone();
			}
			else {
				mpFresnelBlend = nullptr;
			}
		}

		SubstrateMaterial& operator=(const SubstrateMaterial& rhs) {
			if (this == &rhs) {
				return *this;
			}

			MaterialBase::operator=(rhs);
			if (rhs.mpFresnelBlend != nullptr) {
				mpFresnelBlend = (FresnelBlend *)rhs.mpFresnelBlend->Clone();
			}
			else {
				mpFresnelBlend = nullptr;
			}

			return *this;
		}

		virtual ~SubstrateMaterial() {
			if (mpFresnelBlend != nullptr) {
				delete mpFresnelBlend;
				mpFresnelBlend = nullptr;
			}
		}

	public:
		virtual void *Clone() {
			return (void *)(new SubstrateMaterial(*this));
		}

	public:
		virtual Color3f Shade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			Vec3f wo = -inRay.D();
			Color3f L = BLACK;

			int light_count = sceneLights.Count();
			for (int i = 0; i < light_count; ++i) {
				Light *light = sceneLights.GetLight(i);
				Vec3f wi = light->GetDirection(hitRec);
				float ndotwi = Dot(hitRec.n, wi);

				if (ndotwi > 0.0f) {
					bool in_shadow = false;
					if (mSelfShadow && light->CastShadow()) {
						Ray shadowRay(hitRec.wpt, wi,/*light->GetDirection(hitRec),*/ 0.0f);
						in_shadow = light->ShadowHit(shadowRay, sceneObjects);
					}
					if (!in_shadow) {
						L += mpFresnelBlend->F(hitRec, wo, wi) * light->Li(hitRec, sceneObjects) * ndotwi;
					}
				}
			}

			return L;
		}

		virtual Color3f AreaLightShade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			Vec3f wo = -inRay.D();
			Color3f L = BLACK;

			int light_count = sceneLights.Count();
			for (int i = 0; i < light_count; ++i) {
				Light *light = sceneLights.GetLight(i);
				Vec3f wi = light->GetDirection(hitRec);
				float ndotwi = Dot(hitRec.n, wi);

				if (ndotwi > 0.0f) {
					bool in_shadow = false;
					if (mSelfShadow && light->CastShadow()) {
						Ray shadowRay(hitRec.wpt, wi,/*light->GetDirection(hitRec),*/ 0.0f);
						in_shadow = light->ShadowHit(shadowRay, sceneObjects);
					}
					if (!in_shadow) {
						L += mpFresnelBlend->F(hitRec, wo, wi) *
							light->Li(hitRec, sceneObjects) * light->G(hitRec) * ndotwi / light->Pdf(hitRec);
					}
				}
			}

			return L;
		}

		virtual bool AreaLightShade(const Ray& inRay, HitRecord& hitRec, Vec3f& wi, Color3f& CR, float& pdf) {
			Vec3f wo = -inRay.D();
			pdf = 1.0f;

			CR = mpFresnelBlend->Sample_f_pdf(hitRec, wo, wi, pdf);

			return true;
		}

		virtual bool WhittedShade(const Ray& inRay, HitRecord& hitRec, Vec3f& wi, Color3f& CR) {
			Vec3f wo = -inRay.D();

			float pdf = 1.0f;
			CR = mpFresnelBlend->Sample_f_pdf(hitRec, wo, wi, pdf);

			return true;
		}

		virtual bool GlobalShade(const Ray& inRay, HitRecord& hitRec, GlobalPathRecord& globalRec) {
			Vec3f wo = -inRay.D();

			globalRec.albedo = mpFresnelBlend->Sample_f_pdf(hitRec, wo, globalRec.reflected_dir, globalRec.pdf);

			return true;
		}

	public:
		inline void SetDiffuseEx(Texture *texDiffuse) {
			mpFresnelBlend->SetDiffuseEx(texDiffuse);
		}

	private:
		inline void create_material(Color3f const& rd, Color3f const& rs, float roughness, float factor, int sampleNum, float exp) {
			mpFresnelBlend = new FresnelBlend(rd, rs, roughness, factor);

			MultiJitteredSampler *pSampler = new MultiJitteredSampler(sampleNum);
			pSampler->MapSamplesToHemiShpere(exp);
			mpFresnelBlend->SetSampler(pSampler);
		}

	private:
		FresnelBlend *	mpFresnelBlend;

	};

    //
    // rough conductor and rough glass materials using MicrofacetBRDF model.
    //
    class RoughConductor : public MaterialBase {
    public:
        RoughConductor() {
            mpMicrofacetBRDF = nullptr;
        }
        RoughConductor(float roughness, float etaI, float etaT) {
            mpMicrofacetBRDF = new MicrofacetBRDF(roughness, etaI, etaT, true);
            mpMicrofacetBRDF->SetFresnelFunc(MicrofacetBRDF::FresnelType::Conductor, etaI, etaT);
            mpMicrofacetBRDF->SetDTermFunc(MicrofacetBRDF::DFuncType::GGX);
        }
        RoughConductor(const RoughConductor& rhs) {
            mpMicrofacetBRDF = (MicrofacetBRDF *)rhs.mpMicrofacetBRDF->Clone();
        }
        RoughConductor& operator=(const RoughConductor& rhs) {
            if (this == &rhs) {
                return *this;
            }
            
            release_brdf();
            mpMicrofacetBRDF = (MicrofacetBRDF *)rhs.mpMicrofacetBRDF->Clone();
            
            return *this;
        }
        virtual ~RoughConductor() {
            release_brdf();
        }
        
    public:
        virtual void *Clone() {
            RoughConductor *roughConductor = new RoughConductor(*this);
            return roughConductor;
        }
        
    public:
        virtual bool PathShade(Ray const& inRay, HitRecord& hitRec, Color3f& attenunation_albedo, Ray& out_ray)
        {
            return false;
        }

        virtual float PathShade2_pdf(Ray const& inRay, const HitRecord& hitRec, const Ray& outRay)
        {
            return 0.0f;
        }

        virtual bool PathShade2(Ray const& inRay, HitRecord& hitRec, ScatterRecord& scatterRec)
        {
            return false;
        }
        
    private:
        inline void release_brdf() {
            if (mpMicrofacetBRDF != nullptr) {
                delete mpMicrofacetBRDF;
                mpMicrofacetBRDF = nullptr;
            }
        }
        
    private:
        MicrofacetBRDF *mpMicrofacetBRDF;
        
    };

    class RoughGlass : public MaterialBase {
    public:
        RoughGlass() {
            mpMicrofacetBRDF = nullptr;
        }
        RoughGlass(float roughness, float etaI, float etaT) {
            mpMicrofacetBRDF = new MicrofacetBRDF(roughness, etaI, etaT, true);
            mpMicrofacetBRDF->SetFresnelFunc(MicrofacetBRDF::FresnelType::Dielectic, BLACK, BLACK);
            mpMicrofacetBRDF->SetDTermFunc(MicrofacetBRDF::DFuncType::GGX);
        }
        RoughGlass(const RoughGlass& rhs) {
            mpMicrofacetBRDF = (MicrofacetBRDF *)rhs.mpMicrofacetBRDF->Clone();
        }
        RoughGlass& operator=(const RoughGlass& rhs) {
            if (this == &rhs) {
                return *this;
            }
            
            release_brdf();
            mpMicrofacetBRDF = (MicrofacetBRDF *)rhs.mpMicrofacetBRDF->Clone();
            
            return *this;
        }
        virtual ~RoughGlass() {
            release_brdf();
        }
        
    public:
        virtual void *Clone() {
            RoughGlass *roughGlass = new RoughGlass(*this);
            return roughGlass;
        }
        
    public:
        virtual bool PathShade(Ray const& inRay, HitRecord& hitRec, Color3f& attenunation_albedo, Ray& out_ray)
        {
            return false;
        }

        virtual float PathShade2_pdf(Ray const& inRay, const HitRecord& hitRec, const Ray& outRay)
        {
            return 0.0f;
        }

        virtual bool PathShade2(Ray const& inRay, HitRecord& hitRec, ScatterRecord& scatterRec)
        {
            return false;
        }
        
    private:
        inline void release_brdf() {
            if (mpMicrofacetBRDF != nullptr) {
                delete mpMicrofacetBRDF;
                mpMicrofacetBRDF = nullptr;
            }
        }
        
    private:
        MicrofacetBRDF *mpMicrofacetBRDF;
    };

}
