#pragma once

#include <vector>

#include "Common.h"
#include "Vec3.h"
#include "Random.h"
#include "HitRecord.h"
#include "ICloneable.h"
#include "ImageTextureDesc.h"
#include "ResourcePool.h"
#include "ImageTextureMapping.h"

//#define TEXTURE_STREAM_RAW

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
		virtual Color3f GetTextureColor(HitRecord& hitRec) = 0;
		virtual Color3f Sample(float u, float v) = 0;
		virtual Color3f GetTexelColor(int texelX, int texelY) = 0;

	};

	//
	class SimpleSkyTexture : public Texture // what the hell? what exactly is it?
	{
	public:
		SimpleSkyTexture() { }
		virtual ~SimpleSkyTexture() { }

	public:
		virtual void *Clone() { return (Texture *)(new SimpleSkyTexture); }

	public:
		virtual Color3f GetTextureColor(HitRecord& hitRec)
		{
			Vec3f pt_temp = hitRec.pt;
			return Sample(0.0f, MakeUnit<float>(pt_temp).Y());
		}

		virtual Color3f Sample(float u, float v)
		{
			float t = 0.5f * (v + 1.0f);
			return (1.0f - t) * Color3f(1.0f, 1.0f, 1.0f) + t * Color3f(0.5f, 0.7f, 1.0f);
		}

		virtual Color3f GetTexelColor(int texelX, int texelY)
		{
			return Color3f(0.0f, 0.0f, 0.0f);
		}

	};

	// Constant texture
	class ConstantTexture : public Texture
	{
	public:
		ConstantTexture() : mColor(1.0f, 1.0f, 1.0f) { }
		ConstantTexture(Color3f const& c) : mColor(c) { }
		virtual ~ConstantTexture() { }

	public:
		virtual void *Clone() { return (Texture *)(new ConstantTexture(mColor)); }

	public:
		virtual Color3f GetTextureColor(HitRecord& hitRec)
		{
			return mColor;
		}

		virtual Color3f Sample(float u, float v)
		{
			return mColor;
		}

		virtual Color3f GetTexelColor(int texelX, int texelY)
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
		virtual Color3f GetTextureColor(HitRecord& hitRec)
		{
			return Color3f(Random::frand48(), Random::frand48(), Random::frand48());
		}

		virtual Color3f Sample(float u, float v)
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

		virtual Color3f GetTexelColor(int texelX, int texelY)
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
			mfLineWidth = 2.0;
			mColorA.Set(1.0f, 1.0f, 1.0f);
			mColorB.Set(0.0f, 0.0f, 0.0f);
			mLineColor.Set(1.0f, 0.0f, 0.0f);

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
		virtual Color3f GetTextureColor(HitRecord& hitRec)
		{
			float a_;
			float b_;

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

		virtual Color3f Sample(float u, float v) { return Color3f(0.0f, 0.0f, 0.0f); }

		virtual Color3f GetTexelColor(int texelX, int texelY) { return Color3f(0.0f, 0.0f, 0.0f); }

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
		virtual Color3f GetTextureColor(HitRecord& hitRec)
		{
			float sin_value = std::sin(mfSize * hitRec.pt.X())*std::sin(mfSize*hitRec.pt.Y())*std::sin(mfSize*hitRec.pt.Z());
			if (sin_value < 0.0f) {
				return mpTex0->GetTextureColor(hitRec);

			}

			return mpTex1->GetTextureColor(hitRec);
		}

		virtual Color3f Sample(float u, float v) { return Color3f(0.0f, 0.0f, 0.0f); }

		virtual Color3f GetTexelColor(int texelX, int texelY) { return Color3f(0.0f, 0.0f, 0.0f); }

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
		virtual Color3f GetTextureColor(HitRecord& hitRec);
		virtual Color3f Sample(float u, float v);
		virtual Color3f GetTexelColor(int texelX, int texelY);

	public:
		void AttachImageData(ImageTexture const& rhs);
		void DetachImageData(); // directly call this will lead to memory leak. AttachImageData is always before it.
		bool IsValid() const;

		bool LoadPNGFromFile(const char *fileName);
		bool LoadJPGFromFile(const char *fileName);

		void SetInverseMapping(ImageTextureMapping *imgTexMapping);

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
}