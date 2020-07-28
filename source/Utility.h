#pragma once

#include <float.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace LaplataRayTracer
{
	// STRING
	class RTString {
	public:
		static void Split(const string& source, const string& pattern, vector<string>& result) {
			string strs = source + pattern;
			size_t pos = strs.find(pattern);
			size_t size = strs.size();
			while (pos != string::npos) {
				string x = strs.substr(0, pos);
				result.push_back(x);
				strs = strs.substr(pos + 1, size);
				pos = strs.find(pattern);
			}
		}
	};

	//
	class RTMath {
	public:
		inline
		static float Clamp(const float x, const float min, const float max) {
			return (x < min ? min : (x > max ? max : x));
		}

		inline
		static int Clamp(const int x, const int min, const int max) {
			return (x < min ? min : (x > max ? max : x));
		}

		template <typename T>
		inline static T Lerp(const T& a, const T& b, float p) {
		    return (a + p * (b - a));
		}

        inline static float Mix(float a, float b, float c) {
		    return ((1.0f - c) * a + c * b);
		}

        inline static float Mod(float a, const float b) {
            int n = (int)(a / b);

            a -= n * b;
            if (a < 0.0f)
                a += b;

            return (a);
        }

        inline static float SmoothStep(float a, float b, float x) {
            if (x < a)
                return 0.0f;

            if (x >= b)
                return 1.0f;

            double y = (x - a) / ( b - a);
            return (y * y * (3.0f - 2.0f * y));
        }

        inline static float SmoothPulse(float e0, float e1, float e2, float e3, float x) {
            return (SmoothStep(e0, e1, x) - SmoothStep(e2, e3, x));
        }

        inline static float SmoothPulseTrain(float e0, float e1, float e2, float e3, float period, float x) {
            return (SmoothPulse(e0, e1, e2, e3, Mod(x, period)));
        }

		inline
		static bool IsZero(float t) {
			return (t >= -FLT_EPSILON && t <= FLT_EPSILON);
		}

		template <typename T>
		inline static bool IsNan(T v) {
		    return !( v == v );
		}

        static bool Quadratic(float a, float b, float c, float *t0, float *t1);
        static float ErfInv(float x);
        static float Erf(float x);

		static void SolveQuadraticEquation(float a, float b, float c, int& num, float roots[2]);
		static void SolveCubicEquation(float a, float b, float c, float d, int& num, float roots[3]);
		static void SolveQuaraticEquation(float a, float b, float c, float d, float e, int& num, float roots[4]);

		static int SolveQuadric(float c[3], float s[2]);
		static int SolveCubic(float c[4], float s[3]);
		static int SolveQuartic(float c[5], float s[4]);

        static int SolveOne6th_equation(float c[7], float a, float b, float roots[6]);
	};
}
