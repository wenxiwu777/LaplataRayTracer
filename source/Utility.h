#pragma once

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

		inline
		static bool IsZero(float t) {
			return (t >= -FLT_EPSILON && t <= FLT_EPSILON);
		}

		static void SolveQuadraticEquation(float a, float b, float c, int& num, float roots[2]);
		static void SolveCubicEquation(float a, float b, float c, float d, int& num, float roots[3]);
		static void SolveQuaraticEquation(float a, float b, float c, float d, float e, int& num, float roots[4]);

		static int SolveQuadric(float c[3], float s[2]);
		static int SolveCubic(float c[4], float s[3]);
		static int SolveQuartic(float c[5], float s[4]);

	};
}
