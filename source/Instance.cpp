#include "Instance.h"

namespace LaplataRayTracer {

	//
	Instance::Instance() {
		mpProxyObject = nullptr;

		mTransform.SetIdentity();

		mbAutoDelete = false;

        mbTransformTexture = true;
	}

	Instance::Instance(GeometricObject *ptrObj, bool autoDelete) {
		mpProxyObject = ptrObj;
		mbAutoDelete = autoDelete;

		mTransform.SetIdentity();
	}

	Instance::Instance(const Instance& rhs) {
		copy_constructor(rhs);
	}

	Instance& Instance::operator=(const Instance& rhs) {
		if (this == &rhs) {
			return *this;
		}

		release();
		copy_constructor(rhs);

		return *this;
	}

	Instance::~Instance() {
		release();

	}

	//
	void *Instance::Clone() {
		return (Instance *)(new Instance(*this));
	}

	//
	bool Instance::HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec) {
		Ray invRay = mTransform.InverseApplyRay(inRay);

		if (mpProxyObject->HitTest(invRay, tmin, tmax, rec)) {
			rec.n = mTransform.ApplyNormal(rec.n);
			rec.n.MakeUnit();

			if (!mbTransformTexture) {
                rec.pt = inRay.O() + rec.t * inRay.D();
			}

			return true;
		}

		return false;
	}

	bool Instance::IntersectP(Ray const& inRay, float& tvalue) const {
		Ray invRay = mTransform.InverseApplyRay(inRay);

		if (mpProxyObject->IntersectP(invRay, tvalue)) {
			return true;
		}

		return false;
	}

	//
	float Instance::Area() const {
		float area_ = mpProxyObject->Area();
		return area_;
	}

	bool Instance::GetBoundingBox(float t0, float t1, AABB& bounding) {
	//	if (!mpProxyObject->GetBoundingBox(t0, t1, bounding)) {
	//		return false;
	//	}

	//	bounding = mBoundingBox;
		this->ComputeBoundingBox();
		bounding = mBoundingBox;

		return true;
	}

	Vec3f Instance::GetNormal(const HitRecord& rec) const {
		Vec3f n = mTransform.ApplyNormal(mpProxyObject->GetNormal(rec));
		return n;
	}

	Vec3f Instance::RandomSamplePoint() const {
		Vec3f ret_pt = mTransform.ApplyPoint(mpProxyObject->RandomSamplePoint());
		return ret_pt;
	}

	void Instance::Update(float t) {
		mpProxyObject->Update(t);
	}

	bool Instance::IsCompound() const {
		return (mpProxyObject->IsCompound());
	}

	float Instance::PDFValue(Vec3f const& o, Vec3f const& v) const
	{
		return (mpProxyObject->PDFValue(o, v));
	}

	Vec3f Instance::RandomSampleDirection(Vec3f const& v) const
	{
		return (mpProxyObject->RandomSampleDirection(v));
	}

	//
	void Instance::ComputeBoundingBox() {
		// Get the proxy object's bounding-box
		AABB bounding_box;
		mpProxyObject->GetBoundingBox(0.0f, 0.0f, bounding_box);

		// Generate 8 points that are upon the bounding-box as its corners
		Vec3f point_set[8] = { 0 };
		point_set[0][0] = bounding_box.mX0; point_set[0][1] = bounding_box.mY0; point_set[0][2] = bounding_box.mZ0;
		point_set[1][0] = bounding_box.mX1; point_set[1][1] = bounding_box.mY0; point_set[1][2] = bounding_box.mZ0;
		point_set[2][0] = bounding_box.mX1; point_set[2][1] = bounding_box.mY1; point_set[2][2] = bounding_box.mZ0;
		point_set[3][0] = bounding_box.mX0; point_set[3][1] = bounding_box.mY1; point_set[3][2] = bounding_box.mZ0;

		point_set[4][0] = bounding_box.mX0; point_set[4][1] = bounding_box.mY0; point_set[4][2] = bounding_box.mZ1;
		point_set[5][0] = bounding_box.mX1; point_set[5][1] = bounding_box.mY0; point_set[5][2] = bounding_box.mZ1;
		point_set[6][0] = bounding_box.mX1; point_set[6][1] = bounding_box.mY1; point_set[6][2] = bounding_box.mZ1;
		point_set[7][0] = bounding_box.mX0; point_set[7][1] = bounding_box.mY1; point_set[7][2] = bounding_box.mZ1;

		// Transform on those 8 points to get the transformed point set
		point_set[0] = mTransform.ApplyPoint(point_set[0]);
		point_set[1] = mTransform.ApplyPoint(point_set[1]);
		point_set[2] = mTransform.ApplyPoint(point_set[2]);
		point_set[3] = mTransform.ApplyPoint(point_set[3]);
		point_set[4] = mTransform.ApplyPoint(point_set[4]);
		point_set[5] = mTransform.ApplyPoint(point_set[5]);
		point_set[6] = mTransform.ApplyPoint(point_set[6]);
		point_set[7] = mTransform.ApplyPoint(point_set[7]);

		// Find the maximum and minimum points among those transformed point set
		// Deal with the maximum point
		float x0 = FLT_MAX;
		float y0 = FLT_MAX;
		float z0 = FLT_MAX;

		for (int i = 0; i <= 7; ++i) {
			if (point_set[i].X() < x0) {
				x0 = point_set[i].X();
			}
		}

		for (int i = 0; i <= 7; ++i) {
			if (point_set[i].Y() < y0) {
				y0 = point_set[i].Y();
			}
		}

		for (int i = 0; i <= 7; ++i) {
			if (point_set[i].Z() < z0) {
				z0 = point_set[i].Z();
			}
		}

		// Deal with the minimum value
		float x1 = -FLT_MAX;
		float y1 = -FLT_MAX;
		float z1 = -FLT_MAX;

		for (int i = 0; i <= 7; ++i) {
			if (point_set[i].X() > x1) {
				x1 = point_set[i].X();
			}
		}

		for (int i = 0; i <= 7; ++i) {
			if (point_set[i].Y() > y1) {
				y1 = point_set[i].Y();
			}
		}

		for (int i = 0; i <= 7; ++i) {
			if (point_set[i].Z() > z1) {
				z1 = point_set[i].Z();
			}
		}

		// Since we get the maximum and minimum points, then generate the new Bounding-box which is transformed according to them.
// 		bounding_box.mX0 = x0;
// 		bounding_box.mY0 = y0;
// 		bounding_box.mZ0 = z0;
// 		bounding_box.mX1 = x1;
// 		bounding_box.mY1 = y1;
// 		bounding_box.mZ1 = z1;

		mBoundingBox.mX0 = x0;
		mBoundingBox.mY0 = y0;
		mBoundingBox.mZ0 = z0;
		mBoundingBox.mX1 = x1;
		mBoundingBox.mY1 = y1;
		mBoundingBox.mZ1 = z1;

	}

	//
	void Instance::Translate(const Vec3f& p) {
		Transform transTranslate = Transform::Translate(p);
		mTransform = mTransform * transTranslate;
	}

	void Instance::Translate(float x, float y, float z) {
		Transform transTranslate = Transform::Translate(x, y, z);
		mTransform = mTransform * transTranslate;
	}

    void Instance::Scale(float a, float b, float c) {
	    Transform scale = Transform::Scale(a, b, c);
	    mTransform = mTransform * scale;
	}

	void Instance::RotateX(float angleX) {
		Transform transRotateX = Transform::RotateX(angleX);
		mTransform = mTransform * transRotateX;
	}

	void Instance::RotateY(float angleY) {
		Transform transRotateY = Transform::RotateY(angleY);
		mTransform = mTransform * transRotateY;
	}

	void Instance::RotateZ(float angleZ) {
		Transform transRotateZ = Transform::RotateZ(angleZ);
		mTransform = mTransform * transRotateZ;
	}

	void Instance::Rotate(float angle, Vec3f& dir) {
		Transform transRotate = Transform::Rotate(angle, dir);
		mTransform = mTransform * transRotate;
	}

    void Instance::EnableTextureTransform(bool enable) {
        mbTransformTexture = enable;
	}

	//
	void Instance::copy_constructor(Instance const& rhs)
	{
		if (rhs.mpProxyObject) {
			mpProxyObject = (Instance *)rhs.mpProxyObject->Clone();
		}
		else {
			mpProxyObject = rhs.mpProxyObject;
		}

		mbAutoDelete = rhs.mbAutoDelete;
		mBoundingBox = rhs.mBoundingBox;
		mTransform = rhs.mTransform;

	}

	void Instance::release()
	{
		if (mbAutoDelete) {
			delete mpProxyObject;
			mpProxyObject = nullptr;
		}
	}
}
