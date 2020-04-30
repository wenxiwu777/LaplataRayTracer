#pragma once

#include "GeometricObject.h"
#include "Transform.h"

//
namespace LaplataRayTracer {
	class Instance : public GeometricObject {
	public:
		Instance();
		Instance(GeometricObject *ptrObj, bool autoDelete = false);
		Instance(const Instance& rhs);
		Instance& operator=(const Instance& rhs);
		virtual ~Instance();

	public:
		// From IConeable
		virtual void *Clone();

	public:
		// From Hitable
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec);
		virtual bool IntersectP(Ray const& inRay, float& tvalue) const;

	public:
		// From GeometricObject
		virtual float Area() const;
		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding);
		virtual Vec3f GetNormal(const HitRecord& rec) const;
		virtual Vec3f RandomSamplePoint() const;
		virtual void Update(float t);
		virtual bool IsCompound() const;
		virtual float PDFValue(Vec3f const& o, Vec3f const& v) const;
		virtual Vec3f RandomSampleDirection(Vec3f const& v) const;

	public:
		inline void SetObject(GeometricObject *ptrObj, bool autoDelete = false)
		{
			mpProxyObject = ptrObj;
			mbAutoDelete = autoDelete;
		}

		inline GeometricObject *GetObject()
		{
			return mpProxyObject;
		}

	public:
		void ComputeBoundingBox();

		void Translate(const Vec3f& p);
		void Translate(float x, float y, float z);

		void RotateX(float angleX);
		void RotateY(float angleY);
		void RotateZ(float angleZ);
		void Rotate(float angle, Vec3f& dir);

	private:
		void copy_constructor(Instance const& rhs);
		void release();

	private:
		GeometricObject *mpProxyObject;

		AABB	mBoundingBox;

	//	Matrix4x4 mInverseMatrix;
	//	Matrix4x4 mForwardMatrix;
		Transform mTransform;

		bool	mbAutoDelete;

	};
}
