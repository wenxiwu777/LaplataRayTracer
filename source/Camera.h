#pragma once

#include "Common.h"
#include "Vec3.h"
#include "Ray.h"
#include "Sampling.h"

namespace LaplataRayTracer
{
	//
	class Camera
	{
	public:
		Camera()
		{
			mWidth = 0;
			mHeight = 0;
			
			mTime0 = 0.0f;
			mTime1 = 0.0f;

            mEnableZoom = false;
            mZoomFactor = 1.0f;
		}
		virtual ~Camera() { }

	public:
		virtual void SetViewPlane(int w, int h) { mWidth = w; mHeight = h; }
		virtual void SetDistance(float fdist) { mfdist = fdist; }
		//	virtual void SetFOV(float fvfov) { mfvfov = fvfov; }
		virtual void SetEye(Vec3f const& vEye) { mEye = vEye; }
		virtual void SetLookAt(Vec3f const& vLookAt) { mLookAt = vLookAt; }
		virtual void SetUpVector(Vec3f const& vUp) { mUpVector = vUp; }
		virtual void SetExpTime(float time0, float time1) { mTime0 = time0; mTime1 = time1; }

		virtual int Width() const { return mWidth; }
		virtual int Height() const { return mHeight; }
		virtual float Distance() const { return mfdist; }
		//	virtual float FOV() const { return mfvfov; }
		virtual Vec3f Eye() const { return mEye; }
		virtual Vec3f LookAt() const { return mLookAt; }
		virtual Vec3f UpVector() const { return mUpVector; }

	public:
		virtual void Update() = 0;
		virtual bool GenerateRay(float x, float y, Ray& ray) = 0;

	public:
	    inline void EnableZoomFactor(bool enable) { mEnableZoom = enable; }
	    inline bool IsZoomMode() const { return mEnableZoom; }
	    inline void SetZoomFactor(float zoomFactor) { mZoomFactor = zoomFactor; }
	    inline float GetZoomFactor() const { return mZoomFactor; }

	protected:
		virtual float genRayTime() const
		{
			float ray_time = mTime0 + Random::frand48() * (mTime1 - mTime0);
			return ray_time;
		}

	protected:
		int mWidth;
		int mHeight;
		
		float mfdist; // If d is long enough, the ray will miss the object, t < 0 from the hit-test intersection equation.

		Vec3f	mW;
		Vec3f	mU;
		Vec3f	mV;

		Vec3f mEye;
		Vec3f mLookAt;
		Vec3f mUpVector;

		float mTime0;
		float mTime1;

		bool mEnableZoom;
		float mZoomFactor;
	};

	//
	class TestCamera : public Camera
	{
	public:
		TestCamera() { mfdist = 1.0f; }
		virtual ~TestCamera() { }

	public:
		virtual void Update()
		{
			float theta = mfvfov * PI_CONST / 180;
			float half_height = tan(theta / 2);
			float aspect = (float)mWidth / (float)mHeight;
			float half_width = aspect * half_height;
			vOrg = mEye;
			mW = MakeUnit(mEye - mLookAt);
			mU = MakeUnit(Cross(mUpVector, mW));
			mV = Cross(mW, mU);
			vPlaneStart = vOrg - half_width * mU - half_height * mV - mfdist * mW;
			vHor = (2 * half_width) * mU;
			vVer = (2 * half_height) * mV;
		}

		virtual bool GenerateRay(float x, float y, Ray& ray)
		{
			ray.Set(vOrg, vPlaneStart + x * vHor + y * vVer - vOrg, genRayTime());
			return true;
		}

	public:
		inline void SetFOV(float fvfov) { mfvfov = fvfov; }
		inline float FOV() const { return mfvfov; }

	private:
		float  mfvfov;

		Vec3f	vOrg;
		Vec3f	vPlaneStart;
		Vec3f	vHor;
		Vec3f	vVer;
	};

	//
	class OrthoCamera : public Camera
	{
	public:
		OrthoCamera() { }
		virtual ~OrthoCamera() { }

	public:
		virtual void Update()
		{
			mW = mEye - mLookAt;
		}

		virtual bool GenerateRay(float x, float y, Ray& ray)
		{
			Vec3f vDirInUVW = mW;
			vDirInUVW.MakeUnit();
			ray.Set(mEye, vDirInUVW, genRayTime());
			return true;
		}
	};

	//
	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera() { }
		virtual ~PerspectiveCamera() { }

	public:
		virtual void Update()
		{
			UpdateUVW();
		}

		virtual bool GenerateRay(float x, float y, Ray& ray)
		{
			Vec3f vDirInUVW = x * mU + y * mV - mfdist * mW;
			vDirInUVW.MakeUnit();

			ray.Set(mEye, vDirInUVW, genRayTime());

			return true;
		}

	protected:
		virtual void UpdateUVW()
		{
			mW = mEye - mLookAt;
			mW.MakeUnit();
			mU = Cross(mUpVector, mW);
			mU.MakeUnit();
			mV = Cross(mW, mU);

			// take care of the singularity by hardwiring in specific camera orientations
			if (mEye.X() == mLookAt.X() && mEye.Z() == mLookAt.Z() && mEye.Y() > mLookAt.Y()) { // camera looking vertically down
				mU = Vec3f(0.0f, 0.0f, 1.0f);
				mV = Vec3f(1.0f, 0.0f, 0.0f);
				mW = Vec3f(0.0f, 1.0f, 0.0f);
			}

			if (mEye.X() == mLookAt.X() && mEye.Z() == mLookAt.Z() && mEye.Y() < mLookAt.Y()) { // camera looking vertically up
				mU = Vec3f(1.0f, 0.0f, 0.0f);
				mV = Vec3f(0.0f, 0.0f, 1.0f);
				mW = Vec3f(0.0f, -1.0f, 0.0f);
			}
		}

	};

	//
	class FishEyeCamera : public PerspectiveCamera {
	public:
		FishEyeCamera()
			: mPsiPhi(180.0f) {

		}

		FishEyeCamera(float psiPhi)
			: mPsiPhi(psiPhi) {

		}

		virtual ~FishEyeCamera() {

		}

	public:
		virtual bool GenerateRay(float x, float y, Ray& ray) {
			Point2f pt_on_dev;
			pt_on_dev.x = 2.0f / mWidth * x;
			pt_on_dev.y = 2.0f / mHeight * y;

			float pt_square_len = pt_on_dev.SqaureLength();

			if (pt_square_len <= 1.0f) {
				float r_ = std::sqrt(pt_square_len);
				float theta_ = r_ * mPsiPhi * PI_ON_180;
				float sin_theta = std::sin(theta_);
				float cos_theta = std::cos(theta_);
				float sin_phi = pt_on_dev.Y() / r_;
				float cos_phi = pt_on_dev.X() / r_;

				Vec3f ray_dir = sin_theta * cos_phi * mU 
					+ sin_theta * sin_phi * mV
					- cos_theta * mW;
                ray_dir.MakeUnit();
				ray.Set(mEye, ray_dir, genRayTime());

				return true;
			}

			return false;
		}

	public:
		inline void SetPsiPhi(float psiPhi) { mPsiPhi = psiPhi; }

	private:
		float mPsiPhi;

	};

	//
	class ThinLenCamera : public PerspectiveCamera {
	public:
		ThinLenCamera()
			: mLenDiskRadius(2.0f), mFocusPlaneLen(40.0f), mpSampler(nullptr) {

		}

		ThinLenCamera(float lenDiskRadius, float focusPlaneLen)
			: mLenDiskRadius(lenDiskRadius), mFocusPlaneLen(focusPlaneLen), mpSampler(nullptr) {

		}

		virtual ~ThinLenCamera() {
			release_sampler();
		}

	public:
		virtual bool GenerateRay(float x, float y, Ray& ray) {
		//	Point2f pt_sp;
		//	Point2f pt_pp;
			Point2f pt_dp;
			Point2f pt_lp;

			//
			Point2f pt_focus_plane;
			pt_focus_plane.x = x * mFocusPlaneLen / mfdist;
			pt_focus_plane.y = y * mFocusPlaneLen / mfdist;

			//
			pt_dp = mpSampler->SampleFromUnitDisk();
			pt_lp = mLenDiskRadius * pt_dp;

			//
			Vec3f ray_o = mEye + pt_lp.X() * mU + pt_lp.Y() * mV;
			Vec3f ray_dir = (pt_focus_plane.X() - pt_lp.X()) * mU +
				(pt_focus_plane.Y() - pt_lp.Y()) * mV - mFocusPlaneLen * mW;
			ray_dir.MakeUnit();

			ray.Set(ray_o, ray_dir, genRayTime());

			return true;
		}

	public:
		inline void SetLenDiskRadius(float lenDiskRadius) { mLenDiskRadius = lenDiskRadius; }
		inline void SetFocusPlaneLen(float focusPlaneLen) { mFocusPlaneLen = focusPlaneLen; }

		inline void SetSampler(SamplerBase *sampler) {
			release_sampler();

			mpSampler = sampler;
			mpSampler->MapSamplesToUnitDisk();
		}

	private:
		inline void release_sampler() {
			if (mpSampler != nullptr) {
				delete mpSampler;
				mpSampler = nullptr;
			}
		}

	private:
		float mLenDiskRadius;
		float mFocusPlaneLen;
		SamplerBase *mpSampler;

	};

	//
	class PanoramicCamera : public PerspectiveCamera {
	public:
        PanoramicCamera()
        : mLambdaMax(360.0f), mPsiMax(180.0f) {

        }

        PanoramicCamera(float lambdaMax, float psiMax)
        : mLambdaMax(lambdaMax), mPsiMax(psiMax) {

        }

        virtual ~PanoramicCamera() {

        }

	public:
        virtual bool GenerateRay(float x, float y, Ray& ray) {
            Point2f ndc;
            ndc.x = 2.0f / (mZoomFactor * mWidth) * x;
            ndc.y = 2.0f / (mZoomFactor * mHeight) * y;

            float lambda = ndc.X() * mLambdaMax * PI_ON_180;
            float psi = ndc.Y() * mPsiMax * PI_ON_180;

            float phi = PI_CONST - lambda;
            float theta = 0.5f * PI_CONST - psi;

            float sin_phi = std::sin(phi);
            float cos_phi = std::cos(phi);
            float sin_theta = std::sin(theta);
            float cos_theta = std::cos(theta);

            Vec3f ray_dir = sin_theta * sin_phi * mU
                            + cos_theta * mV
                            + sin_theta * cos_phi * mW;
            ray_dir.MakeUnit();
            ray.Set(mEye, ray_dir, genRayTime());

            return true;
        }

	public:
	    inline void SetLambdaMax(float lambdaMax) {
            mLambdaMax = lambdaMax;
        }
        inline void SetPhiMax(float psiMax) {
            mPsiMax = psiMax;
        }

	private:
	    float mLambdaMax;
	    float mPsiMax;

	};

}
