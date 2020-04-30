#pragma once

#include "GeometricObject.h"
#include "Material.h"

namespace LaplataRayTracer
{
	//
	class NormalShpere : public SimpleSphere
	{
	public:
		NormalShpere(Vec3f const& pos, float fRadius)
			: SimpleSphere(pos, fRadius) { }
		virtual ~NormalShpere() { }

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
		{
			if (SimpleSphere::HitTest(inRay, tmin, tmax, rec)) {

				// Here we just modify the color returned by super Class to shade it as the normal mapping
				rec.albedo.Set(0.5f*(rec.n.X() + 1.0f), 0.5f*(rec.n.Y() + 1.0f), 0.5f*(rec.n.Z() + 1.0f));

				return true;
			}

			return false;
		}

	};

	// It is really a good job I've done here
	// It's a typical example of replacing inheritance with the technique of integration one object inside another object.
	// You can see that from our previous implementation.
	// Texture will be added here, too.
	class MaterialObject : public GeometricObject
	{
	public:
		MaterialObject(GeometricObject *pProxyObject, Material *pMaterial, bool bObjAutoDelete = true, bool bMaterialAutoDelete = true)
		{
			mpProxyObject = pProxyObject;
			mpMaterial = pMaterial;

			mbObjAutoDelete = bObjAutoDelete;
			mbMaterialAutoDelete = bMaterialAutoDelete;
		}

		MaterialObject(MaterialObject const& other)
		{
			copy_constructor(other);
		}

		MaterialObject& operator=(MaterialObject const& other)
		{
			if (this == &other) { return *this; }

			copy_constructor(other);

			return *this;
		}

		virtual ~MaterialObject()
		{
			if (mbObjAutoDelete)
			{
				release_proxy_object();
			}

			if (mbMaterialAutoDelete)
			{
				release_material_object();
			}
		}

	public:
		virtual void *Clone()
		{
			return (GeometricObject *)(new MaterialObject((GeometricObject *)mpProxyObject->Clone(), (Material *)mpMaterial->Clone()));
		}

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
		{
			if (mpProxyObject && mpProxyObject->HitTest(inRay, tmin, tmax, rec))
			{
				rec.pMaterial = mpMaterial;

				return true;
			}

			return false;
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const
		{
			bool isHit = mpProxyObject->IntersectP(inRay, tvalue);
			return isHit;
		}

	public:
		virtual float Area() const
		{
			return mpProxyObject->Area();
		}

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding)
		{
			return mpProxyObject->GetBoundingBox(t0, t1, bounding);
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const
		{
			return mpProxyObject->GetNormal(rec);
		}

		virtual Vec3f RandomSamplePoint() const
		{
			return mpProxyObject->RandomSamplePoint();
		}

		virtual void Update(float t)
		{
			mpProxyObject->Update(t);
		}

		virtual bool IsCompound() const
		{
			return mpProxyObject->IsCompound();
		}

		virtual float PDFValue(Vec3f const& o, Vec3f const& v) const {
			return mpProxyObject->PDFValue(o, v);
		}

		virtual Vec3f RandomSampleDirection(Vec3f const& v) const
		{
			return mpProxyObject->RandomSampleDirection(v);
		}

	public:
		inline GeometricObject *GetProxyObject() { return mpProxyObject; }

		inline void SetMaterial(Material *pMaterial, bool autoDelete = true) {
			mbMaterialAutoDelete = autoDelete;

			if (mbMaterialAutoDelete)
			{
				release_material_object();
			}

			mpMaterial = pMaterial;
		}

		inline Material *GetMaterial() { return mpMaterial; }

	private:
		inline void release_proxy_object()
		{ 
			if (mpProxyObject != nullptr)
			{
				delete mpProxyObject;
				mpProxyObject = nullptr;
			}
		}

		inline void release_material_object()
		{
			if (mpMaterial != nullptr)
			{
				delete mpMaterial;
				mpMaterial = nullptr;
			}
		}

		inline void copy_constructor(MaterialObject const& other)
		{
			mbObjAutoDelete = other.mbObjAutoDelete;
			mbMaterialAutoDelete = other.mbMaterialAutoDelete;

			if (mbObjAutoDelete)
			{
				release_proxy_object();
				mpProxyObject = (GeometricObject *)other.mpProxyObject->Clone();
			}
			else
			{
				mpProxyObject = other.mpProxyObject;
			}

			if (mbMaterialAutoDelete)
			{
				release_material_object();
				mpMaterial = (Material *)other.mpMaterial->Clone();
			}
			else
			{
				mpMaterial = other.mpMaterial;
			}
		}

	protected:
		GeometricObject *mpProxyObject;
		Material *mpMaterial;

		bool mbObjAutoDelete;
		bool mbMaterialAutoDelete;

	};

	// I haven't commented This out in order that we can compare the current implementation with This old version.
	/*class MaterialSphere : public SimpleSphere
	{
	public:
		MaterialSphere(Vec3f const& pos, float fRadius, Material *pMaterial)
		: SimpleSphere(pos, fRadius), mpMaterial(pMaterial) { }
		virtual ~MaterialSphere()
		{
			if (mpMaterial)
			{
				delete mpMaterial;
				mpMaterial = nullptr;
			}
		}

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
		{
			if (SimpleSphere::HitTest(inRay, tmin, tmax, rec))
			{
				rec.pMaterial = mpMaterial;

				return true;
			}

			return false;
		}

	public:
		inline void SetMaterial(Material *pMaterial) {
			if (mpMaterial)
			{
				delete mpMaterial;
				mpMaterial = nullptr;
				mpMaterial = pMaterial;
			}
		}

		inline Material *GetMaterial() { return mpMaterial; }

	private:
		Material *	mpMaterial;
	};*/

}