#include "Common.h"
#include "Utility.h"

namespace LaplataRayTracer {

	//
	// RTMath
	void RTMath::SolveQuadraticEquation(float a, float b, float c, int& num, float roots[2]) {
		if (RTMath::IsZero(a)) {
			if (RTMath::IsZero(b)) {
				num = 0;
			}
			else {
				roots[0] = -c / b;
				num = 1;
			}
		}
		else
		{
			float d = b * b - 4.0f * a * c;
			if (d < 0.0f) {
				num = 0;
			}
			else {
				roots[0] = (-b - std::sqrt(d)) / (2.0f * a);
				roots[1] = (-b + std::sqrt(d)) / (2.0f * a);
				num = 2;
			}
		}
	}

	void RTMath::SolveCubicEquation(float a, float b, float c, float d, int& num, float roots[3]) {
		
		if (RTMath::IsZero(a)) {
			float roots_quadratic[2] = { 0.0f };
			RTMath::SolveQuadraticEquation(b, c, d, num, roots_quadratic);
			roots[0] = roots_quadratic[0];
			roots[1] = roots_quadratic[1];
		}
		else {
			float A = b*b-3.0f*a*c;
			float B = b*c-9.0f*a*d;
			float C = c*c-3.0f*b*d;
			float delta = B*B-4.0f*A*C;

			if (RTMath::IsZero(A-B) && RTMath::IsZero(A)) {
				if (!RTMath::IsZero(a)) {
					roots[0] = -b/(3.0f*a);
				}
				else {
					if (!RTMath::IsZero(b)) {
						roots[0] = -c/b;
					}
					else {
						if (!RTMath::IsZero(c)) {
							roots[0] = -3.0f*d/c;
						}
					}
				}
				roots[1] = roots[0];
				roots[2] = roots[0];
				num = 3;
			}
			else if (delta>0.0f) {
				float y1 = A*b+(3.0f*a/2.0f)*(-B+sqrt(delta));
				float y2 = A*b+(3.0f*a/2.0f)*(-B-sqrt(delta));
				float pow_y1, pow_y2;

				if (y1<0.0f) {
					pow_y1 = -std::pow(-y1, 1.0f/3.0f);
				}
				else {
					pow_y1 = std::pow(y1, 1.0f/3.0f);
				}

				if (y2<0.0f) {
					pow_y2 = -std::pow(-y2, 1.0f/3.0f);
				}
				else {
					pow_y2 = std::pow(y2, 1.0f/3.0f);
				}

				roots[0] = (-b-pow_y1-pow_y2)/(3.0f*a);
				num = 1;
			}
			else if (RTMath::IsZero(delta)) {
				float K = B/A;
				roots[0] = -b/a+K;
				roots[1] = -K/2;
				roots[2] = -K/2;
				num = 3;
			}
			else if (delta<0.0f) {
				float theta = std::acos((2.0f*A*b-3.0f*a*B)/(2.0f*std::pow(A, 1.5f)));
				roots[0] = (-b-2.0f*std::sqrt(A)*std::cos(theta/3.0f))/(3.0f*a);
				roots[1] = (-b+std::sqrt(A)*(std::cos(theta/3.0f)+std::sqrt(3.0f)*std::sin(theta/3.0f)))/(3.0f*a);
				roots[2] = (-b+std::sqrt(A)*(std::cos(theta/3.0f)-std::sqrt(3.0f)*std::sin(theta/3.0f)))/(3.0f*a);
				num = 3;
			}
		}
	}

	void RTMath::SolveQuaraticEquation(float a, float b, float c, float d, float e, int& num, float roots[4]) {
		if (RTMath::IsZero(a)) {
			float roots_cubic[3] = { 0.0f };
			RTMath::SolveCubicEquation(b, c, d, e, num, roots_cubic);
			for (int i = 0; i < num; ++i) {
				roots[i] = roots_cubic[i];
			}
		}
		else {
			float b1 = b/a;
			float c1 = c/a;
			float d1 = d/a;
			float e1 = e/a;
			if (RTMath::IsZero(b1) && RTMath::IsZero(c1) && RTMath::IsZero(d1)) {
				if (e1 > 0.0f) {
					num = 0;
				}
				else {
					roots[0] = std::sqrt(std::sqrt(-e1));
					roots[1] = -std::sqrt(std::sqrt(-e1));
					num = 2;
				}
			}
			else {
				float roots_cubic[3] = { 0.0f };
				int num_ = 0;
				RTMath::SolveCubicEquation(-1.0, c1, 4 * e1 - b1*d1, d1*d1 + e1*b1*b1 - 4.0f*e1*c1, num_, roots_cubic);
				float y = roots_cubic[0];
				float B1, B2, C1, C2;
				if (RTMath::IsZero(b1*b1-4*c1+4*y)) {
					B1 = b/2;
					B2 = b/2;
					C1 = y/2;
					C2 = y/2;
				}
				else {
					B1 = b/2.0f-std::sqrt(b1*b1-4.0f*c1+4.0f*y)/2.0f;
					B2 = b/2.0f+std::sqrt(b1*b1-4.0f*c1+4.0f*y)/2.0f;
					C1 = y/2.0f-(b1*y-2.0f*d1)/(2.0f*std::sqrt(b1*b1-4.0f*c1+4.0f*y));
					C2 = y/2.0f+(b1*y-2.0f*d1)/(2.0f*std::sqrt(b1*b1-4.0f*c1+4.0f*y));
				}
				float roots1[2] = { 0.0f };
				float roots2[2] = { 0.0f };
				int num0 = 0, num1 = 0;
				RTMath::SolveQuadraticEquation(1.0f, B1, C1, num0, roots1);
				RTMath::SolveQuadraticEquation(1.0f, B2, C2, num1, roots2);
				for (int i = 0; i < num0; ++i) {
					roots[i] = roots1[i];
				}
				for (int j = 0; j < num1; ++j) {
					roots[num0 + j] = roots2[j];
				}
				num = num0 + num1;
			}
		}
	}

	int RTMath::SolveQuadric(float c[3], float s[2])
	{
		float p, q, D;

		p = c[1] / (2 * c[2]);
		q = c[0] / c[2];

		D = p * p - q;

		if (IsZero(D)) {
			s[0] = -p;
			return 1;
		}
		else if (D > 0) {
			float sqrt_D = sqrt(D);

			s[0] = sqrt_D - p;
			s[1] = -sqrt_D - p;
			return 2;
		}

		return 0;
	}

	int RTMath::SolveCubic(float c[4], float s[3])
	{
		int     i, num;
		float  sub;
		float  A, B, C;
		float  sq_A, p, q;
		float  cb_p, D;

		A = c[2] / c[3];
		B = c[1] / c[3];
		C = c[0] / c[3];

		sq_A = A * A;
		p = 1.0 / 3 * (-1.0 / 3 * sq_A + B);
		q = 1.0 / 2 * (2.0 / 27 * A * sq_A - 1.0 / 3 * A * B + C);

		/* use Cardano's formula */
		cb_p = p * p * p;
		D = q * q + cb_p;

		if (IsZero(D)) {
			if (IsZero(q)) { /* one triple solution */
				s[0] = 0;
				num = 1;
			}
			else { /* one single and one float solution */
				float u = cbrt(-q);
				s[0] = 2 * u;
				s[1] = -u;
				num = 2;
			}
		}
		else if (D < 0) { /* Casus irreducibilis: three real solutions */
			float phi = 1.0 / 3 * acos(-q / sqrt(-cb_p));
			float t = 2 * sqrt(-p);

			s[0] = t * cos(phi);
			s[1] = -t * cos(phi + PI_CONST / 3);
			s[2] = -t * cos(phi - PI_CONST / 3);
			num = 3;
		}
		else { /* one real solution */
			float sqrt_D = sqrt(D);
			float u = cbrt(sqrt_D - q);
			float v = -cbrt(sqrt_D + q);

			s[0] = u + v;
			num = 1;
		}

		/* resubstitute */

		sub = 1.0 / 3 * A;

		for (i = 0; i < num; ++i)
			s[i] -= sub;

		return num;
	}

	int RTMath::SolveQuartic(float c[5], float s[4])
	{
		float  coeffs[4];
		float  z, u, v, sub;
		float  A, B, C, D;
		float  sq_A, p, q, r;
		int     i, num;

		/* normal form: x^4 + Ax^3 + Bx^2 + Cx + D = 0 */

		A = c[3] / c[4];
		B = c[2] / c[4];
		C = c[1] / c[4];
		D = c[0] / c[4];

		/*  substitute x = y - A/4 to eliminate cubic term:
		x^4 + px^2 + qx + r = 0 */

		sq_A = A * A;
		p = -3.0 / 8 * sq_A + B;
		q = 1.0 / 8 * sq_A * A - 1.0 / 2 * A * B + C;
		r = -3.0 / 256 * sq_A*sq_A + 1.0 / 16 * sq_A*B - 1.0 / 4 * A*C + D;

		if (IsZero(r)) {
			/* no absolute term: y(y^3 + py + q) = 0 */

			coeffs[0] = q;
			coeffs[1] = p;
			coeffs[2] = 0;
			coeffs[3] = 1;

			num = SolveCubic(coeffs, s);

			s[num++] = 0;
		}
		else {
			/* solve the resolvent cubic ... */

			coeffs[0] = 1.0 / 2 * r * p - 1.0 / 8 * q * q;
			coeffs[1] = -r;
			coeffs[2] = -1.0 / 2 * p;
			coeffs[3] = 1;

			(void)SolveCubic(coeffs, s);

			/* ... and take the one real solution ... */

			z = s[0];

			/* ... to build two quadric equations */

			u = z * z - r;
			v = 2 * z - p;

			if (IsZero(u))
				u = 0;
			else if (u > 0)
				u = sqrt(u);
			else
				return 0;

			if (IsZero(v))
				v = 0;
			else if (v > 0)
				v = sqrt(v);
			else
				return 0;

			coeffs[0] = z - u;
			coeffs[1] = q < 0 ? -v : v;
			coeffs[2] = 1;

			num = SolveQuadric(coeffs, s);

			coeffs[0] = z + u;
			coeffs[1] = q < 0 ? v : -v;
			coeffs[2] = 1;

			num += SolveQuadric(coeffs, s + num);
		}

		/* resubstitute */

		sub = 1.0 / 4 * A;

		for (i = 0; i < num; ++i)
			s[i] -= sub;

		return num;
	}
}
