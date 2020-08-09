#pragma once

#include "Console.h"
#include "Common.h"
#include "Random.h"
#include "Vec3.h"
#include "ICloneable.h"
#include "HitRecord.h"
#include "WorldObjects.h"
#include "Sampling.h"
#include "Transform.h"

namespace LaplataRayTracer
{
	class Light : public ICloneable {
	public:
		Light() {

		}

		virtual ~Light() {

		}

	public:
		virtual void *Clone() {
			return (new Light);
		}

	public:
		virtual Vec3f GetDirection(const HitRecord& hitRec) {
			return WORLD_ORIGIN;
		}

		virtual Color3f Li(HitRecord& hitRec, SceneObjects& sceneObjects) const {
			return Color3f(0.0f, 0.0f, 0.0f);
		}

		virtual float G(const HitRecord& hitRec) const {
			return 1.0f;
		}

		virtual float Pdf(const HitRecord& hitRec) const {
			return 1.0f;
		}

		virtual bool CastShadow() const {
			return false;
		}

		virtual bool ShadowHit(Ray const& shadowRay, SceneObjects const& sceneObjects) const {
			return false;
		}
	};

	//
	class AmbientLight : public Light {
	public:
		AmbientLight()
			: mLs(0.0f), mLc(BLACK) {

		}

		AmbientLight(float ls, const Color3f& lc)
			: mLs(ls), mLc(lc) {

		}

		virtual ~AmbientLight() {

		}

	public:
		virtual void *Clone() {
			return (new AmbientLight(*this));
		}

	public:
		virtual Vec3f GetDirection(const HitRecord& hitRec) {
			return WORLD_ORIGIN;
		}

		virtual Color3f Li(HitRecord& hitRec, SceneObjects& sceneObjects) const {
			return mLs * mLc;
		}

	public:
		inline void SetScaleRadiance(float ls) {
			mLs = ls;
		}

		inline float GetScaleRadiance() const {
			return mLs;
		}

		inline void SetLightColor(const Color3f& lc) {
			mLc = lc;
		}

		inline void SetLightColor2(float r, float g, float b) {
			mLc.Set(r, g, b);
		}

		inline void SetLightColor3(float c) {
			mLc.Set(c, c, c);
		}

		inline Color3f GetLightColor() const {
			return mLc;
		}

	protected:
		float	mLs;
		Color3f mLc;

	};

	//
	class AmbientOcclusionLight : public AmbientLight {
	public:
		AmbientOcclusionLight()
			: AmbientLight(), mFraction(0.2F), mpSampler(nullptr) {

		}

		AmbientOcclusionLight(float ls, float lc, float fraction)
			: AmbientLight(ls, lc), mFraction(fraction), mpSampler(nullptr) {

		}

		AmbientOcclusionLight(AmbientOcclusionLight const& rhs)
			: AmbientLight(rhs), mFraction(rhs.mFraction) {

			if (mpSampler != nullptr) { delete mpSampler; mpSampler = nullptr; }

			mpSampler = (SamplerBase *)rhs.mpSampler->Clone();
		}

		AmbientOcclusionLight& operator=(AmbientOcclusionLight const& rhs) {
			if (this == &rhs) {
				return *this;
			}

			this->operator=(rhs);

			return *this;
		}

		virtual ~AmbientOcclusionLight() {
			if (mpSampler != nullptr) {
				delete mpSampler;
				mpSampler = nullptr;
			}
		}

	public:
		virtual void *Clone() {
			return (new AmbientOcclusionLight(*this));
		}

	public:
		virtual Vec3f GetDirection(const HitRecord& hitRec){
			return WORLD_ORIGIN;
		}

		virtual Color3f Li(HitRecord& hitRec, SceneObjects& sceneObjects) const {
			Color3f L;

			// 1. Constructor UVW coordinate aligen along the N direction
			Vec3f W = hitRec.n;
			Vec3f V = Cross(W, Vec3f(0.0072f, 1.0f, 0.0034f));
			V.MakeUnit();
			Vec3f U = Cross(V, W);

			// 2. Sample upon the hemishpere
			Vec3f sample_point = mpSampler->SampleFromHemishpere();

			// 3. Convert the sample into the UVW coordinate to get the random outgoing ray from the hit point
			Vec3f ambient_occ_dir = sample_point.X() * U + sample_point.Y() * V + sample_point.Z() * W;

			// 4. do the hit test with the outgoing ray among others objects
			//Vec3f ambient_occ_dir = point_in_UVW - hitRec.pt;
			//ambient_occ_dir.MakeUnit();
			Ray ambient_occ_ray = Ray(hitRec.wpt, ambient_occ_dir, 0.0f);

			// 5. calculate the L according to the fact if the outgoing ray is hit some object
			bool is_hit = ShadowHit(ambient_occ_ray, sceneObjects);
			if (is_hit) {
				L = mFraction * mLs * mLc;
			}
			else {
				L = mLs * mLc;
			}

			return L;
		}

		virtual bool ShadowHit(Ray const& shadowRay, SceneObjects const& sceneObjects) const {
			bool isHit = false;

			int count = (int)sceneObjects.size();
			for (int i = 0; i < count; ++i) {
				float tvalue = -FLT_MAX;
				if (sceneObjects[i]->IntersectP(shadowRay, tvalue)) {
					isHit = true;
					break;
				}
			}

			return isHit;
		}

	public:
		inline void SetFraction(float f) {
			mFraction = f;
		}

		inline void SetSampler(SamplerBase *sampler, float exp) {
			if (mpSampler != nullptr) {
				delete mpSampler;
				mpSampler = nullptr;
			}

			mpSampler = sampler;
			mpSampler->MapSamplesToHemiShpere(exp);
		}

	private:
		float	mFraction;
		SamplerBase *	mpSampler;

	};

	//
	class DirectionLight : public Light {
	public:
		DirectionLight()
			: mDir(WORLD_ORIGIN), mLs(0.0f), mLc(BLACK), mShadow(true) {

		}

		DirectionLight(const Vec3f& lightDir, float ls, const Color3f& lc)
			: mDir(lightDir), mLs(ls), mLc(lc) {

		}

		virtual ~DirectionLight() {

		}

	public:
		virtual void *Clone() {
			return (new DirectionLight(*this));
		}

	public:
		virtual Vec3f GetDirection(const HitRecord& hitRec) {
			return mDir;
		}

		virtual Color3f Li(HitRecord& hitRec, SceneObjects& sceneObjects) const {
			return mLs * mLc;
		}

		virtual bool CastShadow() const {
			return mShadow;
		}

		virtual bool ShadowHit(Ray const& shadowRay, SceneObjects const& sceneObjects) const {
			bool isHit = false;

			int count = (int)sceneObjects.size();
			for (int i = 0; i < count; ++i) {
				float tvalue = -FLT_MAX;
				if (sceneObjects[i]->IntersectP(shadowRay, tvalue)) {
					isHit = true;
					break;
				}
			}

			return isHit;
		}

	public:
		inline void SetDirection(const Vec3f& dir) {
			mDir = dir;
			mDir.MakeUnit();
		}
		
		inline void SetScaleRadiance(float ls) {
			mLs = ls;
		}

		inline float GetScaleRadiance() const {
			return mLs;
		}

		inline void SetLightColor(const Color3f& lc) {
			mLc = lc;
		}

		inline void SetLightColor2(float r, float g, float b) {
			mLc.Set(r, g, b);
		}

		inline void SetLightColor3(float c) {
			mLc.Set(c, c, c);
		}

		inline Color3f GetLightColor() const {
			return mLc;
		}

		inline void EnableShadow(bool enable) {
			mShadow = enable;
		}

	private:
		Vec3f	mDir;
		float	mLs;
		Color3f mLc;
		bool	mShadow;

	};

	//
	class PointLight : public Light {
	public:
		PointLight()
			: mLightPos(WORLD_ORIGIN), mLs(0.0f), mLc(BLACK), mShadow(true), mDistAttenu(false), mDistAttenuParam(0.0f) {

		}

		PointLight(const Vec3f& lightPos, float ls, const Color3f& lc)
			: mLightPos(lightPos), mLs(ls), mLc(lc), mDistAttenu(false), mDistAttenuParam(0.0f) {

		}

		virtual ~PointLight() {

		}

	public:
		virtual void *Clone() {
			return (new PointLight(*this));
		}

	public:
		virtual Vec3f GetDirection(const HitRecord& hitRec) {
			Vec3f vecDir = (mLightPos - hitRec.wpt);
			vecDir.MakeUnit();
			return vecDir;
		}

		virtual Color3f Li(HitRecord& hitRec, SceneObjects& sceneObjects) const {
			if (mDistAttenu) {
				Vec3f vecDir = (mLightPos - hitRec.wpt);
				return ((mLs * mLc) / std::pow(vecDir.Length(), mDistAttenuParam));
			}

			return (mLs * mLc);
		}

		virtual bool CastShadow() const {
			return mShadow;
		}

		virtual bool ShadowHit(Ray const& shadowRay, SceneObjects const& sceneObjects) const {
			bool isHit = false;

			int count = (int)sceneObjects.size();
			for (int i = 0; i < count; ++i) {
				float tvalue = -FLT_MAX;
				if (sceneObjects[i]->IntersectP(shadowRay, tvalue)) {
					float dist = shadowRay.O().Distance(mLightPos);
					if (tvalue < dist) {
						isHit = true;
						break;
					}
				}
			}

			return isHit;
		}

	public:
		inline void SetPosition(const Vec3f& lightPos) {
			mLightPos = lightPos;
		}

		inline void SetScaleRadiance(float ls) {
			mLs = ls;
		}

		inline float GetScaleRadiance() const {
			return mLs;
		}

		inline void SetLightColor(const Color3f& lc) {
			mLc = lc;
		}

		inline void SetLightColor2(float r, float g, float b) {
			mLc.Set(r, g, b);
		}

		inline void SetLightColor3(float c) {
			mLc.Set(c, c, c);
		}

		inline Color3f GetLightColor() const {
			return mLc;
		}

		inline void EnableShadow(bool enable) {
			mShadow = enable;
		}

		inline void EnableDistanceAttenuation(bool enable, float p) {
			mDistAttenu = enable;
			mDistAttenuParam = p;
		}

	private:
		Vec3f	mLightPos;
		float	mLs;
		Color3f mLc;
		bool	mShadow;

		bool	mDistAttenu;
		float   mDistAttenuParam;

	};

	//
	class AreaLight : public Light {
	public:
		AreaLight()
			: mLightNormal(WORLD_ORIGIN), 
			mSamplePoint(WORLD_ORIGIN), mWi(WORLD_ORIGIN), 
			mpLightShape(nullptr), mShadow(true) {

		}

		AreaLight(AreaLight const& rhs)
			: mLightNormal(rhs.mLightNormal),
			  mSamplePoint(rhs.mSamplePoint), mWi(rhs.mWi),
			  mpLightShape(rhs.mpLightShape), mShadow(rhs.mShadow) {

		}

		AreaLight& operator=(AreaLight const& rhs) {
			if (this == &rhs) {
				return *this;
			}

			this->mLightNormal = rhs.mLightNormal;
			this->mSamplePoint = rhs.mSamplePoint;
			this->mWi = rhs.mWi;
			this->mpLightShape = rhs.mpLightShape;
			this->mShadow = rhs.mShadow;

			return *this;
		}

		virtual ~AreaLight() {

		}

	public:
		virtual void *Clone() {
			return (new AreaLight(*this));
		}

	public:
		virtual Vec3f GetDirection(const HitRecord& hitRec) {
			mLightNormal = mpLightShape->GetNormal(hitRec);

            mSamplePoint = mpLightShape->SampleRandomPoint();
			mWi = mSamplePoint - hitRec.wpt;
			mWi.MakeUnit();

			return mWi;
		}

		virtual Color3f Li(HitRecord& hitRec, SceneObjects& sceneObjects) const {
			return BLACK;
		}

		virtual float G(const HitRecord& hitRec) const {
			float ndotd = Dot(-mLightNormal, mWi);
			float d2 = mSamplePoint.SquareDistance(hitRec.wpt);
			return (ndotd / d2);
		}

		virtual float Pdf(const HitRecord& hitRec) const {
			float area = mpLightShape->Area();
			return 1.0f / area;
		}

		virtual bool CastShadow() const {
			return mShadow;
		}

		virtual bool ShadowHit(Ray const& shadowRay, SceneObjects const& sceneObjects) const {
			bool isHit = false;

			int count = (int)sceneObjects.size();
			for (int i = 0; i < count; ++i) {
				float tvalue = -FLT_MAX;
				if (sceneObjects[i]->IntersectP(shadowRay, tvalue)) {
				//	Vec3f oa = (mSamplePoint - shadowRay.O());
				//	float dist = Dot(oa, shadowRay.D());
					float dist = shadowRay.O().Distance(mSamplePoint);
					if (tvalue < dist) {
						isHit = true;
						break;
					}
				}
			}

			return isHit;
		}

	public:
		inline void SetLightShapeObject(GeometricObject *lightShapeObject) {
			mpLightShape = lightShapeObject;
		}

		inline void EnableShadow(bool enable) {
			mShadow = enable;
		}

	protected:
		Vec3f	mLightNormal;
		Vec3f	mSamplePoint;
		Vec3f	mWi;
		GeometricObject *mpLightShape;
		bool	mShadow;

	};

	class AreaConstLight : public AreaLight {
	public:
		AreaConstLight()
			: AreaLight(), mLAs(0.0f), mLAc(BLACK) {

		}

		AreaConstLight(float las, Color3f const& lac)
			: AreaLight(), mLAs(las), mLAc(lac) {

		}

		AreaConstLight(AreaConstLight const& rhs)
			: AreaLight(rhs), mLAs(rhs.mLAs), mLAc(rhs.mLAc) {

		}

		AreaConstLight& operator=(AreaConstLight const& rhs) {
			if (this == &rhs) {
				return *this;
			}

			AreaLight::operator=(rhs);
			this->mLAs = rhs.mLAs;
			this->mLAc = rhs.mLAc;

			return *this;
		}

		virtual ~AreaConstLight() {

		}

	public:
		virtual void *Clone() {
			return (new AreaConstLight(*this));
		}

	public:
		virtual Color3f Li(HitRecord& hitRec, SceneObjects& sceneObjects) const {
			float ndotwi = Dot(-mLightNormal, mWi);
			if (ndotwi > 0.0f) {
				return mLAs * mLAc;
			}
			
			return BLACK;
		}

	public:
		inline void SetScaleRadiance(float las) { mLAs = las; }
		inline void SetLightColor(Color3f const& lac) { mLAc = lac; }
		inline void SetLightColor2(float r, float g, float b) { mLAc.Set(r, g, b); }
		inline void SetLightColor3(float c) { mLAc.Set(c, c, c); }

	private:
		float	mLAs;
		Color3f mLAc;

	};
	
	class AreaTextureLight : public AreaLight {
	public:
		AreaTextureLight()
			: AreaLight(), mLAs(0.0f), mpLAc(nullptr) {

		}

		AreaTextureLight(float las, Texture *tex)
			: AreaLight(), mLAs(las), mpLAc(tex) {

		}

		AreaTextureLight(AreaTextureLight const& rhs)
			: AreaLight(rhs), mLAs(rhs.mLAs), mpLAc(rhs.mpLAc) {

		}

		AreaTextureLight& operator=(AreaTextureLight const& rhs) {
			if (this == &rhs) {
				return *this;
			}

			AreaLight::operator=(rhs);
			this->mLAs = rhs.mLAs;
			this->mpLAc = (Texture *)rhs.mpLAc->Clone();

			return *this;
		}

		virtual ~AreaTextureLight() {
			if (mpLAc) {
				delete mpLAc;
				mpLAc = nullptr;
			}
		}

	public:
		virtual void *Clone() {
			return (new AreaTextureLight(*this));
		}

	public:
		virtual Color3f Li(HitRecord& hitRec, SceneObjects& sceneObjects) const {
			float widotn = Dot(-mLightNormal, mWi);
			if (widotn > 0.0f) {
				HitRecord hitRecForSamplePoint;
				hitRecForSamplePoint.pt = mSamplePoint;
				hitRecForSamplePoint.n = mLightNormal;
				return (mLAs * mpLAc->GetTextureColor(hitRecForSamplePoint));
			}
			
			return BLACK;
		}

	public:
		inline void SetScaleRadiance(float las) { mLAs = las; }
		inline void SetLightColor(Texture *tex) { mpLAc = tex; }

	private:
		float mLAs;
		Texture *mpLAc;

	};

	//
	class EnvironmentLight : public Light {
	public:
		EnvironmentLight()
			: mShadow(true), mWi(WORLD_ORIGIN), mpSampler(nullptr) {

		}

		EnvironmentLight(EnvironmentLight const& rhs)
			: mShadow(rhs.mShadow), mWi(rhs.mWi) {
			if (mpSampler != nullptr) { delete mpSampler; mpSampler = nullptr; }

			mpSampler = (SamplerBase *)rhs.mpSampler->Clone();
		}

		EnvironmentLight& operator=(EnvironmentLight const& rhs) {
			if (this == &rhs) {
				return *this;
			}

			this->mShadow = rhs.mShadow;
			this->mWi = rhs.mWi;
			
			if (mpSampler) { delete mpSampler; mpSampler = nullptr; }
			if (rhs.mpSampler) {
				this->mpSampler = (SamplerBase *)rhs.mpSampler->Clone();
			}

			return *this;
		}

		virtual ~EnvironmentLight() {
			if (mpSampler) { delete mpSampler; mpSampler = nullptr; }
		}

	public:
		virtual void *Clone() {
			return (new EnvironmentLight(*this));
		}

	public:
		virtual Vec3f GetDirection(const HitRecord& hitRec) {
			Vec3f W = hitRec.n;
			Vec3f V = Cross(W, Vec3f(0.0072f, 1.0f, 0.0034f));
			V.MakeUnit();
			Vec3f U = Cross(V, W);

			mSamplePoint = mpSampler->SampleFromHemishpere();
			mWi = mSamplePoint.X() * U + mSamplePoint.Y() * V + mSamplePoint.Z() * W;
			mSamplePoint = mWi;
			mWi.MakeUnit();
			return mWi;
		}

		virtual Color3f Li(HitRecord& hitRec, SceneObjects& sceneObjects) const {
			return BLACK;
		}

		virtual float G(const HitRecord& hitRec) const {
			return 1.0f;
		}

		virtual float Pdf(const HitRecord& hitRec) const {
			return (Dot(mWi, hitRec.n) * INV_PI_CONST);
		}

		virtual bool CastShadow() const {
			return mShadow;
		}

		virtual bool ShadowHit(Ray const& shadowRay, SceneObjects const& sceneObjects) const {
			bool isHit = false;

			int count = (int)sceneObjects.size();
			for (int i = 0; i < count; ++i) {
				float tvalue = -FLT_MAX;
				if (sceneObjects[i]->IntersectP(shadowRay, tvalue)) {
					isHit = true;
					break;
				}
			}

			return isHit;
		}

	public:
		inline void EnableShadow(bool enable) {
			mShadow = enable;
		}

		inline  void SetSampler(SamplerBase *sampler) {
			mpSampler = sampler;
		}

	protected:
		bool mShadow;
		Vec3f mWi;
		Vec3f mSamplePoint;
		SamplerBase *mpSampler;

	};

	class EnvironmentConstLight : public EnvironmentLight {
	public:
		EnvironmentConstLight()
			: EnvironmentLight(), mLEs(0.0f), mLEc(BLACK) {

		}

		EnvironmentConstLight(float les, Color3f const& lec)
			: EnvironmentLight(), mLEs(les), mLEc(lec) {

		}

		EnvironmentConstLight(EnvironmentConstLight const& rhs)
			: EnvironmentLight(rhs), mLEs(rhs.mLEs), mLEc(rhs.mLEc) {

		}

		EnvironmentConstLight& operator=(EnvironmentConstLight const& rhs) {
			if (this == &rhs) {
				return *this;
			}

			EnvironmentLight::operator=(rhs);
			this->mLEs = rhs.mLEs;
			this->mLEc = rhs.mLEc;

			return *this;

		}

		virtual ~EnvironmentConstLight() {

		}

	public:
		virtual void *Clone() {
			return (new EnvironmentConstLight(*this));
		}

	public:
		virtual Color3f Li(HitRecord& hitRec, SceneObjects& sceneObjects) const {
			return mLEs * mLEc;
		}

	public:
		inline void SetRadiance(float les) { mLEs = les; }
		inline void SetLightColor(Color3f const& lc) { mLEc = lc; }
		inline void SetLightColor2(float r, float g, float b) { mLEc.Set(r, g, b); }
		inline void SetLightColor3(float c) { mLEc.Set(c, c, c); }

	private:
		float mLEs;
		Color3f mLEc;

	};

	class EnvrionmentTextureLight : public EnvironmentLight {
	public:
		EnvrionmentTextureLight()
			: mLEs(0.0f), mpLEc(nullptr) {

		}

		EnvrionmentTextureLight(float les, Texture *tex)
			: EnvironmentLight(), mLEs(les) {
			if (mpLEc) { delete mpLEc; mpLEc = nullptr; }
			if (tex) {
				mpLEc = (Texture *)tex->Clone();
			}
		}

		EnvrionmentTextureLight(EnvrionmentTextureLight const& rhs)
			: EnvironmentLight(rhs), mLEs(rhs.mLEs) {
			if (mpLEc) { delete mpLEc; mpLEc = nullptr; }
			if (rhs.mpLEc) {
				mpLEc = (Texture *)rhs.mpLEc->Clone();
			}
		}

		EnvrionmentTextureLight& operator=(EnvrionmentTextureLight const& rhs) {
			if (this == &rhs) {
				return *this;
			}

			EnvironmentLight::operator=(rhs);
			this->mLEs = rhs.mLEs;
			if (mpLEc) { delete mpLEc; mpLEc = nullptr; }
			if (rhs.mpLEc) {
				mpLEc = (Texture *)rhs.mpLEc->Clone();
			}

			return *this;
		}

		virtual ~EnvrionmentTextureLight() {
			if (mpLEc) { delete mpLEc; mpLEc = nullptr; }
		}

	public:
		virtual void *Clone() {
			return (new EnvrionmentTextureLight(*this));
		}

	public:
		virtual Color3f Li(HitRecord& hitRec, SceneObjects& sceneObjects) const {
			HitRecord hitRecForSamplePoint;
			hitRecForSamplePoint.pt = mSamplePoint;
			hitRecForSamplePoint.n = mSamplePoint;
			Color3f col = mLEs * mpLEc->GetTextureColor(hitRecForSamplePoint);
			return col;
		}

	public:
		inline void SetRadiance(float les) { mLEs = les; }
		inline void SetLightColor(Texture *tex) {
			if (mpLEc) { delete mpLEc; mpLEc = nullptr; }

			mpLEc = tex;
		}

	private:
		float mLEs;
		Texture *mpLEc;

	};

	// Additionally, we support SpotLight in our Render.
	class SpotLight : public Light {
	public:
		SpotLight() {
			SetLightPos(WORLD_ORIGIN);
			SetLightDirection(AXIS_Y_BIAS);
			SetUmbra(60.0f);
			SetPenumbra(40.0f);
			SetLightIntensity(2.0f);
			SetLightColor(Color3f(1.0f, 1.0f, 1.0f));
			SetFallOffFactor(1.0f);
			SetDistanceAttenuationFactors(1.0f, 1.0f, 1.0f);
			EnableShadow(true);
		}

		virtual ~SpotLight() {

		}

	public:
		virtual void *Clone() {
			return (new SpotLight(*this));
		}

	public:
		virtual Vec3f GetDirection(const HitRecord& hitRec) {
			mSurfaceDir = mLightPos - hitRec.wpt;
			mSurfaceDir.MakeUnit();
			return mSurfaceDir;
		}

		virtual Color3f Li(HitRecord& hitRec, SceneObjects& sceneObjects) const {
			Color3f L;

			float dist = hitRec.wpt.Distance(mLightPos);
			float sdotdir = Dot(-mSurfaceDir, mLightDir);
			if (sdotdir < mHalfCosPenumbraRad) {
				L.Set(0.0f, 0.0f, 0.0f);
			}
			else if (/*sdotdir > 0.0f && */sdotdir > mHalfCosUmbraRad) {
				L = (mLightIntensity * mLightColor);
				float dist_attenuation = (mKc + mKl * dist + mKq * dist * dist);
				L /= dist_attenuation;
			//	g_Console.Write("path 1\n");
			}
			else if (/*sdotdir > 0.0f && */sdotdir < mHalfCosUmbraRad && sdotdir > mHalfCosPenumbraRad) {
				float dist_attenuation = (mKc + mKl * dist + mKq * dist * dist);
				Color3f I = (mLightIntensity * mLightColor);
				I /= dist_attenuation;
				float FallOff = std::pow(sdotdir - mHalfCosPenumbraRad, mPF) / (mHalfCosUmbraRad - mHalfCosPenumbraRad);
				L = I * FallOff;
			//	g_Console.Write("path 2\n");
			}

			return L;
		}

		virtual float G(const HitRecord& hitRec) const {
			return 1.0f;
		}

		virtual float Pdf(const HitRecord& hitRec) const {
			return 1.0f;
		}

		virtual bool CastShadow() const {
			return mCastShadow;
		}

		virtual bool ShadowHit(Ray const& shadowRay, SceneObjects const& sceneObjects) const {
			bool isHit = false;

			int count = (int)sceneObjects.size();
			for (int i = 0; i < count; ++i) {
				float tvalue = -FLT_MAX;
				if (sceneObjects[i]->IntersectP(shadowRay, tvalue)) {
					float dist = shadowRay.O().Distance(mLightPos);
					if (tvalue < dist) {
						isHit = true;
						break;
					}
				}
			}

			return isHit;
		}

	public:
		inline void SetLightPos(Vec3f const& pos) {
			mLightPos = pos;
		}
		inline void SetLightDirection(Vec3f const& dir) {
			mLightDir = dir;
			mLightDir.MakeUnit();
		}

		// Should update mHalfUmbra and mHalfPenUmbra simultaneously.
		inline void SetUmbra(float umbra) {
			mUmbra = umbra;
			mHalfUmbraRad = ANG2RAD(mUmbra/2.0f);
			mHalfCosUmbraRad = std::cos(mHalfUmbraRad);
		}
		inline void SetPenumbra(float penumbra) {
			mPenUmbra = penumbra;
			mHalfPenumbraRad = ANG2RAD(mPenUmbra/2.0f);
			mHalfCosPenumbraRad = std::cos(mHalfPenumbraRad);
		}

		inline void SetLightIntensity(float intensity) {
			mLightIntensity = intensity;
		}
		inline void SetLightColor(Color3f const& lightColor) {
			mLightColor = lightColor;
		}

		//
		inline void SetFallOffFactor(float pf) {
			mPF = pf;
		}
		inline void SetDistanceAttenuationFactors(float kc, float kl, float kq) {
			mKc = kc;
			mKl = kl;
			mKq = kq;
		}

		inline void EnableShadow(bool enable) {
			mCastShadow = enable;
		}

	private:
		//
		Vec3f	mLightPos;
		Vec3f	mLightDir;
		float	mUmbra;		// The inner angle
		float	mPenUmbra;	// The outer angle
		float	mLightIntensity;
		Color3f mLightColor;

		float  mPF;
		float	mKc;
		float	mKl;
		float	mKq;

		//
		float	mHalfUmbraRad;
		float	mHalfPenumbraRad;
		float	mHalfCosUmbraRad;
		float	mHalfCosPenumbraRad;

		//
		bool	mCastShadow;

		Vec3f	mSurfaceDir;

	};

	//
	class ProjectiveTextureSpotLight : public Light {
	public:
		ProjectiveTextureSpotLight() {
			SetLightPos(WORLD_ORIGIN);
			SetLightDirection(AXIS_Y_BIAS);
			SetLightViewAngle(60.0f);
			SetLightIntensity(1.0f);
			SetProjectiveTexture(nullptr);
			EnableShadowCast(true);

		}

		ProjectiveTextureSpotLight(ProjectiveTextureSpotLight const& rhs) {
			copy_constructor(rhs);

		}

		ProjectiveTextureSpotLight& operator=(ProjectiveTextureSpotLight const& rhs) {
			if (this == &rhs) {
				return *this;
			}

			copy_constructor(rhs);

			return *this;
		}

		virtual ~ProjectiveTextureSpotLight() {
			release_projective_texture();
		}

	public:
		virtual void *Clone() {
			return (ProjectiveTextureSpotLight *)(new ProjectiveTextureSpotLight(*this));
		}

	public:
		virtual Vec3f GetDirection(const HitRecord& hitRec) {
			mSurfaceDir = mLightPos - hitRec.wpt;
			mSurfaceDir.MakeUnit();

			return mSurfaceDir;
		}

		virtual Color3f Li(HitRecord& hitRec, SceneObjects& sceneObjects) const {
			Color3f L;

			float sdotdir = Dot(-mSurfaceDir, mLightDir);
			if (sdotdir < mCosHalfViewAngle || sdotdir < 0.0f) {
				L.Set(0.0f, 0.0f, 0.0f);
			}
			else {
				// conver hit point into ONB space
				Vec3f v_local = mONB.Local2(hitRec.wpt);
				
				// Address the texuture directly
				HitRecord recForTex;
				recForTex.pt = v_local;
				L = mpProjectiveTexture->GetTextureColor(recForTex);
				L = mLightIntensity * L;
			}

			return L;
		}

		virtual float G(const HitRecord& hitRec) const {
			return 1.0f;
		}

		virtual float Pdf(const HitRecord& hitRec) const {
			return 1.0f;
		}

		virtual bool CastShadow() const {
			return mCastShadow;
		}

		virtual bool ShadowHit(Ray const& shadowRay, SceneObjects const& sceneObjects) const {
			bool isHit = false;

			int count = (int)sceneObjects.size();
			for (int i = 0; i < count; ++i) {
				float tvalue = -FLT_MAX;
				if (sceneObjects[i]->IntersectP(shadowRay, tvalue)) {
					float dist = shadowRay.O().Distance(mLightPos);
					if (tvalue < dist) {
						isHit = true;
						break;
					}
				}
			}

			return isHit;
		}

	public:
		inline void SetLightPos(Vec3f const& pos) {
			mLightPos = pos;
		}

		inline void SetLightDirection(Vec3f const& dir) {
			mLightDir = dir;
			mLightDir.MakeUnit();

			setup_light_view_uvw();
		}

		inline void SetLightViewAngle(float viewAngle) {
			mLightViewAngle = viewAngle;
			mHalfViewAngleRad = ANG2RAD(viewAngle / 2.0f);
			mCosHalfViewAngle = std::cos(mHalfViewAngleRad);

		}

		inline void SetLightIntensity(float lightIntensity) {
			mLightIntensity = lightIntensity;
		}

		inline void SetProjectiveTexture(Texture *tex) {
			if (tex != nullptr) {
				release_projective_texture();

				mpProjectiveTexture = tex;
			}
		}

		inline void EnableShadowCast(bool enable) {
			mCastShadow = enable;
		}

	private:
		inline void setup_light_view_uvw() {
			mONB.BuildFromW(mLightDir);

		}

		inline void release_projective_texture() {
			if (mpProjectiveTexture != nullptr) {
				delete mpProjectiveTexture;
				mpProjectiveTexture = nullptr;
			}
		}

		inline void copy_constructor(ProjectiveTextureSpotLight const& rhs) {
			this->mLightPos = rhs.mLightPos;
			this->mLightDir = rhs.mLightDir;
			this->mLightViewAngle = rhs.mLightViewAngle;
			this->mLightIntensity = rhs.mLightIntensity;

			if (this->mpProjectiveTexture != nullptr) {
				release_projective_texture();
			}
			this->mpProjectiveTexture = (Texture *)rhs.mpProjectiveTexture->Clone();

			this->mHalfViewAngleRad = rhs.mHalfViewAngleRad;
			this->mCosHalfViewAngle = rhs.mCosHalfViewAngle;

			this->mONB = rhs.mONB;

			this->mCastShadow = rhs.mCastShadow;

			this->mSurfaceDir = rhs.mSurfaceDir;
		}

	private:
		Vec3f	mLightPos;
		Vec3f	mLightDir;
		float	mLightViewAngle;
		float	mLightIntensity;
		Texture *mpProjectiveTexture;

		float	mHalfViewAngleRad;
		float	mCosHalfViewAngle;

		ONB		mONB;

		bool	mCastShadow;

		Vec3f	mSurfaceDir;

	};

	//
	class ProjectiveTextureAreaLight : public Light {
	public:
		ProjectiveTextureAreaLight() {
			SetLightPos(WORLD_ORIGIN);
			SetLightDirection(AXIS_Y_BIAS);
			SetHorizontalViewAngle(60.0f);
			SetVerticalViewAngle(40.0f);
			SetNearDistance(0.1f);
			SetFarDistance(1000.0f);
			SetLightIntensity(1.0f);

			mpProjectiveTexture = nullptr;

			EnableShadow(true);
		}

		ProjectiveTextureAreaLight(ProjectiveTextureAreaLight const& rhs) {
			copy_constructor(rhs);

		}

		ProjectiveTextureAreaLight& operator=(ProjectiveTextureAreaLight const& rhs) {
			if (this == &rhs) {
				return *this;
			}

			copy_constructor(rhs);

			return *this;
		}

		virtual ~ProjectiveTextureAreaLight() {
			release_projective_texture();

		}

	public:
		virtual void *Clone() {
			return (ProjectiveTextureAreaLight *)(new ProjectiveTextureAreaLight(*this));

		}

	public:
		virtual Vec3f GetDirection(const HitRecord& hitRec) {
			Vec3f v_dir = mLightPos - hitRec.wpt;
			v_dir.MakeUnit();

			return v_dir;
		}

		virtual Color3f Li(HitRecord& hitRec, SceneObjects& sceneObjects) const {
			Color3f L;

			Vec3f v_view_onb = hitRec.wpt - mLightPos;
			v_view_onb = mInverseONB.Local2(v_view_onb);

			float view_z = v_view_onb.Z(); //std::abs(v_view_onb.Z());
			if (view_z < mNearDist || view_z > mFarDist) {
				L.Set(0.0f, 0.0f, 0.0f);
			}
			else {
				float view_x = (v_view_onb.X() * mNearDist) / view_z;
				float view_y = (v_view_onb.Y() * mNearDist) / view_z;

				if ((view_x >= 0 && view_x <= mViewPlaneWidth)
					&& (view_y >= 0 && view_y <= mViewPlaneHeight)) {
					HitRecord recForTex;
					recForTex.pt = hitRec.pt;
					recForTex.u = (float)(view_x / mViewPlaneWidth);
					recForTex.v = (float)(view_y / mViewPlaneHeight);

					L = mpProjectiveTexture->GetTextureColor(recForTex);
					L = mLightIntensity * L;
				}
				else {
					L.Set(0.0f, 0.0f, 0.0f);
				}

			//	g_Console.Write("111\n");
			}

			return L;
		}

		virtual float G(const HitRecord& hitRec) const {
			return 1.0f;
		}

		virtual float Pdf(const HitRecord& hitRec) const {
			return 1.0f;
		}

		virtual bool CastShadow() const {
			return mCastShadow;
		}

		virtual bool ShadowHit(Ray const& shadowRay, SceneObjects const& sceneObjects) const {
			bool isHit = false;

			int count = (int)sceneObjects.size();
			for (int i = 0; i < count; ++i) {
				float tvalue = -FLT_MAX;
				if (sceneObjects[i]->IntersectP(shadowRay, tvalue)) {
					float dist = shadowRay.O().Distance(mLightPos);
					if (tvalue < dist) {
						isHit = true;
						break;
					}
				}
			}

			return isHit;
		}

	public:
		inline void SetLightPos(Vec3f const& pos) {
			mLightPos = pos;
		}

		inline void SetLightDirection(Vec3f const& dir) {
			mLightDir = dir;

			setup_light_view_uvw();
		}

		inline void SetHorizontalViewAngle(float viewAngleH) {
			mHorizontalViewAngle = viewAngleH;

			update_view_plane();
		}

		inline void SetVerticalViewAngle(float viewAngleV) {
			mVerticalViewAngle = viewAngleV;

			update_view_plane();
		}

		inline void SetNearDistance(float nearDist) {
			mNearDist = nearDist;

			update_view_plane();
		}

		inline void SetFarDistance(float farDist) {
			mFarDist = farDist;

			update_view_plane();
		}

		inline void SetLightIntensity(float intensity) {
			mLightIntensity = intensity;
		}

		inline void SetProjectiveTexture(Texture *tex) {
			release_projective_texture();

			mpProjectiveTexture = tex;
		}

		inline void EnableShadow(bool enable) {
			mCastShadow = enable;
		}

	private:
		inline void setup_light_view_uvw() {
			mONB.BuildFromW(mLightDir);

			mInverseONB[0].Set(mONB[0][0], mONB[1][0], mONB[2][0]);
			mInverseONB[1].Set(mONB[0][1], mONB[1][1], mONB[2][1]);
			mInverseONB[2].Set(mONB[0][2], mONB[1][2], mONB[2][2]);

// 			char szBuf[128] = { 0 };
// 			sprintf_s(szBuf, sizeof(szBuf), ">>> %.2f, %.2f, %.2f\n", mInverseONB[0].X(), mInverseONB[0].Y(), mInverseONB[0].Z());
// 			g_Console.Write(szBuf);
		}

		inline void release_projective_texture() {
			if (mpProjectiveTexture != nullptr) {
				delete mpProjectiveTexture;
				mpProjectiveTexture = nullptr;
			}
		}

		inline void update_view_plane() {
			mViewPlaneWidth = calc_view_edge_length(mHorizontalViewAngle);
			mViewPlaneHeight = calc_view_edge_length(mVerticalViewAngle);

		}

		inline void copy_constructor(ProjectiveTextureAreaLight const& rhs) {
			this->mLightPos = rhs.mLightPos;
			this->mLightDir = rhs.mLightDir;
			this->mHorizontalViewAngle = rhs.mHorizontalViewAngle;
			this->mVerticalViewAngle = rhs.mVerticalViewAngle;
			this->mNearDist = rhs.mNearDist;
			this->mFarDist = rhs.mFarDist;
			this->mLightIntensity = rhs.mLightIntensity;

			if (this->mpProjectiveTexture != nullptr) {
				release_projective_texture();
			}
			this->mpProjectiveTexture = (Texture *)rhs.mpProjectiveTexture->Clone();

			this->mONB = rhs.mONB;
			this->mInverseONB = rhs.mInverseONB;

			this->mCastShadow = rhs.mCastShadow;

		}

	private:
		float calc_view_edge_length(float viewAngle) {
			float half_view_angle_rad = ANG2RAD(viewAngle / 2.0f);
			float edge_length = 2.0f * mNearDist * std::tan(half_view_angle_rad);

			return edge_length;
		}

	private:
		Vec3f	mLightPos;
		Vec3f	mLightDir;
		float	mHorizontalViewAngle;
		float	mVerticalViewAngle;
		float	mNearDist;
		float	mFarDist;
		float	mLightIntensity;
		Texture *mpProjectiveTexture;

		float	mViewPlaneWidth;
		float	mViewPlaneHeight;

		ONB		mONB;
		ONB		mInverseONB;

		bool	mCastShadow;

	};

	//
	// XX The light at index 0 is always the Ambient Light or its Sub classes, like Occlustion Ambient. XX
	// Use light container object to hold all the lights in the scene, including the ambient light.
	// Perhaps the world object should be changed like it as well.
	class SceneLights {
	public:
		AmbientLight *mpAmbientLight;
		vector<Light * > mvecLights;
		map<string, Light * > mmapNamedLights;

	public:
		SceneLights() : mpAmbientLight(nullptr) { }
		~SceneLights() {
			Purge();
		}

	public:
		inline void SetAmbientLight(AmbientLight *ambientLight) { mpAmbientLight = ambientLight; }
		inline AmbientLight *GetAmbientLight() { return mpAmbientLight; }

		inline void AddLight(Light *light) { mvecLights.push_back(light); }
		inline Light *GetLight(int idx) { return mvecLights[idx]; }
		inline void RemoveLight(Light *light) {
			vector<Light * >::iterator iterLight = mvecLights.begin();
			while (iterLight != mvecLights.end()) {
				if (*iterLight == light) {
					delete light;
					mvecLights.erase(iterLight);
					break;
				}
				++iterLight;
			}
		}
		inline int Count() const { return mvecLights.size(); }

		inline void BindNamedLight(const string& name, Light *light) {
			mmapNamedLights[name] = light;
		}
		inline Light *GetNamedLight(const string& name) {
			Light *light = nullptr;
			map<string, Light * >::iterator iterLight = mmapNamedLights.find(name);
			if (iterLight != mmapNamedLights.end()) {
				light = iterLight->second;
			}
			return light;
		}
		inline bool RemoveNamedLight(const string& name) { // the light iteself is still existent though, be careful.
			bool succ = false;
			map<string, Light * >::iterator iterLight = mmapNamedLights.find(name);
			if (iterLight != mmapNamedLights.end()) {
				succ = true;
				mmapNamedLights.erase(iterLight);
			}
			return succ;
		}
		inline bool HasNamedLight(const string& name) {
			bool has = false;
			map<string, Light * >::iterator iterLight = mmapNamedLights.find(name);
			if (iterLight != mmapNamedLights.end()) {
				has = true;
			}
			return has;
		}
		inline int GetNamedLightCount() const {
			return mmapNamedLights.size();
		}

		inline void Purge() {
			if (mpAmbientLight) {
				delete mpAmbientLight;
				mpAmbientLight = nullptr;
			}

			for (int i = 0; i < mvecLights.size(); ++i) {
				delete mvecLights[i];
				mvecLights[i] = nullptr;
			}
			mvecLights.clear();

			mmapNamedLights.clear();
		}
	};
}
