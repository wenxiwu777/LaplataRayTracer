#pragma once

#include <vector>

#include <float.h>
#include <iostream>

#include "Vec3.h"
#include "Ray.h"
#include "Material.h"
#include "Light.h"
#include "GeometricObject.h"
#include "ShadeObject.h"
#include "WorldObjects.h"
#include "Texture.h"
#include "SceneEnvrionment.h"

using std::vector;

namespace LaplataRayTracer
{
	// Rendering Context
	struct RTEnv
	{
		SceneObjects *	mpvecHitableObjs;
		SceneLights *	mpSceneLights;
        WorldEnvironment *  mpBackground;

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
			float tmax = FLT_MAX;
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
					if (t < tmax) {
						tmax = t;
				        hitTempRec.wpt = ray.O() + hitTempRec.t * ray.D();
						hitRec = hitTempRec;
						bHitAnything = true;
					}
				}
			}

			if (!bHitAnything) {
                return mRTEvn.mpBackground->Shade(ray);
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
			//    std::cout << "over path" << std::endl;
			//	g_Console.Write("over path\n");
				return BLACK;
			}

			float ft = 0.0f;
			float tmax = FLT_MAX;
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
					if (t < tmax) {
						tmax = t;
                        hitTempRec.wpt = ray.O() + hitTempRec.t * ray.D();
						hitRec = hitTempRec;
						bHitAnything = true;
					}
				}
			}

			if (!bHitAnything) {
                return mRTEvn.mpBackground->Shade(ray);
			}

			if (hitRec.pMaterial != nullptr) {
				hitRec.albedo = hitRec.pMaterial->Shade(ray, hitRec, *mRTEvn.mpvecHitableObjs, *mRTEvn.mpSceneLights);
				
				//
				Vec3f wi;
				Color3f CR;
				bool is_specular = hitRec.pMaterial->WhittedShade(ray, hitRec, wi, CR);
				if (is_specular) {
					Ray reflectedRay(hitRec.wpt, wi, ray.T());
					hitRec.albedo += CR * this->Run(reflectedRay, depth + 1, maxDepth) /* * Dot(hitRec.n, wi) */;
				}
				
				//
				WhittedRecord whittedRec;
				Color3f Lr;
				Color3f Lt;
				bool is_dielectric = hitRec.pMaterial->WhittedShade(ray, hitRec, whittedRec);
				if (is_dielectric) {
					if (whittedRec.feature == 1) {
						Ray reflectedRay(hitRec.wpt, whittedRec.wr, ray.T());
						Lr = this->Run(reflectedRay, depth + 1, maxDepth);
						hitRec.albedo += whittedRec.cf1.Filter(t) * Lr;
					}
					else if (whittedRec.feature == 2) {
						Ray reflectedRay(hitRec.wpt, whittedRec.wr, ray.T());
						Lr = this->Run(reflectedRay, depth + 1, maxDepth);
						hitRec.albedo += whittedRec.cf2.Filter(t) * Lr;
					}
					else if (whittedRec.feature == 3) {
						Ray reflectedRay(hitRec.wpt, whittedRec.wr, ray.T());
						Lr = whittedRec.fr * this->Run(reflectedRay, depth + 1, maxDepth) * std::fabs(Dot(hitRec.n, whittedRec.wr));
						hitRec.albedo += whittedRec.cf1.Filter(t) * Lr;

						Ray refractedRay(hitRec.wpt, whittedRec.wt, ray.T());
						Lt = whittedRec.ft * this->Run(refractedRay, depth + 1, maxDepth) * std::fabs(Dot(hitRec.n, whittedRec.wt));
						hitRec.albedo += whittedRec.cf2.Filter(t) * Lt;
					}
					else if (whittedRec.feature == 4) {
						Ray reflectedRay(hitRec.wpt, whittedRec.wr, ray.T());
						Lr = whittedRec.fr * this->Run(reflectedRay, depth + 1, maxDepth) * std::fabs(Dot(hitRec.n, whittedRec.wr));
						hitRec.albedo += whittedRec.cf2.Filter(t) * Lr;

						Ray refractedRay(hitRec.wpt, whittedRec.wt, ray.T());
						Lt = whittedRec.ft * this->Run(refractedRay, depth + 1, maxDepth) * std::fabs(Dot(hitRec.n, whittedRec.wt));
						hitRec.albedo += whittedRec.cf1.Filter(t) * Lt;
					}
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
			float tmax = FLT_MAX;
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
					if (t < tmax) {
						tmax = t;
                        hitTempRec.wpt = ray.O() + hitTempRec.t * ray.D();
						hitRec = hitTempRec;
						bHitAnything = true;
					}
				}
			}

			if (!bHitAnything) {
                return mRTEvn.mpBackground->Shade(ray);
			}

			if (hitRec.pMaterial != nullptr) {
				hitRec.albedo = hitRec.pMaterial->AreaLightShade(ray, hitRec, *mRTEvn.mpvecHitableObjs, *mRTEvn.mpSceneLights);

				Vec3f wi;
				Color3f CR;
				float pdf;
				bool glossy_reflective = hitRec.pMaterial->AreaLightShade(ray, hitRec, wi, CR, pdf);
				if (glossy_reflective) {
					Ray glossy_reflective_ray(hitRec.wpt, wi, ray.T());
					hitRec.albedo += CR * this->Run(glossy_reflective_ray, depth + 1, maxDepth);//  *Dot(hitRec.n, wi) / pdf;
				}
			}

			return hitRec.albedo;
		}
	};

	//
	// This one is based on <<RTGU>>, so the implementation is different from PathTracer.
	class GlobalTracer : public RayTracer
	{
	public:
		GlobalTracer()
		{

		}

		virtual ~GlobalTracer()
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
			float tmax = FLT_MAX;
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
					if (t < tmax) {
						tmax = t;
						hitTempRec.wpt = ray.O() + hitTempRec.t * ray.D();
						hitRec = hitTempRec;
						bHitAnything = true;
					}
				}
			}

			if (!bHitAnything) {
                return mRTEvn.mpBackground->Shade(ray);
			}

			if (hitRec.pMaterial != nullptr) {
				GlobalPathRecord globalRec;
				globalRec.depth = depth;
				if (hitRec.pMaterial->GlobalShade(ray, hitRec, globalRec)) {
					// Matte2Material
					// GlossyReflectiveMaterial
					if (globalRec.mat_type == 1 || globalRec.mat_type == 2) {
						if (depth == 0) {
							hitRec.albedo = hitRec.pMaterial->AreaLightShade(ray, hitRec, *mRTEvn.mpvecHitableObjs, *mRTEvn.mpSceneLights);
						}

						Ray scattered(hitRec.wpt, globalRec.reflected_dir, ray.T());
						Color3f temp = this->Run(scattered, depth + 1, maxDepth);
						hitRec.albedo += globalRec.albedo * temp * Dot(hitRec.n, globalRec.reflected_dir) / globalRec.pdf;
					}
					// ReflectiveMaterial
					else if (globalRec.mat_type == 3) {
						Ray scattered(hitRec.wpt, globalRec.reflected_dir, ray.T());

						if (depth == 0) {
                            Color3f temp = this->Run(scattered, depth + 2, maxDepth);
                            hitRec.albedo = globalRec.albedo * temp * Dot(hitRec.n, globalRec.reflected_dir) / globalRec.pdf;
						}
						else {
                            Color3f temp = this->Run(scattered, depth + 1, maxDepth);
							hitRec.albedo = globalRec.albedo * temp * Dot(hitRec.n, globalRec.reflected_dir) / globalRec.pdf;
						}
					}
					// Emissive2Material EmissiveTextureMaterial
					else if (globalRec.mat_type == 4) { // we reach the leaf node of the path tracing.
					     hitRec.albedo = globalRec.albedo;
					}
					// DielectricMaterial
					else if (globalRec.mat_type == 5) {
						hitRec.albedo += hitRec.pMaterial->AreaLightShade(ray, hitRec, *mRTEvn.mpvecHitableObjs, *mRTEvn.mpSceneLights);

						WhittedRecord whittedRec;
						Color3f Lr;
						Color3f Lt;
						bool is_dielectric = hitRec.pMaterial->WhittedShade(ray, hitRec, whittedRec);
						if (is_dielectric) {
							if (whittedRec.feature == 1) {
								Ray reflectedRay(hitRec.wpt, whittedRec.wr, ray.T());
								Lr = this->Run(reflectedRay, depth + 1, maxDepth);
								hitRec.albedo += whittedRec.cf1.Filter(hitRec.t) * Lr;
							}
							else if (whittedRec.feature == 2) {
								Ray reflectedRay(hitRec.wpt, whittedRec.wr, ray.T());
								Lr = this->Run(reflectedRay, depth + 1, maxDepth);
								hitRec.albedo += whittedRec.cf2.Filter(hitRec.t) * Lr;
							}
							else if (whittedRec.feature == 3) {
								Ray reflectedRay(hitRec.wpt, whittedRec.wr, ray.T());
								Lr = whittedRec.fr * this->Run(reflectedRay, depth + 1, maxDepth) * std::fabs(Dot(hitRec.n, whittedRec.wr));
								hitRec.albedo += whittedRec.cf1.Filter(hitRec.t) * Lr;

								Ray refractedRay(hitRec.wpt, whittedRec.wt, ray.T());
								Lt = whittedRec.ft * this->Run(refractedRay, depth + 1, maxDepth) * std::fabs(Dot(hitRec.n, whittedRec.wt));
								hitRec.albedo += whittedRec.cf2.Filter(hitRec.t) * Lt;
							}
							else if (whittedRec.feature == 4) {
								Ray reflectedRay(hitRec.wpt, whittedRec.wr, ray.T());
								Lr = whittedRec.fr * this->Run(reflectedRay, depth + 1, maxDepth) * std::fabs(Dot(hitRec.n, whittedRec.wr));
								hitRec.albedo += whittedRec.cf2.Filter(hitRec.t) * Lr;

								Ray refractedRay(hitRec.wpt, whittedRec.wt, ray.T());
								Lt = whittedRec.ft * this->Run(refractedRay, depth + 1, maxDepth) * std::fabs(Dot(hitRec.n, whittedRec.wt));
								hitRec.albedo += whittedRec.cf1.Filter(hitRec.t) * Lt;
							}
						}
					}
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
			float tmax = FLT_MAX;
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
					if (t < tmax) {
						tmax = t;
                        hitTempRec.wpt = ray.O() + hitTempRec.t * ray.D();
						hitRec = hitTempRec;
						//	return Color3f((hitRec.n.X()+1.0f)/2.0f, (hitRec.n.Y()+1.0f)/2, (hitRec.n.Z()+1.0f)/2);
						//	return hitRec.albedo;
						bHitAnything = true; 
					}
				}
			}

			if (!bHitAnything) {
                return mRTEvn.mpBackground->Shade(ray);
			}

			// Get the hit object's material, calculate the albedo, then Return it.
			// ... ...
			if (hitRec.pMaterial != nullptr) {
				Ray rayScatter;
				Color3f attenuation_albedo;
				Color3f Emissive_albedo = hitRec.pMaterial->Emissive(ray, hitRec);
				if (hitRec.pMaterial->PathShade(ray, hitRec, attenuation_albedo, rayScatter)) {
					hitRec.albedo = Emissive_albedo + attenuation_albedo * Run(rayScatter, depth + 1, maxDepth);
				}
				else {
					hitRec.albedo = Emissive_albedo;
				}
			}

			return hitRec.albedo;
		}
	};

	//
    class PathTracer_MIS : public RayTracer
	{
	public:
        PathTracer_MIS()
		{

		}

        virtual ~PathTracer_MIS()
		{

		}

	public:
		virtual Color3f Run(Ray& ray, int depth = 0, int maxDepth = 0)
		{
            if (depth > maxDepth) {
                return BLACK;
            }

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
                    hitTempRec.wpt = ray.O() + hitTempRec.t * ray.D();
					hitRec = hitTempRec;
					bHitAnything = true;
				}
			}

			if (!bHitAnything) {
			//	g_Console.Write("exit\n");
                return mRTEvn.mpBackground->Shade(ray);
			}

			if (hitRec.pMaterial != nullptr) {
                Color3f Emissive_albedo = hitRec.pMaterial->Emissive(ray, hitRec);
				ScatterRecord srec;
				if (hitRec.pMaterial->PathShade2(ray, hitRec, srec)) {
					if (srec.is_specular) {
						hitRec.albedo = srec.albedo * Run(srec.specular_ray, depth + 1, maxDepth);
					}
					else {
						LightPDF lightPDF(mpLightList, hitRec.wpt);
                        MixturePDF mixPDF(&lightPDF, srec.pPDF);
						Ray outRay;
                        outRay.Set(hitRec.wpt, mixPDF.Generate(), ray.T());
                        float pdf = mixPDF.Value(outRay.D());
                        hitRec.albedo = Emissive_albedo +
                                (srec.albedo * hitRec.pMaterial->PathShade2_pdf(ray, hitRec, outRay)
                            * Run(outRay, depth + 1, maxDepth)) / pdf;
					}
				}
				else {
					hitRec.albedo = Emissive_albedo;
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
