#pragma once

#include <algorithm>

#include "Vec3.h"
#include "Utility.h"

namespace LaplataRayTracer
{
	class ImageProc
	{
	public:
		ImageProc() { }
		~ImageProc() { }

	public:
		inline static void HDR_Operator_MaxToOne(Color3f& pixel)
		{
			float max_one = std::max<float>(std::max<float>(pixel.R(), pixel.G()), pixel.B());
			if (max_one > 1.0f)
			{
				pixel.Set(pixel.R() / max_one, pixel.G() / max_one, pixel.B() / max_one);
			}
		}

		inline static void HDR_Operator_ClampToOne(Color3f& pixel, Color3f const& mark)
		{
			if (pixel.R() > 1.0f || pixel.G() > 1.0f || pixel.B() > 1.0f)
			{
				pixel = mark;
			}
		}

        inline static void HDR_Operator_Gamma1(Color3f& col, float gamma) {
            float r = col.R();
            float g = col.G();
            float b = col.B();
            r = std::pow(r, gamma);
            g = std::pow(g, gamma);
            b = std::pow(b, gamma);
            col.Set(r, g, b);
        }

        inline static void HDR_Operator_Gamma2(Color3f& col, float gamma) {
            float r = col.R();
            float g = col.G();
            float b = col.B();
            r = r/(1.0f+r);
            g = g/(1.0f+g);
            b = b/(1.0f+b);
            r = std::pow(r, gamma);
            g = std::pow(g, gamma);
            b = std::pow(b, gamma);
            col.Set(r, g, b);
        }

        inline static void HDR_operator_Uncharted2Tonemap(Color3f& col, float gamma) {
            float r = col.R();
            float g = col.G();
            float b = col.B();
            Color3f exp_bias(r*2.0f, g*2.0f, b*2.0f);
            Color3f curr_col = uncharted_2_tonemap(exp_bias);
            Color3f temp_col = uncharted_2_tonemap(Color3f(11.2f, 11.2f, 11.2f));
            Color3f white_scale(1.0f / temp_col.R(), 1.0f / temp_col.G(), 1.0f / temp_col.B());
            Color3f color = curr_col * white_scale;
            r = std::pow(color.R(), gamma);
            g = std::pow(color.G(), gamma);
            b = std::pow(color.B(), gamma);
            col.Set(r, g, b);
        }

		inline static Color3f De_NAN(Color3f const& col) {
			Color3f col_temp = col;

//			if (!(col_temp[0] == col_temp[0])) {
//				col_temp[0] = 0.0f;
//			}
//			if (!(col_temp[1] == col_temp[1])) {
//				col_temp[1] = 0.0f;
//			}
//			if (!(col_temp[2] == col_temp[2])) {
//				col_temp[2] = 0.0f;
//			}

            if (RTMath::IsNan(col_temp[0])) {
                col_temp[0] = 0.0f;
            }
            if (RTMath::IsNan(col_temp[1])) {
                col_temp[1] = 0.0f;
            }
            if (RTMath::IsNan(col_temp[2])) {
                col_temp[2] = 0.0f;
            }

			return col_temp;
		}

    private:
        inline static Color3f uncharted_2_tonemap(const Color3f& x) {
            static const float A = 0.15f;
            static const float B = 0.50f;
            static const float C = 0.10f;
            static const float D = 0.20f;
            static const float E = 0.02f;
            static const float F = 0.30f;
            static const float W = 11.2f;

            return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
        }

	};

}
