#pragma once

namespace LaplataRayTracer
{
	template <typename T >
	class Vec3
	{
	public:
		T v[3];

		Vec3() { this->Set(T(0), T(0), T(0)); }
		Vec3(const T v)
		{
			this->Set(v, v, v);
		}
		Vec3(const T v0, const T v1, const T v2)
		{
			this->Set(v0, v1, v2);
		}

		inline void Set(const T v0, const T v1, const T v2)
		{
			v[0] = v0;
			v[1] = v1;
			v[2] = v2;
		}

		inline T operator[](const int index) const
		{
			return v[index];
		}

		inline T& operator[](const int index)
		{
			return v[index];
		}

		inline T X() const { return v[0]; }
		inline T Y() const { return v[1]; }
		inline T Z() const { return v[2]; }
		inline T R() const { return v[0]; }
		inline T G() const { return v[1]; }
		inline T B() const { return v[2]; }

		inline const Vec3& operator+() const { return *this; }
		inline Vec3 operator-() const { return Vec3(-v[0], -v[1], -v[2]); }

		inline Vec3<T>& operator=(Vec3<T> const& rhs)
		{
			if (&rhs == this) return *this;
			v[0] = rhs.X(); v[1] = rhs.Y(); v[2] = rhs.Z();
			return *this;
		}

		inline bool operator==(const Vec3& rhs)
		{
			return (v[0] == rhs.v[0] && v[1] == rhs.v[1] && v[2] == rhs.v[2]);
		}

		inline Vec3& operator+=(const Vec3& rhs)
		{
			v[0] += rhs[0];
			v[1] += rhs[1];
			v[2] += rhs[2];
			return *this;
		}
		inline Vec3& operator-=(const Vec3& rhs)
		{
			v[0] -= rhs[0];
			v[1] -= rhs[1];
			v[2] -= rhs[2];
			return *this;
		}
		inline Vec3& operator*=(const Vec3& rhs)
		{
			v[0] *= rhs[0];
			v[1] *= rhs[1];
			v[2] *= rhs[2];
			return *this;
		}
		inline Vec3& operator/=(const Vec3& rhs)
		{
			v[0] /= rhs[0];
			v[1] /= rhs[1];
			v[2] /= rhs[2];
			return *this;
		}
		inline Vec3& operator*=(const T f)
		{
			v[0] *= f;
			v[1] *= f;
			v[2] *= f;
			return *this;
		}
		inline Vec3& operator/=(const T f)
		{
			v[0] /= f;
			v[1] /= f;
			v[2] /= f;
			return *this;
		}
		inline Vec3 operator*(const T f)
		{
			return Vec3<T>(v[0] * f, v[1] * f, v[2] * f);
		}
		inline Vec3 operator/(const T f)
		{
			return Vec3<T>(v[0] / f, v[1] / f, v[2] / f);
		}
// 		inline Vec3 operator*(const float f)
// 		{
// 			return Vec3<T>(v[0] * f, v[1] * f, v[2] * f);
// 		}
// 		inline Vec3 operator/(const float f)
// 		{
// 			return Vec3<T>(v[0] / f, v[1] / f, v[2] / f);
// 		}

		inline T Length() const { return std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); }
		inline T SquareLength() const {
			return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
		}

		inline void MakeUnit()
		{
			T len = Length();
			v[0] /= len;
			v[1] /= len;
			v[2] /= len;
		}

		inline T Dot(const Vec3& rhs) const
		{
			T fDot = v[0] * rhs.v[0] + v[1] * rhs.v[1] + v[2] * rhs.v[2];
			return fDot;
		}

		inline Vec3 Cross(const Vec3& rhs)
		{
			return Vec3((v[1] * rhs[2] - v[2] * rhs[1]),
				-(v[0] * rhs[2] - v[2] * rhs[0]),
				(v[0] * rhs[1] - v[1] * rhs[0]));
		}

		inline float SquareDistance(const Vec3& rhs) const
		{
			return ((v[0] - rhs.X()) * (v[0] - rhs.X())
				+ (v[1] - rhs.Y()) * (v[1] - rhs.Y())
				+ (v[2] - rhs.Z()) * (v[2] - rhs.Z()));
		}

		inline float Distance(const Vec3& rhs) const
		{
			return std::sqrt(this->SquareDistance(rhs));
		}

	};

	//
	template<typename T >
	inline Vec3<T> operator+(const Vec3<T>& v0, const Vec3<T>& v1)
	{
		return Vec3<T>(v0[0] + v1[0], v0[1] + v1[1], v0[2] + v1[2]);
	}

	template<typename T >
	inline Vec3<T> operator-(const Vec3<T>& v0, const Vec3<T>& v1)
	{
		return Vec3<T>(v0[0] - v1[0], v0[1] - v1[1], v0[2] - v1[2]);
	}

	template<typename T >
	inline Vec3<T> operator*(const Vec3<T>& v0, const Vec3<T>& v1)
	{
		return Vec3<T>(v0[0] * v1[0], v0[1] * v1[1], v0[2] * v1[2]);
	}

	template<typename T >
	inline Vec3<T> operator*(const Vec3<T>& v, float f)
	{
		return Vec3<T>(v[0] * f, v[1] * f, v[2] * f);
	}

	template<typename T >
	inline Vec3<T> operator*(float f, const Vec3<T>& v)
	{
		return Vec3<T>(f * v[0], f * v[1], f * v[2]);
	}

	template<typename T >
	inline Vec3<T> operator/(const Vec3<T>& v0, const Vec3<T>& v1)
	{
		return Vec3<T>(v0[0] / v1[0], v0[1] / v1[1], v0[2] / v1[2]);
	}

	template<typename T >
	inline Vec3<T> operator/(const Vec3<T>& v, float f)
	{
		return Vec3<T>(v[0] / f, v[1] / f, v[2] / f);
	}

	template<typename T >
	inline T Dot(const Vec3<T>& v0, const Vec3<T>& v1)
	{
		return (v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2]);
	}


	template<typename T >
	inline Vec3<T> Cross(const Vec3<T>& v0, const Vec3<T>& v1)
	{
		return Vec3<T>((v0[1] * v1[2] - v0[2] * v1[1]),
			-(v0[0] * v1[2] - v0[2] * v1[0]),
			(v0[0] * v1[1] - v0[1] * v1[0]));
	}

	template<typename T >
	inline Vec3<T> MakeUnit(Vec3<T> v)
	{
		return v / v.Length();
	}

	//
	typedef Vec3<float> Vec3f;
	typedef Vec3<double> Vec3d;
	typedef Vec3<int> Vec3i;
	typedef Vec3f Color3f;
	typedef Vec3i Color3i;
	typedef Vec3<float> Point3f;
	typedef Vec3<double> Point3d;

#define WORLD_ORIGIN	Vec3f(0.0f, 0.0f, 0.0f)
#define AXIS_X_BIAS		Vec3f(1.0f, 0.0f, 0.0f)
#define AXIS_Y_BIAS		Vec3f(0.0f, 1.0f, 0.0f)
#define AXIS_Z_BIAS		Vec3f(0.0f, 0.0f, 1.0f)

#define BLACK			Color3f(0.0f, 0.0f, 0.0f)
#define WHITE			Color3f(1.0f, 1.0f, 1.0f)

}
