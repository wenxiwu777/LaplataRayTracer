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

        inline static void GammaCorrection(Color3f& col, float gamma) {
            float r = std::pow(col.R(), gamma);
            float g = std::pow(col.G(), gamma);
            float b = std::pow(col.B(), gamma);
            col.Set(r, g, b);
        }

	};

}
