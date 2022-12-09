#pragma once
#include <iostream>
#include <vector>

#include "Common.h"
#include "Vec3.h"
#include "Random.h"
#include "HitRecord.h"
#include "ICloneable.h"
#include "ImageTextureDesc.h"
#include "ResourcePool.h"
#include "Transform.h"
#include "Noise.h"
#include "ImageTextureMapping.h"

namespace LaplataRayTracer
{
	class Texture : public ICloneable
	{
	public:
		Texture() { }
		virtual ~Texture() { }
	
	public:
		virtual void *Clone() = 0;

	public:
		virtual Color3f GetTextureColor(HitRecord& hitRec) const = 0;
		virtual Color3f Sample(float u, float v) const = 0;
		virtual Color3f GetTexelColor(int texelX, int texelY) const = 0;

	};

	// Constant texture
	class ConstantTexture : public Texture
	{
	public:
		ConstantTexture() : mColor(1.0f, 1.0f, 1.0f) { }
		ConstantTexture(Color3f const& c) : mColor(c) { }
		ConstantTexture(float r, float g, float b) : mColor(r, g, b) { }
		virtual ~ConstantTexture() { }

	public:
		virtual void *Clone() { return (Texture *)(new ConstantTexture(mColor)); }

	public:
		virtual Color3f GetTextureColor(HitRecord& hitRec) const
		{
			return mColor;
		}

		virtual Color3f Sample(float u, float v) const
		{
			return mColor;
		}

		virtual Color3f GetTexelColor(int texelX, int texelY) const
		{
			return mColor;
		}

	public:
		inline void Set(Color3f const& c) { mColor = c; }
		inline Color3f Get(void) const { return mColor; }

	private:
		Color3f mColor;

	};

	//
	class RandomTexture : public Texture
	{
	public:
		RandomTexture() { }
		virtual ~RandomTexture() { }

	public:
		virtual void *Clone() { return (Texture *)(new RandomTexture); }

	public:
		virtual Color3f GetTextureColor(HitRecord& hitRec) const
		{
			return Color3f(Random::frand48(), Random::frand48(), Random::frand48());
		}

		virtual Color3f Sample(float u, float v) const
		{
			static int n = 0;
			++n;
			if (n < 8000)
			{
				return Color3f(1.0f, 1.0f, 1.0f);
			}
			else if (n >= 8000 && n < 11000)
			{
				return Color3f(0.0f, 0.0f, 0.0f);
			}
			else if (n >= 11000)
			{
				n = 0;
				return Color3f(1.0f, 1.0f, 1.0f);
			}

			return Color3f(0.0f, 0.0f, 0.0f);
		}

		virtual Color3f GetTexelColor(int texelX, int texelY) const
		{
			return Color3f(Random::frand48(), Random::frand48(), Random::frand48());;
		}
	};

	//
	class PlaneCheckerTexture : public Texture
	{
	public:
		PlaneCheckerTexture()
		{
			mPlaneOritention = PLANE_XOZ;
			mfSize = 20.0f;
			mfLineWidth = 0.0;
			mColorA.Set(1.0f, 1.0f, 1.0f);
			mColorB.Set(0.0f, 0.0f, 0.0f);
			mLineColor.Set(0.0f, 0.0f, 0.0f);

		}

		PlaneCheckerTexture(EPlaneOritention planeOritention, float size, float lineWidth, Color3f colorA, Color3f colorB, Color3f lineColor)
			: mPlaneOritention(planeOritention), mfSize(size), mfLineWidth(lineWidth), mColorA(colorA), mColorB(colorB), mLineColor(lineColor)
		{
		
		}

		virtual ~PlaneCheckerTexture() { }

	public:
		virtual void *Clone()
		{
			return (new PlaneCheckerTexture(mPlaneOritention, mfSize, mfLineWidth, mColorA, mColorB, mLineColor));
		}

	public:
		virtual Color3f GetTextureColor(HitRecord& hitRec) const
		{
			float a_ = 0.0f;
			float b_ = 0.0f;

			if (mPlaneOritention == PLANE_XOZ)
			{
				a_ = hitRec.pt.X();
				b_ = hitRec.pt.Z();
			}
			else if (mPlaneOritention == PALNE_XOY)
			{
				a_ = hitRec.pt.X();
				b_ = hitRec.pt.Y();
			}
			else if (mPlaneOritention == PLANE_YOZ)
			{
				a_ = hitRec.pt.Y();
				b_ = hitRec.pt.Z();
			}

			int ia_ = std::floor(a_ / mfSize);
			int ib_ = std::floor(b_ / mfSize);
			float a2_ = a_ / mfSize - ia_;
			float b2_ = b_ / mfSize - ib_;
			float width_ = 0.5f * mfLineWidth / mfSize;
			bool in_outline = (a2_ < width_ || a2_ > 1.0f - width_) || (b2_ < width_ || b2_ > 1.0f - width_);

			if ((ia_ + ib_) % 2 == 0)
			{
				if (!in_outline)
				{
					return mColorA;
				}
			}
			else
			{
				if (!in_outline)
				{
					return mColorB;
				}
			}

			return mLineColor;
		}

		virtual Color3f Sample(float u, float v) const  { return Color3f(0.0f, 0.0f, 0.0f); }

		virtual Color3f GetTexelColor(int texelX, int texelY) const { return Color3f(0.0f, 0.0f, 0.0f); }

	public:
		inline void SetPlaneOritention(EPlaneOritention planeOritention) { mPlaneOritention = planeOritention; }

		inline void SetCheckSize(float size) { mfSize = size; }

		inline void SetLineWidth(float lineWidth) { mfLineWidth = lineWidth; }

		inline void SetAlternateColor(Color3f colorA, Color3f colorB)
		{
			mColorA = colorA;
			mColorB = colorB;
		}

		inline void SetLineColor(Color3f lineColor) { mLineColor = lineColor; }

	protected:
		EPlaneOritention mPlaneOritention;
		float mfSize;
		float mfLineWidth;
		Color3f mColorA;
		Color3f mColorB;
		Color3f mLineColor;

	};

	//
	class CheckerTexture : public Texture
	{
	public:
		CheckerTexture() { }
		CheckerTexture(Texture *tex0, Texture *tex1, float checkSize = 0.2F)
			: mpTex0(tex0), mpTex1(tex1), mfSize(checkSize)
		{

		}
		virtual ~CheckerTexture()
		{
			if (mpTex0) {
				delete mpTex0; mpTex0 = nullptr;
			}
			if (mpTex1) {
				delete mpTex1; mpTex1 = nullptr;
			}
		}

	public:
		virtual void *Clone()
		{
			return (new CheckerTexture((Texture *)mpTex0->Clone(), (Texture *)mpTex1->Clone(), mfSize));
		}

	public:
		virtual Color3f GetTextureColor(HitRecord& hitRec) const
		{
			float sin_value = std::sin(mfSize * hitRec.pt.X())*std::sin(mfSize*hitRec.pt.Y())*std::sin(mfSize*hitRec.pt.Z());
			if (sin_value < 0.0f) {
				return mpTex0->GetTextureColor(hitRec);

			}

			return mpTex1->GetTextureColor(hitRec);
		}

		virtual Color3f Sample(float u, float v) const { return Color3f(0.0f, 0.0f, 0.0f); }

		virtual Color3f GetTexelColor(int texelX, int texelY) const { return Color3f(0.0f, 0.0f, 0.0f); }

	private:
		float mfSize;
		Texture *mpTex0;
		Texture *mpTex1;

	};

	//
 	class ImageTexture : public Texture
 	{
 	public:
		ImageTexture();
		ImageTexture(const ResourcePool::RESID resID, bool autoDelete);
		ImageTexture(const ImageTexture& rhs);
		ImageTexture& operator=(const ImageTexture& rhs);
		virtual ~ImageTexture();

	public:
		virtual void *Clone();

	public:
		virtual Color3f GetTextureColor(HitRecord& hitRec) const;
		virtual Color3f Sample(float u, float v) const;
		virtual Color3f GetTexelColor(int texelX, int texelY) const;

	public:
		void AttachImageData(ImageTexture const& rhs);
		void DetachImageData(); // directly call this will lead to memory leak. AttachImageData is always before it.
		bool IsValid() const;

		bool LoadPNGFromFile(const char *fileName);
		bool LoadJPGFromFile(const char *fileName);

		void SetMappingMethod(ImageTextureMapping *method);

		// bilinear/mip-map/trilinear sample features.
		// ... ...

	public:
		int GetImageWidth() const;
		int GetImageHeight() const;
		unsigned long GetTotalBytes() const;

	public:
		void SetAutoDelete(bool autoDel);
		void Release();

	private:
		void init_members();

		void copy_constructor(ImageTexture const& rhs);

		bool create_image_data();

	private:
		void fill_texture_data_raw(unsigned char *img_rgb_src);
		void fill_texture_data(unsigned char *img_rgb_src);

	private:
		ResourcePool::RESID		mRESID;
		ImageTextureDesc *		mpImgTexData; // Cause it can be reused by the outer, so it won't be destroied in its destructor.
		bool	mbAutoDelete;
		// This Class is not responsible for its releasing.
		// Cause it can be reused by the outer, so it won't be destroied in its destructor.
		ImageTextureMapping *	mpImgTexMapping;

 	};

 	//
 	// Noise-based textures
 	// FBMTexture
 	class FBMTexture : public Texture {
 	public:
 	    FBMTexture()
 	    : mpLatticeNoise(nullptr), mColor(1.0f, 0.0f, 0.0f), mMinValue(0.0f), mMaxValue(1.0f) {

 	    }

 	    FBMTexture(Color3f const& color, float minValue, float maxValue)
 	    : mpLatticeNoise(nullptr), mColor(color), mMinValue(minValue), mMaxValue(maxValue) {

 	    }

 	    FBMTexture(const FBMTexture& rhs)
 	    : mColor(rhs.mColor), mMinValue(rhs.mMinValue), mMaxValue(rhs.mMaxValue) {
 	        if (rhs.mpLatticeNoise != nullptr) {
                mpLatticeNoise = (LatticeNoise *)rhs.mpLatticeNoise->Clone();
 	        } else {
 	            mpLatticeNoise = nullptr;
 	        }
 	    }

 	    FBMTexture& operator=(const FBMTexture& rhs) {
 	        if (this == &rhs) {
 	            return *this;
 	        }

 	        this->mColor = rhs.mColor;
 	        this->mMinValue = rhs.mMinValue;
 	        this->mMaxValue = rhs.mMaxValue;
            if (rhs.mpLatticeNoise != nullptr) {
                this->mpLatticeNoise = (LatticeNoise *)rhs.mpLatticeNoise->Clone();
            } else {
                this->mpLatticeNoise = nullptr;
            }

 	        return *this;
 	    }

 	    virtual ~FBMTexture() {
 	        if (mpLatticeNoise != nullptr) {
 	            delete mpLatticeNoise;
 	            mpLatticeNoise = nullptr;
 	        }
 	    }

 	public:
 	    virtual void *Clone() {
 	        return (void *)(new FBMTexture(*this));
 	    }

 	public:
        virtual Color3f GetTextureColor(HitRecord& hitRec) const {
            float value = mpLatticeNoise->CalcValue_fbm(hitRec.pt);
            value = mMinValue + (mMaxValue - mMinValue) * value;
            value = RTMath::Clamp(value, 0.0f, 1.0f);

            return (value * mColor);
 	    }

        virtual Color3f Sample(float u, float v) const {
            return BLACK;
 	    }

        virtual Color3f GetTexelColor(int texelX, int texelY) const {
            return BLACK;
 	    }

 	public:
 	    inline void SetLatticeNoise(LatticeNoise *latticeNoise) {
            if (mpLatticeNoise != nullptr) {
                delete mpLatticeNoise;
            }
 	        mpLatticeNoise = latticeNoise;

 	    }

 	    inline void SetColor(const Color3f& color) {
 	        mColor = color;
 	    }

 	    inline void SetMinMaxRange(float minValue, float maxValue) {
 	        mMinValue = minValue;
 	        mMaxValue = maxValue;

 	    }

 	private:
 	    LatticeNoise *mpLatticeNoise;

 	    Color3f mColor;
        float mMinValue;
        float mMaxValue;
 	};

 	// WrappedFBmTexture
    class WrappedFBmTexture : public Texture {
    public:
        WrappedFBmTexture()
        : mpLatticeNoise(nullptr), mExpNum(3), mColor(1.0f, 0.0f, 0.0f), mMinValue(0.0f), mMaxValue(1.0f) {

        }

        WrappedFBmTexture(int expNum, Color3f const& color, float minValue, float maxValue)
        : mpLatticeNoise(nullptr), mExpNum(expNum), mColor(color), mMinValue(minValue), mMaxValue(maxValue) {

        }

        WrappedFBmTexture(const WrappedFBmTexture& rhs)
        : mExpNum(rhs.mExpNum), mColor(rhs.mColor), mMinValue(rhs.mMinValue), mMaxValue(rhs.mMaxValue) {
            if (rhs.mpLatticeNoise != nullptr) {
                mpLatticeNoise = (LatticeNoise *)rhs.mpLatticeNoise->Clone();
            } else {
                mpLatticeNoise = nullptr;
            }
        }

        WrappedFBmTexture& operator=(const WrappedFBmTexture& rhs) {
            if (this == &rhs) {
                return *this;
            }

            this->mExpNum = rhs.mExpNum;
            this->mColor = rhs.mColor;
            this->mMinValue = rhs.mMinValue;
            this->mMaxValue = rhs.mMaxValue;
            if (rhs.mpLatticeNoise != nullptr) {
                mpLatticeNoise = (LatticeNoise *)rhs.mpLatticeNoise->Clone();
            } else {
                mpLatticeNoise = nullptr;
            }

            return *this;
        }

        virtual ~WrappedFBmTexture() {
            if (mpLatticeNoise != nullptr) {
                delete mpLatticeNoise;
                mpLatticeNoise = nullptr;
            }
        }

    public:
        virtual void *Clone() {
            return (void *)(new WrappedFBmTexture(*this));
        }

    public:
        virtual Color3f GetTextureColor(HitRecord& hitRec) const {
            float noise = mExpNum * mpLatticeNoise->CalcValue_fbm(hitRec.pt);
            float value = noise - FLOOR(noise);
            value = mMinValue + (mMaxValue - mMinValue) * value;
            value = RTMath::Clamp(value, 0.0f, 1.0f);

            return (value * mColor);
        }

        virtual Color3f Sample(float u, float v) const {
            return BLACK;
        }

        virtual Color3f GetTexelColor(int texelX, int texelY) const {
            return BLACK;
        }

    public:
        inline void SetExpNum(int expNum) {
            mExpNum = expNum;
        }

        inline void SetLatticeNoise(LatticeNoise *latticeNoise) {
            if (mpLatticeNoise != nullptr) {
                delete mpLatticeNoise;
            }
            mpLatticeNoise = latticeNoise;
        }

        inline void SetColor(const Color3f& color) {
            mColor = color;
        }

        inline void SetMinMaxRange(float minValue, float maxValue) {
            mMinValue = minValue;
            mMaxValue = maxValue;
        }

    private:
        LatticeNoise *mpLatticeNoise;

        int mExpNum;
        Color3f mColor;
        float mMinValue;
        float mMaxValue;
    };

    // WrappedTwoColorsFBmTexture
    class WrappedTwoColorsFBmTexture : public Texture {
    public:
        WrappedTwoColorsFBmTexture()
        : mpLatticeNoise(nullptr), mExpNum(3), mColor1(1.0f, 0.0f, 0.0f), mColor2(0.0f, 0.0f, 1.0f), mMinValue(0.0f), mMaxValue(1.0f) {

        }

        WrappedTwoColorsFBmTexture(int expNum, Color3f const& color1, Color3f const& color2, float minValue, float maxValue)
        : mpLatticeNoise(nullptr), mExpNum(expNum), mColor1(color1), mColor2(color2), mMinValue(minValue), mMaxValue(maxValue) {

        }

        WrappedTwoColorsFBmTexture(const WrappedTwoColorsFBmTexture& rhs)
        : mExpNum(rhs.mExpNum), mColor1(rhs.mColor1), mColor2(rhs.mColor2), mMinValue(rhs.mMinValue), mMaxValue(rhs.mMaxValue) {
            if (rhs.mpLatticeNoise != nullptr) {
                mpLatticeNoise = (LatticeNoise *) rhs.mpLatticeNoise->Clone();
            } else {
                mpLatticeNoise = nullptr;
            }
        }

        WrappedTwoColorsFBmTexture& operator=(const WrappedTwoColorsFBmTexture& rhs) {
            if (this == &rhs) {
                return *this;
            }

            this->mExpNum = rhs.mExpNum;
            this->mColor1 = rhs.mColor1;
            this->mColor2 = rhs.mColor2;
            this->mMinValue = rhs.mMinValue;
            this->mMaxValue = rhs.mMaxValue;
            if (rhs.mpLatticeNoise != nullptr) {
                mpLatticeNoise = (LatticeNoise *)rhs.mpLatticeNoise->Clone();
            } else {
                mpLatticeNoise = nullptr;
            }

            return *this;
        }

        virtual ~WrappedTwoColorsFBmTexture() {
            if (mpLatticeNoise != nullptr) {
                delete mpLatticeNoise;
                mpLatticeNoise = nullptr;
            }
        }

    public:
        virtual void *Clone() {
            return (void *)(new WrappedTwoColorsFBmTexture(*this));
        }

    public:
        virtual Color3f GetTextureColor(HitRecord& hitRec) const {
            float noise = mExpNum * mpLatticeNoise->CalcValue_fbm(hitRec.pt);
            float value = noise - FLOOR(noise);
            value = mMinValue + (mMaxValue - mMinValue) * value;
            value = RTMath::Clamp(value, 0.0f, 1.0f);

            if (noise < 1.0f)
                return (value * mColor1);

            return (value * mColor2);
        }

        virtual Color3f Sample(float u, float v) const {
            return BLACK;
        }

        virtual Color3f GetTexelColor(int texelX, int texelY) const {
            return BLACK;
        }

    public:
        inline void SetExpNum(int expNum) {
            mExpNum = expNum;
        }

        inline void SetLatticeNoise(LatticeNoise *latticeNoise) {
            if (mpLatticeNoise != nullptr) {
                delete mpLatticeNoise;
            }
            mpLatticeNoise = latticeNoise;
        }

        inline void SetColor(const Color3f& color1, const Color3f& color2) {
            mColor1 = color1;
            mColor2 = color2;
        }

        inline void SetMinMaxRange(float minValue, float maxValue) {
            mMinValue = minValue;
            mMaxValue = maxValue;
        }

    private:
        LatticeNoise *mpLatticeNoise;

        int mExpNum;
        Color3f mColor1;
        Color3f mColor2;
        float mMinValue;
        float mMaxValue;
    };

    // RampFBmTexture
    class RampFBmTexture : public Texture {
    public:
        RampFBmTexture()
        : mpRampTex(nullptr), mpLatticeNoise(nullptr), mPerturbence(0.1f), mColNum(0) {

        }

        RampFBmTexture(const RampFBmTexture& rhs)
        : mPerturbence(rhs.mPerturbence), mColNum(rhs.mColNum) {
            if (rhs.mpRampTex != nullptr) {
                mpRampTex = (ImageTexture *)rhs.mpRampTex->Clone();
            } else {
                mpRampTex = nullptr;
            }

            if (rhs.mpLatticeNoise != nullptr) {
                mpLatticeNoise = (LatticeNoise *)rhs.mpLatticeNoise->Clone();
            } else {
                mpLatticeNoise = nullptr;
            }
        }

        RampFBmTexture& operator=(const RampFBmTexture& rhs) {
            if (this == &rhs) {
                return *this;
            }

            this->mPerturbence = rhs.mPerturbence;
            this->mColNum = rhs.mColNum;

            if (rhs.mpRampTex != nullptr) {
                mpRampTex = (ImageTexture *)rhs.mpRampTex->Clone();
            } else {
                mpRampTex = nullptr;
            }

            if (rhs.mpLatticeNoise != nullptr) {
                mpLatticeNoise = (LatticeNoise *)rhs.mpLatticeNoise->Clone();
            } else {
                mpLatticeNoise = nullptr;
            }

            return *this;
        }

        virtual ~RampFBmTexture() {
            if (mpRampTex != nullptr) {
                delete mpRampTex;
                mpRampTex = nullptr;
            }

            if (mpLatticeNoise != nullptr) {
                delete mpLatticeNoise;
                mpLatticeNoise = nullptr;
            }
        }

    public:
        virtual void *Clone() {
            return (void *)(new RampFBmTexture(*this));
        }

    public:
        virtual Color3f GetTextureColor(HitRecord& hitRec) const {
            float noise = mpLatticeNoise->CalcValue_fbm(hitRec.pt);
            float y = hitRec.pt.Y() + mPerturbence * noise;
            float u = (1.0f + std::sin(y)) / 2.0f;
            int row = 1;
            int column = (int)(u * (mColNum - 1));

            return mpRampTex->GetTexelColor(column, row);
        }

        virtual Color3f Sample(float u, float v) const {
            return BLACK;
        }

        virtual Color3f GetTexelColor(int texelX, int texelY) const {
            return BLACK;
        }

    public:
        inline void SetRampTexture(ImageTexture *rampTex) {
            if (mpRampTex != nullptr) {
                delete mpRampTex;
            }
            mpRampTex = rampTex;
            mColNum = mpRampTex->GetImageWidth();
        }

        inline void SetLatticeNoise(LatticeNoise *latticeNoise) {
            if (mpLatticeNoise != nullptr) {
                delete mpLatticeNoise;
            }
            mpLatticeNoise = latticeNoise;
        }

        inline void SetPerturbence(float perterbence) {
            mPerturbence = perterbence;
        }

    private:
        ImageTexture *mpRampTex;

        LatticeNoise *mpLatticeNoise;

        float mPerturbence;
        int mColNum;
    };

	// Floating texture
	class FixedFloatTexture : public Texture {
	public:
		FixedFloatTexture() : mFixedValue(0.0f, 0.0f, 0.0f) { }
		FixedFloatTexture(const float texValue) : mFixedValue(texValue, 0.0f, 0.0f) { }
		virtual ~FixedFloatTexture() { }

	public:
		virtual void *Clone() {
			return (void *)(new FixedFloatTexture(*this));
		}

	public:
		virtual Color3f GetTextureColor(HitRecord& hitRec) const {
			return mFixedValue;
		}

		virtual Color3f Sample(float u, float v) const {
			return mFixedValue;
		}

		virtual Color3f GetTexelColor(int texelX, int texelY) const {
			return mFixedValue;
		}

	public:
		inline void SetFixedValue(const float texValue) {
			mFixedValue.Set(texValue, 0.0f, 0.0f);
		}

	private:
		Color3f mFixedValue;

	};

	// FloatTexture
	class FloatTexture : public Texture {
	public:
		FloatTexture() : mW(0), mH(0), mpTex(nullptr), mpTexMapping(nullptr) {
			
		}

		FloatTexture(int w, int h, float *data) : mW(w), mH(h), mpTex(nullptr) {
			LoadTexture(mW, mH, data);
		}

		FloatTexture(const FloatTexture& rhs)
			: mW(rhs.mW), mH(rhs.mH), mpTex(nullptr), mpTexMapping(rhs.mpTexMapping) {

			if (rhs.mpTex != nullptr) {
				LoadTexture(mW, mH, rhs.mpTex);
			}
			else {
				mpTex = nullptr;
			}

		}

		FloatTexture& operator=(const FloatTexture& rhs) {
			if (this == &rhs) {
				return *this;
			}

			this->mW = rhs.mW;
			this->mH = rhs.mH;
			if (rhs.mpTex != nullptr) {
				LoadTexture(mW, mH, rhs.mpTex);
			}
			else {
				mpTex = nullptr;
			}
			this->mpTexMapping = rhs.mpTexMapping;

			return *this;
		}

		virtual ~FloatTexture() {
			if (mpTex != nullptr) {
				delete[] mpTex;
				mpTex = nullptr;
			}
		}

	public:
		virtual void *Clone() {
			return (void *)(new FloatTexture(*this));
		}

	public:
		virtual Color3f GetTextureColor(HitRecord& hitRec) const {
			if (mpTexMapping != nullptr) {
				mpTexMapping->DoMapping(hitRec);
			}

			Color3f texel_color = Sample(hitRec.u, hitRec.v);
			return texel_color;
		}

		virtual Color3f Sample(float u, float v) const {
			int texel_x = (int)(u * (mW - 1));
			int texel_y = (int)(v * (mH - 1));

			return GetTexelColor(texel_x, texel_y);
		}

		virtual Color3f GetTexelColor(int texelX, int texelY) const {
			return mpTex[texelY * mW + texelX];
		}

	public:
		inline void LoadTexture(int w, int h, float *data) {
			if (mpTex != nullptr) {
				delete[] mpTex;
			}

			mW = w;
			mH = h;
			int len = mW * mH;
			mpTex = new float[len];

			for (int i = 0; i < len; ++i) {
				mpTex[i] = data[i];
			}
		}

		inline void SetMappingMethod(ImageTextureMapping *method) {
			mpTexMapping = method;
		}

	private:
		int mW;
		int mH;
		float *	mpTex;
		ImageTextureMapping *mpTexMapping;

	};

 	//
 	// wrapper transformation functions for texture object.
 	class TextureTransform : public Texture {
 	public:
        TextureTransform()
            : mpProxyTexture(nullptr), mbAutoDelete(false) {

        }

        TextureTransform(Texture *proxy, bool autoDelete = false) {
            mpProxyTexture = proxy;
            mbAutoDelete = autoDelete;

        }

        TextureTransform(const TextureTransform& rhs) {
            create_object(rhs);
        }

        TextureTransform& operator=(const TextureTransform& rhs) {
            if (this == &rhs) {
                return *this;
            }

            create_object(rhs);

            return *this;
        }

        virtual ~TextureTransform() {
            if (mbAutoDelete) {
                delete mpProxyTexture;
                mpProxyTexture = nullptr;
            }
        }

 	public:
 	    virtual void *Clone() {
 	        return (void *)(new TextureTransform(*this));
 	    }

 	public:
        virtual Color3f GetTextureColor(HitRecord& hitRec) const {
            HitRecord recCopy = hitRec;
            Vec3f iv = mTransform.InverseApplyPoint(recCopy.pt);
            recCopy.pt = iv;

            return mpProxyTexture->GetTextureColor(recCopy);
        }

        virtual Color3f Sample(float u, float v) const {
            return mpProxyTexture->Sample(u, v);
        }

        virtual Color3f GetTexelColor(int texelX, int texelY) const {
            return mpProxyTexture->GetTexelColor(texelX, texelY);
        }

 	public:
        virtual void Translate(float x, float y, float z) {
            Transform translate = Transform::Translate(x, y, z);
            mTransform = mTransform * translate;
        }

        virtual void Scale(float a, float b, float c) {
            Transform scale = Transform::Scale(a, b, c);
            mTransform = mTransform * scale;
        }

        virtual void RotateX(float angle) {
            Transform rotateX = Transform::RotateX(angle);
            mTransform = mTransform * rotateX;
        }

        virtual void RotateY(float angle) {
            Transform rotateY = Transform::RotateY(angle);
            mTransform = mTransform * rotateY;
        }

        virtual void RotateZ(float angle) {
            Transform rotateZ = Transform::RotateZ(angle);
            mTransform = mTransform * rotateZ;
        }

 	public:
 	    inline void SetTexture(Texture *tex, bool autoDelete) {
 	        if (mbAutoDelete) {
 	            delete mpProxyTexture;
 	            mpProxyTexture = nullptr;
 	        }

 	        mpProxyTexture = tex;
 	        mbAutoDelete = autoDelete;
 	    }

 	private:
 	    inline void create_object(const TextureTransform& rhs) {
            if (rhs.mpProxyTexture != nullptr) {
                this->mpProxyTexture = (TextureTransform *)rhs.mpProxyTexture->Clone();
                this->mbAutoDelete = rhs.mbAutoDelete;
                this->mTransform = rhs.mTransform;
            } else {
                this->mpProxyTexture = nullptr;
                this->mbAutoDelete = false;
            }
 	    }

 	private:
 	    Texture *mpProxyTexture;
 	    bool mbAutoDelete;

 	    Transform mTransform;

 	};

}
