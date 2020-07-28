#include "Common.h"
#include "Utility.h"

namespace LaplataRayTracer {

	//
	// RTMath
    bool RTMath::Quadratic(float a, float b, float c, float *t0, float *t1) {
        // Find quadratic discriminant
        double discrim = (double)b * (double)b - 4.0f * (double)a * (double)c;
        if (discrim < 0.0f) {
            return false;
        }
        double rootDiscrim = std::sqrt(discrim);

        // Compute quadratic _t_ values
        double q;
        if (b < 0.0f) {
            q = -0.5 * (b - rootDiscrim);
        } else {
            q = -0.5 * (b + rootDiscrim);
        }
        *t0 = q / a;
        *t1 = c / q;
        if (*t0 > *t1) {
            std::swap(*t0, *t1);
        }
        return true;
    }

    float RTMath::ErfInv(float x) {
        float w, p;
        x = RTMath::Clamp(x, -0.99999f, 0.99999f);
        w = -std::log((1.0f - x) * (1.0f + x));
        if (w < 5.0f) {
            w = w - 2.5f;
            p = 2.81022636e-08f;
            p = 3.43273939e-07f + p * w;
            p = -3.5233877e-06f + p * w;
            p = -4.39150654e-06f + p * w;
            p = 0.00021858087f + p * w;
            p = -0.00125372503f + p * w;
            p = -0.00417768164f + p * w;
            p = 0.246640727f + p * w;
            p = 1.50140941f + p * w;
        } else {
            w = std::sqrt(w) - 3.0f;
            p = -0.000200214257f;
            p = 0.000100950558f + p * w;
            p = 0.00134934322f + p * w;
            p = -0.00367342844f + p * w;
            p = 0.00573950773f + p * w;
            p = -0.0076224613f + p * w;
            p = 0.00943887047f + p * w;
            p = 1.00167406f + p * w;
            p = 2.83297682f + p * w;
        }
        return p * x;
    }

    float RTMath::Erf(float x) {
        // constants
        float a1 = 0.254829592f;
        float a2 = -0.284496736f;
        float a3 = 1.421413741f;
        float a4 = -1.453152027f;
        float a5 = 1.061405429f;
        float p = 0.3275911f;

        // Save the sign of x
        int sign = 1;
        if (x < 0.0f) {
            sign = -1;
        }
        x = std::abs(x);

        // A&S formula 7.1.26
        float t = 1.0f / (1.0f + p * x);
        float y = 1.0f - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * std::exp(-x * x);

        return sign * y;
    }

	//
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

    //
    //
    class One6thEquationHelper
    {
    public:
        One6thEquationHelper()
        {

        }

        ~One6thEquationHelper()
        {

        }

    public:
        static int SolveOne6th_equation(float c[7], float a, float b, float roots[6]);

    private:
        static int get_root_num(float c[7], float a, float b);
        static bool get_one6th_equation_derivative(float c[7], float init, float& fnc, float& drv);
        static bool solve_one6th_equation_by_newton_iteration(float c[7], float range[2], float tolerance, int& num, float& root);

    };

    //
    int One6thEquationHelper::SolveOne6th_equation(float c[7], float a, float b, float roots[6])
    {
        int num = 0;
        int num_root = 0;
        float root, range[2], temp;
        float left = a;
        float right = b;
        int total_num, interval_num;
        float tolerance = 1e-6;
        /*”total_num” for the whole interval; “interval_num” for every sub-interval*/
        int offset = 0;
        total_num = get_root_num(c, a, b);
        /*determine the number of distinct real roots in the interval*/
        if (total_num == 0)
        {
            return 0;
        }
        else
        {
            interval_num = total_num;
            for (int i = 1; i < (total_num + 1 + offset); i++)
            {
                while (interval_num != 1)
                {
                    /*to find a sub-interval with one real root in*/
                    if (left > right)
                    {
                        temp = left;
                        left = right;
                        right = temp;
                    }
                    right = (left + right) / 2;

                    interval_num = get_root_num(c, left, right);
                    if (interval_num == 0)
                    {
                        left = right;
                        right = b;

                        if (left == right)
                        {
                            /*s2: handle the situation that both left and right value reach the right end of the whole interval*/
                            break;
                        }
                    }
                    else if (std::fabs(left - right) < tolerance)
                    {
                        /*s1: left and right value are are so close that their values even don't change under sub-dividing*/
                        break;
                    }
                }
                if (interval_num == 0)
                {
                    /*s2: handle the situation that both left and right value reach the right end of the whole interval*/
                    break;
                }
                range[0] = left;
                range[1] = right;
                solve_one6th_equation_by_newton_iteration(c, range, tolerance, num, root);
                if (num != 0)
                {
                    /*if newton finds the root*/
                    roots[num_root] = root;
                    num_root += 1;
                    left = right;
                    right = b;
                }
                else if ((num == 0) && (std::fabs(left - right) < tolerance) && (interval_num >= 1))
                {
                    /*if newton doesn’t find the root, but the interval is small enough*/
                    /*s1: left and right value are are so close that their values even don't change under sub-dividing,
                        the interval_num may be more than one, so we modify "interval_num==1" to "interval_num>=1"*/
                    roots[num_root] = left;
                    num_root += 1;
                    left = right;
                    right = b;
                }
                else
                {
                    /*newton failed to find the root in regular interval, we need another time to find the root with smaller interval*/
                    offset++;
                    right = (left + right) / 2.0f;
                }

                interval_num = get_root_num(c, left, right);
            }
        }
        return num_root;
    }

    //
    int One6thEquationHelper::get_root_num(float c[7], float a, float b)
    {
        float temp1, temp2, aaa, bbb;
        float ee77[7][7] = { 0.0 };
        float fun_a[7] = { 0.0 };
        float fun_b[7] = { 0.0 };
        int change_num_a = 0;
        int change_num_b = 0;

        for(int i=0;i<7;++i){
            for(int j=0;j<7;++j){
                ee77[i][j]=0.0f;
            }
            fun_a[i]=0.0f;
            fun_b[i]=0.0f;
        }

        if (std::fabs(a) < 1e-3) {
            aaa = 1e-3;
        }
        else {
            aaa = a;
        }
        if (std::fabs(b) < 1e-3) {
            bbb = 1e-3;
        }
        else {
            bbb = b;
        }

        for (int i = 0; i < 6; i++) {
            //determine f0, f1
            ee77[0][i] = c[i];
            ee77[1][i] = (6 - i)*c[i];
        }
        ee77[0][6] = c[6];

        for (int i = 2; i < 6; i++) {
            //determine f2, f3, f4, f5
            temp1 = ee77[i - 2][0] / ee77[i - 1][0];
            temp2 = (ee77[i - 2][1] - temp1*ee77[i - 1][1]) / ee77[i - 1][0];
            for (int j = 0; j < (6 - i); j++) {
                ee77[i][j] = temp1*ee77[i - 1][j + 2] + temp2*ee77[i - 1][j + 1] - ee77[i - 2][j + 2];
                if (std::fabs(ee77[i][j]) < 1e-5) {
                    ee77[i][j] = 0.0f;
                }
            }
            ee77[i][6 - i] = temp2*ee77[i - 1][6 - i + 1] - ee77[i - 2][6 - i + 2];
            if (std::fabs(ee77[i][6 - i]) < 1e-5) {
                ee77[i][6 - i] = 0.0f;
            }

            if (RTMath::IsZero(ee77[i][0]) && RTMath::IsZero(ee77[i][1]) && RTMath::IsZero(ee77[i][2]) &&
                RTMath::IsZero(ee77[i][3]) && RTMath::IsZero(ee77[i][4]) && RTMath::IsZero(ee77[i][5]) &&
                RTMath::IsZero(ee77[i][6])) {
                break;
            }
        }
        if (std::fabs(ee77[5][0]) > 1e-5) {
            //determine f6
            temp1 = ee77[4][0] / ee77[5][0];
            temp2 = (ee77[4][1] - temp1*ee77[5][1]) / ee77[5][0];
            ee77[6][0] = temp2*ee77[5][1] - ee77[4][2];
            if (std::fabs(ee77[6][0]) < 1e-5) {
                ee77[6][0] = 0.0f;
            }
        }
        else {
            ee77[6][0] = 0.0f;
        }

        for (int j = 0; j < 7; j++) {
            fun_a[0] = fun_a[0] + ee77[0][j] * std::pow(aaa, (6 - j));
            fun_b[0] = fun_b[0] + ee77[0][j] * std::pow(bbb, (6 - j));
        }
        for (int i = 1; i < 7; i++) {
            for (int j = 0; j < (7 - i); j++) {
                fun_a[i] = fun_a[i] + ee77[i][j] * std::pow(aaa, ((6 - i) - j));
                fun_b[i] = fun_b[i] + ee77[i][j] * std::pow(bbb, ((6 - i) - j));
            }
            if ((fun_a[i] * fun_a[i - 1]) < 0) {
                change_num_a++;
            }
            if ((fun_b[i] * fun_b[i - 1]) < 0) {
                change_num_b++;
            }
        }
        int num = std::abs(change_num_a - change_num_b);
        return num;
    }

    bool One6thEquationHelper::get_one6th_equation_derivative(float c[7], float init, float& fnc, float& drv)
    {
        /*determine function value and derivative value at xxx*/
        fnc = 0.0f;
        drv = 0.0f;
        for (int i = 0; i < 7; i++) {
            fnc = fnc + c[i] * std::pow(init, (6 - i));
            if (i < 6) {
                drv = drv + (6 - i)*c[i] * std::pow(init, (5 - i));
            }
        }
        return true;
    }

    bool One6thEquationHelper::solve_one6th_equation_by_newton_iteration(float c[7], float range[2], float tolerance, int& num, float& root)
    {
        /*find the single root in the interval [x0[0], x0[1]] by newton iteration */
        float t_k, t_k1, ft_k, ft_d_k;
        int get = 0;

        for (int i = 0; i < 2; i++)
        {
            t_k = range[i];
            for (int k = 0; k < 20; k++)
            {
                if (!(std::isnan(t_k)))
                {
                    get_one6th_equation_derivative(c, t_k, ft_k, ft_d_k);
                    if ((!RTMath::IsZero(ft_d_k) && !std::isnan(ft_k)) && !std::isnan(ft_d_k))
                    {
                        t_k1 = t_k - ft_k / ft_d_k;
                        if (((std::fabs(t_k1) >= 1e-2) && (std::fabs((t_k1 - t_k) / t_k1) < tolerance)) ||
                            ((std::fabs(t_k1) < 1e-2) && (std::fabs((t_k1 - t_k)) < tolerance)))
                        {
                            if ((t_k1 > range[0]) && (t_k1 <= range[1]))
                            {
                                root = (std::fabs(t_k1) < (tolerance * 10.0f)) ? 0.0f : t_k1;
                                num = 1;
                                get++;
                            }
                            else
                            {
                                range[1] = (range[0] + range[1]) / 2.0f;
                            }
                            break;
                        }
                        else
                        {
                            t_k = t_k1;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
            if (get == 1)
            {
                break;
            }
        }
        num = get;
        return true;
    }

    //
    int RTMath::SolveOne6th_equation(float c[7], float a, float b, float roots[6])
    {
        int num_root = 0;
        num_root = One6thEquationHelper::SolveOne6th_equation(c, a, b, roots);

        return num_root;
    }
}
