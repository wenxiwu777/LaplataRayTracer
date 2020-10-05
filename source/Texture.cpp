#include "ImageIO.h"
#include "Utility.h"
#include "Texture.h"

namespace LaplataRayTracer
{
	//
	ImageTexture::ImageTexture() {
		init_members();
	}

	ImageTexture::ImageTexture(const ResourcePool::RESID resID, bool autoDelete) {
		mRESID = resID;
		mpImgTexData = ResourcePool::Instance()->QueryTexture(resID);
		mbAutoDelete = autoDelete;
		mpImgTexMapping = nullptr;
	}

	ImageTexture::ImageTexture(const ImageTexture& rhs) {
		copy_constructor(rhs);
	}

	ImageTexture& ImageTexture::operator=(const ImageTexture& rhs) {
		if (this == &rhs) {
			return *this;
		}

		copy_constructor(rhs);

		return *this;
	}

	ImageTexture::~ImageTexture() {
		Release();
	}

	//
	void *ImageTexture::Clone() {
		return new ImageTexture(*this);		// ERROR: new (ImageTexture(*this));
	}

	//
	Color3f ImageTexture::GetTextureColor(HitRecord& hitRec) const {
		if (mpImgTexMapping != nullptr) {
			mpImgTexMapping->DoMapping(hitRec);
		}

		Color3f texel_color = Sample(hitRec.u, hitRec.v);
		return texel_color;
	}

	// !!NOTE: don't support mip-map/bilinear/trilinear mapping so far.!!
	Color3f ImageTexture::Sample(float u, float v) const {
		int texel_x = (int)(u * (mpImgTexData->width - 1));
		int texel_y = (int)(v * (mpImgTexData->height - 1));

		//texel_y = mpImgTexData->height - 1 - texel_y;

		return GetTexelColor(texel_x, texel_y);
	}

	Color3f ImageTexture::GetTexelColor(int texelX, int texelY) const {
//		texelX = RTMath::Clamp(texelX, 0, mpImgTexData->width - 1);
//		texelY = RTMath::Clamp(texelY, 0, mpImgTexData->height - 1);

#ifdef TEXTURE_STREAM_RAW
		float r = mpImgTexData->texDataRAW[texelY * mpImgTexData->width * 3 + texelX * 3 + 0] / 255.0F;
		float g = mpImgTexData->texDataRAW[texelY * mpImgTexData->width * 3 + texelX * 3 + 1] / 255.0F;
		float b = mpImgTexData->texDataRAW[texelY * mpImgTexData->width * 3 + texelX * 3 + 2] / 255.0F;

		Color3f texel_rgb;
		texel_rgb.Set(r, g, b);

		return texel_rgb;
#else
		return mpImgTexData->texData[texelY * mpImgTexData->width + texelX];
#endif // TEXTURE_STREAM_RAW
	}

	//
	void ImageTexture::AttachImageData(ImageTexture const& rhs) {
		if (this->IsValid()) {
			this->Release();
		}

		copy_constructor(rhs);
	}

	void ImageTexture::DetachImageData() {
		init_members();
	}

	bool ImageTexture::IsValid() const {
		return ((mRESID != ResourcePool::INVALID_RES_ID) && (mpImgTexData != nullptr));
	}

	bool ImageTexture::LoadPNGFromFile(const char *fileName) {
		if (!this->IsValid()) {
			if (!create_image_data()) {
				return false;
			}
		}

		unsigned char *img_rgb;
		unsigned int img_width, img_height;
		unsigned int error_ = ImageIO::LodePNGDecode24File(&img_rgb, &img_width, &img_height, fileName);
		if (error_ != 0) {
		//	error_ = ImageIO::LodePNGDecode32File(&img_rgb, &img_width, &img_height, fileName);
		//	if (error_ != 0) {
				std::string str_error = ImageIO::LodePNGErrorText(error_);
				printf("Error reading PNG \"%s\": %s", fileName, str_error.c_str());
				return false;
		//	}
		}

		mpImgTexData->width = img_width;
		mpImgTexData->height = img_height;

		unsigned char *img_rgb_src = img_rgb;

#ifdef TEXTURE_STREAM_RAW
		fill_texture_data_raw(img_rgb_src);
#else
		fill_texture_data(img_rgb_src);
#endif // TEXTURE_STREAM_RAW

		free(img_rgb);
		return true;
	}

	bool ImageTexture::LoadJPGFromFile(const char *fileName) {
		if (!this->IsValid()) {
			if (!create_image_data()) {
				return false;
			}
		}

		int img_width, img_height, img_channel_count;
	//	stbi_set_flip_vertically_on_load(true);

		bool loaded_ = false;
        unsigned char *img_rgb = ImageIO::STBILoadImage(fileName, &img_width, &img_height, &img_channel_count, STBI_rgb);
		if (img_rgb) {
			if (img_channel_count == 3) {
				loaded_ = true;

				mpImgTexData->width = img_width;
				mpImgTexData->height = img_height;

				unsigned char *img_rgb_src = img_rgb;

#ifdef TEXTURE_STREAM_RAW
				fill_texture_data_raw(img_rgb_src);
#else
				fill_texture_data(img_rgb_src);
#endif // TEXTURE_STREAM_RAW
			}
		}

		stbi_image_free(img_rgb);

		return loaded_;
	}

	void ImageTexture::SetMappingMethod(ImageTextureMapping *method) {
		mpImgTexMapping = method;
	}

	//
	int ImageTexture::GetImageWidth() const {
		if (!this->IsValid()) {
			return 0;
		}

		return mpImgTexData->width;
	}

	int ImageTexture::GetImageHeight() const {
		if (!this->IsValid()) {
			return 0;
		}

		return mpImgTexData->height;
	}

	unsigned long ImageTexture::GetTotalBytes() const {
		return (GetImageWidth() * GetImageHeight() * 3U);
	}

	void ImageTexture::SetAutoDelete(bool autoDel) {
		mbAutoDelete = autoDel;
	}

	//
	void ImageTexture::Release() {
		if (mbAutoDelete) {
			if (IsValid()) {
				ResourcePool::Instance()->FreeTexture(mRESID);
				mRESID = ResourcePool::INVALID_RES_ID;
				mpImgTexData = nullptr;

// 				if (mpImgTexMapping) {
// 					delete mpImgTexMapping;
// 					mpImgTexMapping = nullptr;
// 				}
			}
		}
	}

	//
	void ImageTexture::init_members() {
		mRESID = ResourcePool::INVALID_RES_ID;
		mpImgTexData = nullptr;
		mbAutoDelete = false;
		mpImgTexMapping = nullptr;
	}

	void ImageTexture::copy_constructor(ImageTexture const& rhs) {
		this->Release();

		mRESID = rhs.mRESID;
		mpImgTexData = rhs.mpImgTexData;
		if (mRESID != ResourcePool::INVALID_RES_ID) {
			mpImgTexData = ResourcePool::Instance()->QueryTexture(mRESID);
		}
		mbAutoDelete = rhs.mbAutoDelete;
		mpImgTexMapping = rhs.mpImgTexMapping;
	}

	bool ImageTexture::create_image_data() {
		mRESID = ResourcePool::Instance()->AllocTexture();
		mpImgTexData = ResourcePool::Instance()->QueryTexture(mRESID);
		if (mpImgTexData == nullptr) {
			return false;
		}
		mbAutoDelete = true;

		return true;
	}

	//
	void ImageTexture::fill_texture_data_raw(unsigned char *img_rgb_src) {
		mpImgTexData->texDataRAW = new unsigned char[mpImgTexData->width * mpImgTexData->height * 3];
		for (unsigned int y = 0; y < mpImgTexData->height; ++y) {
			for (unsigned int x = 0; x < mpImgTexData->width; ++x, img_rgb_src += 3) {
				mpImgTexData->texDataRAW[y * mpImgTexData->width * 3 + x * 3 + 0] = img_rgb_src[0];
				mpImgTexData->texDataRAW[y * mpImgTexData->width * 3 + x * 3 + 1] = img_rgb_src[1];
				mpImgTexData->texDataRAW[y * mpImgTexData->width * 3 + x * 3 + 2] = img_rgb_src[2];
			}
		}
	}

	void ImageTexture::fill_texture_data(unsigned char *img_rgb_src) {
		mpImgTexData->texData.reserve(mpImgTexData->width * mpImgTexData->height);
		for (unsigned int y = 0; y < mpImgTexData->height; ++y) {
			for (unsigned int x = 0; x < mpImgTexData->width; ++x, img_rgb_src += 3) {
				float r = img_rgb_src[0] / 255.0f;
				float g = img_rgb_src[1] / 255.0f;
				float b = img_rgb_src[2] / 255.0f;
				Color3f texel_rgb;
				texel_rgb.Set(r, g, b);
				mpImgTexData->texData.push_back(texel_rgb);
			}
		}
	}
}
