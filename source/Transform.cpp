#include "Transform.h"

namespace LaplataRayTracer {

	//
	// Transformation series functions
	//
	//
	Transform Transform::Translate(Vec3f const& p) {
		return (Translate(p[0], p[1], p[2]));
	}

	Transform Transform::Translate(float x, float y, float z) {
		Transform trans;

		trans.mMat.SetCol(3, x, y, z, 1.0f);
		trans.mInvMat.SetCol(3, -x, -y, -z, 1.0f);

		return trans;
	}

	//
	Transform Transform::Scale(Vec3f const& s) {
		return (Scale(s[0], s[1], s[2]));
	}

	Transform Transform::Scale(float a, float b, float c) {
		Transform trans;

		trans.mMat.Set(0, 0, a);
		trans.mMat.Set(1, 1, b);
		trans.mMat.Set(2, 2, c);

		trans.mInvMat.Set(0, 0, 1.0f / a);
		trans.mInvMat.Set(1, 1, 1.0f / b);
		trans.mInvMat.Set(2, 2, 1.0f / c);

		return trans;
	}

	//
	Transform Transform::RotateX(float angleX) {
		Transform trans;

		float sin_theta = std::sin(ANG2RAD(angleX));
		float cos_theta = std::cos(ANG2RAD(angleX));

		trans.mMat.Set(1, 1, cos_theta);
		trans.mMat.Set(1, 2, -sin_theta);
		trans.mMat.Set(2, 1, sin_theta);
		trans.mMat.Set(2, 2, cos_theta);

		trans.mInvMat.Set(1, 1, cos_theta);
		trans.mInvMat.Set(1, 2, sin_theta);
		trans.mInvMat.Set(2, 1, -sin_theta);
		trans.mInvMat.Set(2, 2, cos_theta);

		return trans;
	}

	Transform Transform::RotateY(float angleY) {
		Transform trans;

		float sin_theta = std::sin(ANG2RAD(angleY));
		float cos_theta = std::cos(ANG2RAD(angleY));

		trans.mMat.Set(0, 0, cos_theta);
		trans.mMat.Set(0, 2, sin_theta);
		trans.mMat.Set(2, 0, -sin_theta);
		trans.mMat.Set(2, 2, cos_theta);

		trans.mInvMat.Set(0, 0, cos_theta);
		trans.mInvMat.Set(0, 2, -sin_theta);
		trans.mInvMat.Set(2, 0, sin_theta);
		trans.mInvMat.Set(2, 2, cos_theta);

		return trans;
	}

	Transform Transform::RotateZ(float angleZ) {
		Transform trans;

		float sin_theta = std::sin(ANG2RAD(angleZ));
		float cos_theta = std::cos(ANG2RAD(angleZ));

		trans.mMat.Set(0, 0, cos_theta);
		trans.mMat.Set(0, 1, -sin_theta);
		trans.mMat.Set(1, 0, sin_theta);
		trans.mMat.Set(1, 1, cos_theta);

		trans.mInvMat.Set(0, 0, cos_theta);
		trans.mInvMat.Set(0, 1, sin_theta);
		trans.mInvMat.Set(1, 0, -sin_theta);
		trans.mInvMat.Set(1, 1, cos_theta);

		return trans;
	}

	Transform Transform::Rotate(float angle, Vec3f& dir) {
		Transform trans;

		Vec3f dir_n = dir;
		dir_n.MakeUnit();

		float cos_theta = std::cos(ANG2RAD(angle));
		float sin_theta = std::sin(ANG2RAD(angle));

		trans.mMat.Set(0, 0, dir_n.X() * dir_n.X() + (1.0f - dir_n.X() * dir_n.X()) * cos_theta);
		trans.mMat.Set(0, 1, dir_n.X() * dir_n.Y() * (1.0f - cos_theta) - dir_n.Z() * sin_theta);
		trans.mMat.Set(0, 2, dir_n.X() * dir_n.Z() * (1.0f - cos_theta) + dir_n.Y() * sin_theta);
		trans.mMat.Set(0, 3, 0.0f);
 
		trans.mMat.Set(1, 0, dir_n.X() * dir_n.Y() * (1.0f - cos_theta) + dir_n.Z() * sin_theta);
		trans.mMat.Set(1, 1, dir_n.Y() * dir_n.Y() + (1.0f - dir_n.Y() * dir_n.Y()) * cos_theta);
		trans.mMat.Set(1, 2, dir_n.Y() * dir_n.Z() * (1.0f - cos_theta) - dir_n.X() * sin_theta);
		trans.mMat.Set(1, 3, 0.0f);
 
		trans.mMat.Set(2, 0, dir_n.X() * dir_n.Z() * (1.0f - cos_theta) - dir_n.Y() * sin_theta);
		trans.mMat.Set(2, 1, dir_n.Y() * dir_n.Z() * (1.0f - cos_theta) + dir_n.X() * sin_theta);
		trans.mMat.Set(2, 2, dir_n.Z() * dir_n.Z() + (1.0f - dir_n.Z() * dir_n.Z()) * cos_theta);
		trans.mMat.Set(2, 3, 0.0f);

		return trans;
	}

	//
	Transform Transform::LookAt(Vec3f& at, Vec3f& to, Vec3f& up) {
		Transform trans;

		Matrix4x4 mat_camera_to_world;

		mat_camera_to_world.Set(0, 3, at.X());
		mat_camera_to_world.Set(1, 3, at.Y());
		mat_camera_to_world.Set(2, 3, at.Z());
		mat_camera_to_world.Set(3, 3, 1.0f);

		Vec3f dir_ = to - at;
		dir_.MakeUnit();
		up.MakeUnit();
		if (Cross(up, dir_).Length() == 0.0f) {
			return trans;
		}

		Vec3f right_ = Cross(up, dir_);
		right_.MakeUnit();
		Vec3f new_up = Cross(dir_, right_);

		mat_camera_to_world.Set(0, 0, right_.X());
		mat_camera_to_world.Set(1, 0, right_.Y());
		mat_camera_to_world.Set(2, 0, right_.Z());
		mat_camera_to_world.Set(3, 0, 0.0f);

		mat_camera_to_world.Set(0, 1, new_up.X());
		mat_camera_to_world.Set(1, 1, new_up.Y());
		mat_camera_to_world.Set(2, 1, new_up.Z());
		mat_camera_to_world.Set(3, 1, 0.0f);

		mat_camera_to_world.Set(0, 2, dir_.X());
		mat_camera_to_world.Set(1, 2, dir_.Y());
		mat_camera_to_world.Set(2, 2, dir_.Z());
		mat_camera_to_world.Set(3, 2, 0.0f);

		trans.mMat = mat_camera_to_world.Inverse();
		trans.mInvMat = mat_camera_to_world;

		return trans;
	}

	//
	//
	//
	void Transform::SetIdentity() {
		mMat.SetIdentity();
		mInvMat.SetIdentity();

	}

	//
	// Apply*** transformation series functions
	//
	Vec3f Transform::ApplyPoint(Vec3f const& p) const {
		Vec3f ret_pt;
		apply_point_impl(mMat, p, ret_pt);
		return ret_pt;
	}

	Vec3f Transform::ApplyVector(Vec3f const& v) const {
		Vec3f ret_v;
		apply_vector_impl(mMat, v, ret_v);
		return ret_v;
	}

	Vec3f Transform::ApplyNormal(Vec3f const& n) const {
		Vec3f ret_n;
		apply_normal_impl(mInvMat, n, ret_n);
		return ret_n;
	}

	Ray Transform::ApplyRay(Ray const& ray) const {
		Ray ret_ray;
		apply_ray_impl(mMat, ray, ret_ray);
		return ret_ray;
	}

	Vec3f Transform::InverseApplyPoint(Vec3f const& p) const {
		Vec3f ret_pt;
		apply_point_impl(mInvMat, p, ret_pt);
		return ret_pt;
	}

	Vec3f Transform::InverseApplyVector(Vec3f const& v) const {
		Vec3f ret_vec;
		apply_vector_impl(mInvMat, v, ret_vec);
		return ret_vec;
	}

	Ray Transform::InverseApplyRay(Ray const& ray) const {
		Ray ret_ray;
		apply_ray_impl(mInvMat, ray, ret_ray);
		return ret_ray;
	}

	//
	inline void Transform::apply_point_impl(Matrix4x4 const& mat, Vec3f const& p, Vec3f& ret_pt) const {
		float x = mat.mData[0][0] * p.X() + mat.mData[0][1] * p.Y() + mat.mData[0][2] * p.Z() + mat.mData[0][3];
		float y = mat.mData[1][0] * p.X() + mat.mData[1][1] * p.Y() + mat.mData[1][2] * p.Z() + mat.mData[1][3];
		float z = mat.mData[2][0] * p.X() + mat.mData[2][1] * p.Y() + mat.mData[2][2] * p.Z() + mat.mData[2][3];
		float w = mat.mData[3][0] * p.X() + mat.mData[3][1] * p.Y() + mat.mData[3][2] * p.Z() + mat.mData[3][3];

		if (w == 1.0f) {
			ret_pt[0] = x; ret_pt[1] = y; ret_pt[2] = z;
		}
		else {
			ret_pt[0] = x / w; ret_pt[1] = y / w; ret_pt[2] = z / w;
		}
	}

	void Transform::apply_vector_impl(Matrix4x4 const& mat, Vec3f const& v, Vec3f& ret_v) const {
		float x = mat.mData[0][0] * v.X() + mat.mData[0][1] * v.Y() + mat.mData[0][2] * v.Z();
		float y = mat.mData[1][0] * v.X() + mat.mData[1][1] * v.Y() + mat.mData[1][2] * v.Z();
		float z = mat.mData[2][0] * v.X() + mat.mData[2][1] * v.Y() + mat.mData[2][2] * v.Z();

		ret_v[0] = x; ret_v[1] = y; ret_v[2] = z;
	}

	void Transform::apply_normal_impl(Matrix4x4 const& mat, Vec3f const& n, Vec3f& ret_n) const {
		float x = mat.mData[0][0] * n.X() + mat.mData[1][0] * n.Y() + mat.mData[2][0] * n.Z();
		float y = mat.mData[0][1] * n.X() + mat.mData[1][1] * n.Y() + mat.mData[2][1] * n.Z();
		float z = mat.mData[0][2] * n.X() + mat.mData[1][2] * n.Y() + mat.mData[2][2] * n.Z();

		ret_n[0] = x; ret_n[1] = y; ret_n[2] = z;
	}

	void Transform::apply_ray_impl(Matrix4x4 const& mat, Ray const& ray, Ray& ret_ray) const {
		Vec3f ray_o;
		Vec3f ray_d;

		apply_point_impl(mat, ray.O(), ray_o);
		apply_vector_impl(mat, ray.D(), ray_d);

		ret_ray.Set(ray_o, ray_d, ray.T());
	}

	//
	// *** NOTE: ***
	// in our render we adopt the following order when matrix multiply another operator(like point, vector, normal)
	// m00 m01		a
	//			*	b
	// m10 m11		c
	//
	// in pbrt, it's in reverse order
	// but the result matrix is identical, because they are transpose.
	//
	Transform Transform::operator*(Transform const& mat) const {
		Transform trans;

// PBRT approach:
//		trans.mMat = Matrix4x4::Mul(mMat, mat.mMat);
//		trans.mInvMat = Matrix4x4::Mul(mat.mInvMat, mInvMat);

		trans.mMat = Matrix4x4::Mul(mat.mMat, mMat);
		trans.mInvMat = Matrix4x4::Mul(mInvMat, mat.mInvMat);

		return trans;
	}
}
