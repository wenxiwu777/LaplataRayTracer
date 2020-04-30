#pragma once

#include "Vec3.h"
#include "HitRecord.h"
#include "Ray.h"
#include "Random.h"
#include "Texture.h"
#include "BRDF.h"
#include "PDF.h"
#include "WorldObjects.h"
#include "Light.h"
#include "ICloneable.h"
#include "Sampling.h"
#include "Surface.h"

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
		virtual Color3f Emisive(Ray const& inRay, HitRecord& hitRec) = 0;
		virtual Color3f Shade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) = 0;
		virtual Color3f AreaLightShade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) = 0;
		virtual bool AreaLightShade(const Ray& inRay, HitRecord& hitRec, Vec3f& wi, Color3f& CR) = 0;
		virtual bool WhittedShade(const Ray& inRay, HitRecord& hitRec, Vec3f& wi, Color3f& CR) = 0;
		virtual bool PathShade(Ray const& inRay, HitRecord& hitRec, Color3f& attenunation_albedo, Ray& out_ray) = 0;
		virtual float PathShade2_pdf(Ray const& inRay, const HitRecord& hitRec, const Ray& outRay) = 0;
		virtual bool PathShade2(Ray const& inRay, HitRecord& hitRec, ScatterRecord& scatterRec) = 0;

	};

	//
	class MaterialBase : public Material
	{
	public:
		MaterialBase() { }
		virtual ~MaterialBase() { }

	public:
		virtual void *Clone() {
			return (new MaterialBase);
		}

	public:
		virtual Color3f Emisive(Ray const& inRay, HitRecord& hitRec) { return Color3f(0.0f, 0.0f, 0.0f); }

		virtual Color3f Shade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) { return BLACK; }

		virtual Color3f AreaLightShade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights)
		{
			return BLACK;
		}

		virtual bool AreaLightShade(const Ray& inRay, HitRecord& hitRec, Vec3f& wi, Color3f& CR)
		{
			return false;
		}

		virtual bool WhittedShade(const Ray& inRay, HitRecord& hitRec, Vec3f& wi, Color3f& CR)
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
			return Emisive(inRay, hitRec);
		}

		virtual Color3f Emisive(Ray const& inRay, HitRecord& hitRec)
		{
			Color3f col(0.5f*(hitRec.n.X() + 1.0f), 0.5f*(hitRec.n.Y() + 1.0f), 0.5f*(hitRec.n.Z() + 1.0f));
			return col;
		}

	};

	//
	class MatteMaterial : public MaterialBase
	{
	public:
		MatteMaterial() : mpAlbedo(nullptr) { mpCosinePDF = new CosinePDF; }
		MatteMaterial(Color3f const& albedo)
		{
			mpAlbedo = new ConstantTexture(albedo);
			mpCosinePDF = new CosinePDF;
		}
		MatteMaterial(Texture *albedo) : mpAlbedo(albedo) { mpCosinePDF = new CosinePDF; }
		virtual ~MatteMaterial()
		{
			release();
		}

	public:
		virtual void *Clone() { return (MatteMaterial *)(new MatteMaterial((Texture *)mpAlbedo->Clone())); }

	public:
		virtual bool PathShade(Ray const& inRay, HitRecord& hitRec, Color3f& attenunation_albedo, Ray& out_ray)
		{
			Vec3f sample = SamplerBase::SampleInUnitSphere();
			Vec3f ptInUnitShpereCenter = hitRec.pt + hitRec.n + sample;
			out_ray.Set(hitRec.pt, ptInUnitShpereCenter - hitRec.pt, inRay.T());
			attenunation_albedo = mpAlbedo->GetTextureColor(hitRec);
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
			scatterRec.alebdo = mpAlbedo->GetTextureColor(hitRec);
			mpCosinePDF->SetW(hitRec.n);
			scatterRec.pPDF = mpCosinePDF;
			return true;
		}

	public:
		inline void Set_cd(Texture *alebdo)
		{
			if (mpAlbedo) {
				release();
			}

			mpAlbedo = alebdo;
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
		}

	private:
		MatteMaterial(MatteMaterial const&) { }
		MatteMaterial& operator=(MatteMaterial const&) { }

	private:
	//	Color3f mAlbedo;
		Texture *mpAlbedo;
		CosinePDF *mpCosinePDF;

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
			out_ray.Set(hitRec.pt, r + mFuzz * SamplerBase::SampleInUnitSphere(), inRay.T());
			attenunation_albedo = mAlbedo;
			return (Dot(r, hitRec.n) > 0.0f);
		}

		virtual bool PathShade2(Ray const& inRay, HitRecord& hitRec, ScatterRecord& scatterRec)
		{
			Vec3f norm_dir = inRay.D();
			norm_dir.MakeUnit();
			Vec3f r = Surface::Reflect(norm_dir, hitRec.n);
			scatterRec.is_specular = true;
			scatterRec.specular_ray.Set(hitRec.pt, r + mFuzz * SamplerBase::SampleInUnitSphere(), inRay.T());
			scatterRec.alebdo = mAlbedo;
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
		GlassMaterial() : mRefIdx(1.5f), mAlbedo(1.0f, 1.0f, 1.0f) { mpCosinePDF = new CosinePDF; }
		GlassMaterial(float ref_idx, Color3f const& albedo)
			: mRefIdx(ref_idx), mAlbedo(albedo) { mpCosinePDF = new CosinePDF; }
		virtual ~GlassMaterial() { }

	public:
		virtual void *Clone() { return (GlassMaterial *)(new GlassMaterial(mRefIdx, mAlbedo)); }

	public:
		virtual bool PathShade(Ray const& inRay, HitRecord& hitRec, Color3f& attenunation_albedo, Ray& out_ray)
		{
			Vec3f outfaced_normal;
			Vec3f reflected = Surface::Reflect(inRay.D(), hitRec.n);
			float ni_over_nt;
			Vec3f refracted;
			float reflected_prob;
			float consine;
			attenunation_albedo = mAlbedo;

			if (Dot(inRay.D(), hitRec.n) > 0.0f)
			{
				outfaced_normal = -hitRec.n;
				ni_over_nt = mRefIdx;
				consine = mRefIdx * Dot(inRay.D(), hitRec.n) / inRay.D().Length();
			}
			else
			{
				outfaced_normal = hitRec.n;
				ni_over_nt = 1.0f / mRefIdx;
				consine = -Dot(inRay.D(), hitRec.n) / inRay.D().Length();
			}

			if (Surface::Refract(inRay.D(), outfaced_normal, ni_over_nt, refracted))
			{
				reflected_prob = schlick(consine, mRefIdx);
			}
			else
			{
			//	out_ray.Set(hitRec.pt, reflected);
				reflected_prob = 1.0f;
			}

			float prob_value = Random::frand48();
			if (prob_value < reflected_prob)
			{
				out_ray.Set(hitRec.pt, reflected, inRay.T());
			}
			else
			{
				out_ray.Set(hitRec.pt, refracted, inRay.T());
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
		//	scatterRec.alebdo = mAlbedo;
		//	mpCosinePDF->SetW(hitRec.n);
		//	scatterRec.pPDF = mpCosinePDF;

			this->PathShade(inRay, hitRec, scatterRec.alebdo, scatterRec.specular_ray);
			scatterRec.is_specular = true;

			return true;
		}

	private:
		inline float schlick(float consine, float ref_idx)
		{
			float r0 = (1 - ref_idx) / (1 + ref_idx);
			r0 = r0 * r0;
			float value = r0 + (1 - r0) * std::pow((1 - consine), 5);
			return value;
		}

	private:
		float		mRefIdx;
		Color3f		mAlbedo;
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
		virtual Color3f Emisive(Ray const& inRay, HitRecord& hitRec)
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
		EmissiveMaterial& operator=(const EmissiveMaterial& rhs) { }

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
			out_ray.Set(hitRec.pt, SamplerBase::SampleInUnitSphere(), inRay.T());
			attenunation_albedo = mpTex->Sample(hitRec.u, hitRec.v);
			return true;
		}

	private:
		IsotropicMaterial(const IsotropicMaterial& rhs) { }
		IsotropicMaterial& operator=(const IsotropicMaterial& rhs) { }

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
		virtual Color3f Emisive(Ray const& inRay, HitRecord& hitRec)
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
		TextureOnlyMaterial& operator=(const TextureOnlyMaterial& rhs) { }

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
			mpDiffuseBRDF = new Lambertian;
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
					if (light->CastShadow()) {
						Ray shadowRay(hitRec.pt, wi/*light->GetDirection(hitRec)*/, 0.0f);
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

			int light_count = sceneLights.Count();
			for (int i = 0; i < light_count; ++i) {
				Light *light = sceneLights.GetLight(i);
				Vec3f wi = light->GetDirection(hitRec);
				float ndotwi = Dot(hitRec.n, wi);

				if (ndotwi > 0.0f) {
					bool in_shadow = false;
					if (light->CastShadow()) {
						Ray shadowRay(hitRec.pt, wi/*light->GetDirection(hitRec)*/, 0.0f);
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

	public:
		virtual void SetKa(float ka) {
			mpAmbientBRDF->SetKa(ka);
		}

		virtual void SetKd(float kd) {
			mpDiffuseBRDF->SetKd(kd);
		}

		virtual void SetCd(const Color3f& cd) {
			mpAmbientBRDF->SetCd(cd);
			mpDiffuseBRDF->SetCd(cd);
		}

		virtual void SetCd2(float r, float g, float b) {
			mpAmbientBRDF->SetCd2(r, g, b);
			mpDiffuseBRDF->SetCd2(r, g, b);
		}

		virtual void SetCd3(float c) {
			mpAmbientBRDF->SetCd3(c);
			mpDiffuseBRDF->SetCd3(c);
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
				mpDiffuseBRDF = (Lambertian *)(rhs.mpDiffuseBRDF->Clone());
			}
			else {
				mpDiffuseBRDF = nullptr;
			}
		}

	private:
		Lambertian *	mpAmbientBRDF;
		Lambertian *	mpDiffuseBRDF;

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
					if (light->CastShadow()) {
						Ray shadowRay(hitRec.pt, wi,/*light->GetDirection(hitRec),*/ 0.0f);
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
					if (light->CastShadow()) {
						Ray shadowRay(hitRec.pt, wi,/*light->GetDirection(hitRec),*/ 0.0f);
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
		virtual Color3f AreaLightShade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			Color3f L = PhongMaterial::AreaLightShade(inRay, hitRec, sceneObjects, sceneLights);
			return L;
		}

		virtual bool AreaLightShade(const Ray& inRay, HitRecord& hitRec, Vec3f& wi, Color3f& CR) {
			Vec3f wo = -inRay.D();
			float pdf = 0.0f;

			CR = mpGlossyBRDF->Sample_f_pdf(hitRec, wo, wi, pdf);

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
		virtual Color3f Emisive(Ray const& inRay, HitRecord& hitRec) {
			return mKe * mCe;
		}

		virtual Color3f Shade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			return mKe * mCe;
		}

		virtual Color3f AreaLightShade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			return mKe * mCe;
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
		virtual Color3f Emisive(Ray const& inRay, HitRecord& hitRec) {
			return mKe * mpCe->GetTextureColor(hitRec);
		}

		virtual Color3f Shade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			return mKe * mpCe->GetTextureColor(hitRec);
		}

		virtual Color3f AreaLightShade(const Ray& inRay, HitRecord& hitRec, SceneObjects& sceneObjects, SceneLights& sceneLights) {
			return mKe * mpCe->GetTextureColor(hitRec);
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

}
