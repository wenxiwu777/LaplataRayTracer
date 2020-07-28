#pragma once

namespace LaplataRayTracer
{
	class Matrix4x4 {
	public:
		float mData[4][4];

	public:
		Matrix4x4() {
			SetIdentity();
		}

		Matrix4x4(float data[4][4]) {
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					mData[i][j] = data[i][j];
				}
			}
		}

		Matrix4x4(
			float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33
		) {
			mData[0][0] = m00; mData[0][1] = m01; mData[0][2] = m02; mData[0][3] = m03;
			mData[1][0] = m10; mData[1][1] = m11; mData[1][2] = m12; mData[1][3] = m13;
			mData[2][0] = m20; mData[2][1] = m21; mData[2][2] = m22; mData[2][3] = m23;
			mData[3][0] = m30; mData[3][1] = m31; mData[3][2] = m32; mData[3][3] = m33;
		}

		Matrix4x4(const Matrix4x4& rhs) {
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					mData[i][j] = rhs.mData[i][j];
				}
			}
		}

		Matrix4x4& operator=(const Matrix4x4& rhs) {
			if (this == &rhs) {
				return *this;
			}

			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					mData[i][j] = rhs.mData[i][j];
				}
			}

			return *this;
		}

		~Matrix4x4() { }

	public:
		inline bool operator==(const Matrix4x4& rhs) {
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					if (mData[i][j] != rhs.mData[i][j]) {
						return false;
					}
				}
			}

			return true;
		}

		inline bool operator!=(const Matrix4x4& rhs) {
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					if (mData[i][j] != rhs.mData[i][j]) {
						return true;
					}
				}
			}

			return false;
		}

		inline Matrix4x4 operator*(const Matrix4x4& rhs) {
			return Matrix4x4::Mul(*this, rhs);
		}

		inline Matrix4x4 operator/(float d) {
			return this->Divide(d);
		}

	public:
		// Access facilities
		inline void Set(int row, int col, float val) {
			mData[row][col] = val;
		}

		inline void SetRow(int row, float v0, float v1, float v2, float v3) {
			mData[row][0] = v0; mData[row][1] = v1; mData[row][2] = v2; mData[row][3] = v3;
		}

		inline void SetCol(int col, float v0, float v1, float v2, float v3) {
			mData[0][col] = v0; mData[1][col] = v1; mData[2][col] = v2; mData[3][col] = v3;
		}

	public:
		inline void SetIdentity() {
			mData[0][0] = mData[1][1] = mData[2][2] = mData[3][3] = 1.0;
			mData[0][1] = mData[0][2] = mData[0][3] = mData[1][0] = mData[1][2] = mData[1][3] = mData[2][0] =
				mData[2][1] = mData[2][3] = mData[3][0] = mData[3][1] = mData[3][2] = 0.0f;
		}

		inline Matrix4x4 Mul(const Matrix4x4& mat) {
			return Matrix4x4::Mul(*this, mat);
		}

		inline Matrix4x4 Divide(float d) {
			Matrix4x4 ret_mat(
				mData[0][0] / d, mData[0][1] / d, mData[0][2] / d, mData[0][3] / d,
				mData[1][0] / d, mData[1][1] / d, mData[1][2] / d, mData[1][3] / d,
				mData[2][0] / d, mData[2][1] / d, mData[2][2] / d, mData[2][3] / d,
				mData[3][0] / d, mData[3][1] / d, mData[3][2] / d, mData[3][3] / d
			);
			return ret_mat;
		}

		inline Matrix4x4 Transpose() {
			Matrix4x4 ret_mat(
				mData[0][0], mData[1][0], mData[2][0], mData[3][0],
				mData[0][1], mData[1][1], mData[2][1], mData[3][1],
				mData[0][2], mData[1][2], mData[2][2], mData[3][2],
				mData[0][3], mData[1][3], mData[2][3], mData[3][3]
			);
			return ret_mat;
		}

		Matrix4x4 Inverse();

	public:
		inline static Matrix4x4 Mul(const Matrix4x4& mat1, const Matrix4x4& mat2) {
			Matrix4x4 ret_mat;

			for (int m = 0; m < 4; ++m) {
				for (int n = 0; n < 4; ++n) {
					float temp_ = mat1.mData[m][0] * mat2.mData[0][n] + mat1.mData[m][1] * mat2.mData[1][n]
						+ mat1.mData[m][2] * mat2.mData[2][n] + mat1.mData[m][3] * mat2.mData[3][n];
					ret_mat.mData[m][n] = temp_;
				}
			}

			return ret_mat;
		}

	};

	//
	void Matrix4x4_Mul_Matrix_4_1(float mat4x4[4][4], float mat4x1[4][1], float mat_ret[4][1]);
	float Matrix1x4_Mul_Matrix_4_1(float mat1x4[1][4], float mat4x1[4][1]);

}
