#pragma once

#include <vector>

#include <float.h>

#include "Vec3.h"
#include "Ray.h"
#include "Material.h"
#include "Light.h"
#include "GeometricObject.h"
#include "ShadeObject.h"
#include "WorldObjects.h"
#include "Texture.h"

using std::vector;

namespace LaplataRayTracer
{
	// Rendering Context
	struct RTEnv
	{
		SceneObjects *	mpvecHitableObjs;
		SceneLights *	mpSceneLights;
		Texture *		mpBackground;

	};

	//
	class RayTracer
	{
	public:
		RayTracer() { }
		virtual ~RayTracer() { }

	public:
		virtual Color3f Run(Ray& ray, int depath = 0, int maxDepath = 0) = 0;

	public:
		virtual void SetRTEnv(RTEnv env)
		{
			this->mRTEvn = env;
		}

	protected:
		RTEnv		mRTEvn;

	};

	//
	class RayCastTracer : public RayTracer
	{
	public:
		RayCastTracer() { }
		virtual ~RayCastTracer() { }

	public:
		virtual Color3f Run(Ray& ray, int depth = 0, int maxDepth = 0)
		{
			float ft = 0.0f;
		//	float tmax = FLT_MAX;
			float t = FLT_MAX;
			bool bHitAnything = false;
			HitRecord hitRec;
			HitRecord hitTempRec;
			int count = mRTEvn.mpvecHitableObjs->size();
			for (int i = 0; i < count; ++i)
			{
				bool bHit = (*mRTEvn.mpvecHitableObjs)[i]->HitTest(ray, ft, t, hitTempRec);
				if (bHit)
				{
				//	if (t < tmax) {
				//		tmax = t;
						hitRec = hitTempRec;
						bHitAnything = true;
				//	}
				}
			}

			if (!bHitAnything) {
				return mRTEvn.mpBackground->Sample(0.0f, MakeUnit<float>(ray.D()).Y());
			}

			if (hitRec.pMaterial != nullptr) {
				hitRec.albedo = hitRec.pMaterial->Shade(ray, hitRec, *mRTEvn.mpvecHitableObjs, *mRTEvn.mpSceneLights);
			}

			return hitRec.albedo;
		}
	};

	//
	class WhittedTracer : public RayTracer
	{
	public:
		WhittedTracer()
		{

		}

		virtual ~WhittedTracer()
		{

		}

	public:
		virtual Color3f Run(Ray& ray, int depth = 0, int maxDepth = 0)
		{
			if (depth > maxDepth)
			{
				return BLACK;
			}

			float ft = 0.0f;
		//	float tmax = FLT_MAX;
			float t = FLT_MAX;
			bool bHitAnything = false;
			HitRecord hitRec;
			HitRecord hitTempRec;
			int count = mRTEvn.mpvecHitableObjs->size();
			for (int i = 0; i < count; ++i)
			{
				bool bHit = (*mRTEvn.mpvecHitableObjs)[i]->HitTest(ray, ft, t, hitTempRec);
				if (bHit)
				{
			//		if (t < tmax) {
			//			tmax = t;
						hitRec = hitTempRec;
						bHitAnything = true;
			//		}
				}
			}

			if (!bHitAnything) {
				return mRTEvn.mpBackground->Sample(0.0f, MakeUnit<float>(ray.D()).Y());
			}

			if (hitRec.pMaterial != nullptr) {
				hitRec.albedo = hitRec.pMaterial->Shade(ray, hitRec, *mRTEvn.mpvecHitableObjs, *mRTEvn.mpSceneLights);

				//
				Vec3f wi;
				Color3f CR;
				bool reflective = hitRec.pMaterial->WhittedShade(ray, hitRec, wi, CR);
				if (reflective) {
					Ray reflectedRay(hitRec.pt, wi, ray.T());
					hitRec.albedo += CR * this->Run(reflectedRay, depth + 1, maxDepth) /* * Dot(hitRec.n, wi) */;
				}
			}

			return hitRec.albedo;
		}
	};

	//
	class AreaLightTracer : public RayTracer
	{
	public:
		AreaLightTracer()
		{

		}

		virtual ~AreaLightTracer()
		{

		}

	public:
		virtual Color3f Run(Ray& ray, int depth = 0, int maxDepth = 0)
		{
			if (depth > maxDepth)
			{
				return BLACK;
			}

			float ft = 0.0f;
		//	float tmax = FLT_MAX;
			float t = FLT_MAX;
			bool bHitAnything = false;
			HitRecord hitRec;
			HitRecord hitTempRec;
			int count = mRTEvn.mpvecHitableObjs->size();
			for (int i = 0; i < count; ++i)
			{
				bool bHit = (*mRTEvn.mpvecHitableObjs)[i]->HitTest(ray, ft, t, hitTempRec);
				if (bHit)
				{
			//		if (t < tmax) {
			//			tmax = t;
						hitRec = hitTempRec;
						bHitAnything = true;
			//		}
				}
			}

			if (!bHitAnything) {
				return mRTEvn.mpBackground->Sample(0.0f, MakeUnit<float>(ray.D()).Y());
			}

			if (hitRec.pMaterial != nullptr) {
				hitRec.albedo = hitRec.pMaterial->AreaLightShade(ray, hitRec, *mRTEvn.mpvecHitableObjs, *mRTEvn.mpSceneLights);

				Vec3f wi;
				Color3f CR;
				bool glossy_reflective = hitRec.pMaterial->AreaLightShade(ray, hitRec, wi, CR);
				if (glossy_reflective) {
					Ray glossy_reflective_ray(hitRec.pt, wi, ray.T());
					hitRec.albedo += CR * this->Run(glossy_reflective_ray, depth + 1, maxDepth); // * Dot(hitRec.n, wi) / pdf;
				}
			}

			return hitRec.albedo;
		}
	};

	//
	class PathTracer : public RayTracer
	{
	public:
		PathTracer() { }
		virtual ~PathTracer() { }

	public:
		virtual Color3f Run(Ray& ray, int depth = 0, int maxDepth = 0)
		{
			if (depth > maxDepth)
				return BLACK;

			// In fact, each object has its own KEpsilon Const value which represents its minimum t value
			// If I require it from the abstract base Class' function, like KEpsilon(), it will slow down our running,
			// so ft is not used currently.
			float ft = 0.0f;
		//	float tmax = FLT_MAX;
			float t = FLT_MAX;
			bool bHitAnything = false;
			HitRecord hitRec;
			HitRecord hitTempRec;
			int count = mRTEvn.mpvecHitableObjs->size();
			for (int i = 0; i < count; ++i)
			{
				bool bHit = (*mRTEvn.mpvecHitableObjs)[i]->HitTest(ray, ft, t, hitTempRec);
				if (bHit)
				{
		//			if (t < tmax) {
		//				tmax = t;
						hitRec = hitTempRec;
						//	return Color3f((hitRec.n.X()+1.0f)/2.0f, (hitRec.n.Y()+1.0f)/2, (hitRec.n.Z()+1.0f)/2);
						//	return hitRec.albedo;
						bHitAnything = true; 
		//			}
				}
			}

			if (!bHitAnything) {
				return mRTEvn.mpBackground->Sample(0.0f, MakeUnit<float>(ray.D()).Y());
			}

			// Get the hit object's material, calculate the albedo, then Return it.
			// ... ...
			if (hitRec.pMaterial != nullptr) {
				Ray rayScatter;
				Color3f attenuation_albedo;
				Color3f emisive_albedo = hitRec.pMaterial->Emisive(ray, hitRec);
				if (hitRec.pMaterial->PathShade(ray, hitRec, attenuation_albedo, rayScatter)) {
					hitRec.albedo = emisive_albedo + attenuation_albedo * Run(rayScatter, depth + 1, maxDepth);
				}
				else {
					hitRec.albedo = emisive_albedo;
				}
			}

			return hitRec.albedo;
		}
	};

	//
	class PathTracer_PBRS : public RayTracer
	{
	public:
		PathTracer_PBRS()
		{

		}

		virtual ~PathTracer_PBRS()
		{

		}

	public:
		virtual Color3f Run(Ray& ray, int depth = 0, int maxDepth = 0)
		{
			if (depth > maxDepth)
				return BLACK;

			float ft = 0.0f;
			float t = FLT_MAX;
			bool bHitAnything = false;
			HitRecord hitRec;
			HitRecord hitTempRec;
			int count = mRTEvn.mpvecHitableObjs->size();
			for (int i = 0; i < count; ++i)
			{
				bool bHit = (*mRTEvn.mpvecHitableObjs)[i]->HitTest(ray, ft, t, hitTempRec);
				if (bHit)
				{
					hitRec = hitTempRec;
					bHitAnything = true;
				}
			}

			if (!bHitAnything) {
			//	g_Console.Write("exit\n");
				return mRTEvn.mpBackground->Sample(0.0f, MakeUnit<float>(ray.D()).Y());
			}

			if (hitRec.pMaterial != nullptr) {
				Color3f emisive_albedo = hitRec.pMaterial->Emisive(ray, hitRec);
				ScatterRecord srec;
				if (hitRec.pMaterial->PathShade2(ray, hitRec, srec)) {
				//	g_Console.Write("pathshade2\n");
					if (srec.is_specular) {
						hitRec.albedo = srec.alebdo * Run(srec.specular_ray, depth + 1, maxDepth);
					}
					else {
						LightPDF lightPDF(mpLightList, hitRec.pt);
						MixturePDF mixPDF(&lightPDF, srec.pPDF);
						Ray outRay;
						outRay.Set(hitRec.pt, mixPDF.Generate(), ray.T());
					//	g_Console.Write("prepare to get mixture pdf\n");
						float pdf = mixPDF.Value(outRay.D());
					//	g_Console.Write("get mixture pdf\n");
						hitRec.albedo = emisive_albedo + (srec.alebdo * hitRec.pMaterial->PathShade2_pdf(ray, hitRec, outRay)
							* Run(outRay, depth + 1, maxDepth)) / pdf;
					}
				}
				else {
					hitRec.albedo = emisive_albedo;
				}
			}

			return hitRec.albedo;
		}

	public:
		inline void BindLightList(GeometricObject *lightList)
		{
			mpLightList = lightList;
		}

	private:
		GeometricObject *mpLightList;

	};

}
