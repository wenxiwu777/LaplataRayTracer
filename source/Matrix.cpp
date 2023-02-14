#include <string.h>
#include <algorithm>
#include <cmath>
#include "Matrix.h"

namespace LaplataRayTracer {
	Matrix4x4 Matrix4x4::Inverse() {
		int indxc[4], indxr[4];
		int ipiv[4] = { 0, 0, 0, 0 };
		float minv[4][4];
		memcpy(minv, mData, 4 * 4 * sizeof(float));
		for (int i = 0; i < 4; i++) {
			int irow = 0, icol = 0;
			float big = 0.0f;
			// Choose pivot
			for (int j = 0; j < 4; j++) {
				if (ipiv[j] != 1) {
					for (int k = 0; k < 4; k++) {
						if (ipiv[k] == 0) {
							if (std::abs(minv[j][k]) >= big) {
								big = float(std::abs(minv[j][k]));
								irow = j;
								icol = k;
							}
						}
						else if (ipiv[k] > 1)
						{  }
					}
				}
			}
			++ipiv[icol];
			// Swap rows _irow_ and _icol_ for pivot
			if (irow != icol) {
				for (int k = 0; k < 4; ++k) std::swap(minv[irow][k], minv[icol][k]);
			}
			indxr[i] = irow;
			indxc[i] = icol;
			if (minv[icol][icol] == 0.0f) { }

			// Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
			float pivinv = 1.0f / minv[icol][icol];
			minv[icol][icol] = 1.0f;
			for (int j = 0; j < 4; j++) minv[icol][j] *= pivinv;

			// Subtract this row from others to zero out their columns
			for (int j = 0; j < 4; j++) {
				if (j != icol) {
					float save = minv[j][icol];
					minv[j][icol] = 0;
					for (int k = 0; k < 4; k++) minv[j][k] -= minv[icol][k] * save;
				}
			}
		}
		// Swap columns to reflect permutation
		for (int j = 3; j >= 0; j--) {
			if (indxr[j] != indxc[j]) {
				for (int k = 0; k < 4; k++)
					std::swap(minv[k][indxr[j]], minv[k][indxc[j]]);
			}
		}
		return Matrix4x4(minv);
	}

	//
    void Matrix4x4_Mul_Matrix_4_1(float mat4x4[4][4], float mat4x1[4][1], float mat_ret[4][1]) {
        mat_ret[0][0] = mat4x4[0][0] * mat4x1[0][0] + mat4x4[0][1] * mat4x1[1][0]
                + mat4x4[0][2] * mat4x1[2][0] + mat4x4[0][3] * mat4x1[3][0];
        mat_ret[1][0] = mat4x4[1][0] * mat4x1[0][0] + mat4x4[1][1] * mat4x1[1][0]
                + mat4x4[1][2] * mat4x1[2][0] + mat4x4[1][3] * mat4x1[3][0];
        mat_ret[2][0] = mat4x4[2][0] * mat4x1[0][0] + mat4x4[2][1] * mat4x1[1][0]
                + mat4x4[2][2] * mat4x1[2][0] + mat4x4[2][3] * mat4x1[3][0];
        mat_ret[3][0] = mat4x4[3][0] * mat4x1[0][0] + mat4x4[3][1] * mat4x1[1][0]
                + mat4x4[3][2] * mat4x1[2][0] + mat4x4[3][3] * mat4x1[3][0];
	}

    float Matrix1x4_Mul_Matrix_4_1(float mat1x4[1][4], float mat4x1[4][1]) {
	    float ret_value = mat1x4[0][0] * mat4x1[0][0] + mat1x4[0][1] * mat4x1[1][0]
	            + mat1x4[0][2] * mat4x1[2][0] + mat1x4[0][3] * mat4x1[3][0];
	    return ret_value;
	}
}