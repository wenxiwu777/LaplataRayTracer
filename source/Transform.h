#pragma once

#include <algorithm>

#include "Common.h"
#include "Vec3.h"
#include "Ray.h"
#include "AABB.h"
#include "Matrix.h"

//
namespace LaplataRayTracer
{
	//
	// Partion of the codes referenced <<pbrt>>
	class Transform {
	public:
		Matrix4x4	mMat;
		Matrix4x4	mInvMat;

	public:
		Transform() { }
		~Transform() { }

	public:
		//
		static Transform Translate(Vec3f const& p);
		static Transform Translate(float x, float y, float z);

		//
		static Transform Scale(Vec3f const& s);
		static Transform Scale(float a, float b, float c);

		//
		static Transform RotateX(float angleX);
		static Transform RotateY(float angleY);
		static Transform RotateZ(float angleZ);
		static Transform Rotate(float angle, Vec3f& dir);

		//
		static Transform LookAt(Vec3f& at, Vec3f& to, Vec3f& up);

	public:
		//
		void SetIdentity();

		//
		Vec3f ApplyPoint(Vec3f const& p) const;
		Vec3f ApplyVector(Vec3f const& v) const;
		Vec3f ApplyNormal(Vec3f const& n) const;
		Ray ApplyRay(Ray const& ray) const;

        	Vec3f InverseApplyPoint(Vec3f const& p) const;
        	Vec3f InverseApplyVector(Vec3f const& v) const;
		Ray InverseApplyRay(Ray const& ray) const;

		Transform operator*(Transform const& mat) const;

	private:
		void apply_point_impl(Matrix4x4 const& mat, Vec3f const& p, Vec3f& ret_pt) const;
		void apply_vector_impl(Matrix4x4 const& mat, Vec3f const& v, Vec3f& ret_v) const;
		void apply_normal_impl(Matrix4x4 const& mat, Vec3f const& n, Vec3f& ret_n) const;
		void apply_ray_impl(Matrix4x4 const& mat, Ray const& ray, Ray& ret_ray) const;

	};

	//
	// Z(W) axis aligned
	class UVM {
	public:
		Vec3f	mU;
		Vec3f	mV;
		Vec3f	mW;

	public:
		UVM() { }

	public:
		inline void BuildFromW(Vec3f const& n, Vec3f const& u) {
			mW = n;
			mV = Cross(mW, u);
			mV.MakeUnit();
			mU = Cross(mV, mW);
		}

		inline Vec3f World(Vec3f const& v) {
			Vec3f v_ = v.X() * mU + v.Y() * mV + v.Z() * mW;
			return v_;
		}

		inline Vec3f U() const { return mU; }
		inline Vec3f V() const { return mV; }
		inline Vec3f W() const { return mW; }
	};

	//
	class ONB {
	public:
		ONB() { }
		inline const Vec3f operator[](int i) const { return axis[i]; }
		inline Vec3f& operator[](int i) { return axis[i]; }
		inline Vec3f U() const { return axis[0]; }
		inline Vec3f V() const { return axis[1]; }
		inline Vec3f W() const { return axis[2]; }
		inline Vec3f Local(float a, float b, float c) const { return a*U() + b*V() + c*W(); }
		inline Vec3f Local2(Vec3f const& a) const {
			return a.X()*U() + a.Y()*V() + a.Z()*W();
		}
		inline void BuildFromW(Vec3f& w) {
			w.MakeUnit();
			axis[2] = w;
			Vec3f a;
			if (std::fabs(W().X()) > 0.9) {
				a = Vec3f(0.0f, 1.0f, 0.0f);
			}
			else {
				a = Vec3f(1.0f, 0.0f, 0.0f);
			}
			axis[1] = Cross(W(), a);
			axis[1].MakeUnit();
			axis[0] = Cross(W(), V());
		}
		Vec3f axis[3];

	};
}
