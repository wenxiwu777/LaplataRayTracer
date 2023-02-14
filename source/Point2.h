#pragma once

namespace LaplataRayTracer
{
	template<typename T>
	class Point2
	{
	public:
        T x;
        T y;

	public:
        Point2(void) : x(0), y(0) { }
        Point2(const T a) : x(a), y(a) { }
        Point2(const T a, const T b) : x(a), y(b) { }
        Point2(Point2<T> const& o) : x(o.x), y(o.y) { }
		~Point2() = default;

		inline T X() const { return x; }
		inline T Y() const { return y; }
        inline T operator[](const int n) const {
            if (n == 0) { return x; }
            if (n == 1) { return y; }
            return 0;
        }

		Point2<T>& operator=(Point2<T> const& rhs)
		{
			if (&rhs == this) return *this;
			x = rhs.x; y = rhs.y;
			return *this;
		}

		inline Point2<T> operator*(const T a) const
		{
			return Point2<T>(a * x, a * y);
		}

		inline float SqaureLength() const
		{
			return (x * x + y * y);
		}

		inline float Length() const
		{
			return std::sqrt(this->SqaureLength());
		}

		inline float SquareDistance(const Point2<T>& rhs) const
		{
			return ((x - rhs.x) * (x - rhs.x) + (y - rhs.y) * (y - rhs.y));
		}

		inline float Distance(const Point2<T>& rhs) const
		{
			return std::sqrt(this->SquareDistance(rhs));
		}

		// the following two may be help for sampling statistic
		inline float DeltaX(const Point2<T>& rhs) const
		{
			return abs(x - rhs.x);
		}

		inline float DeltaY(const Point2<T>& rhs) const
		{
			return abs(y - rhs.y);
		}

	};

	template<typename T>
	inline Point2<T> operator*(const T a, Point2<T> const p)
	{
		return Point2<T>(a * p.x, a * p.y);
	}

    template<typename T>
    inline Point2<T> operator-(Point2<T> const a, Point2<T> const b)
    {
        return Point2<T>(a.x - b.x, a.y - b.y);
    }

	typedef Point2<float>	Point2f;
	typedef Point2<double>	Point2d;
	typedef Point2<int>		Point2i;

}
