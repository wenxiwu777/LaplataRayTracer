#pragma once

#include "../ext/img/lodepng.h"
#include "../ext/img/stb_image.h"

namespace LaplataRayTracer
{
	class ImageIO {
	public:
		ImageIO() { }
		~ImageIO() { }

	public:
		// PNG Load
		inline static unsigned int LodePNGDecode24File(unsigned char** imgData, unsigned* imgWidth, unsigned* imgHeight,
			const char* fileName) {
			unsigned int error_ = lodepng_decode24_file(imgData, imgWidth, imgHeight, fileName);
			return error_;
		}

		inline static unsigned int LodePNGDecode32File(unsigned char** imgData, unsigned* imgWidth, unsigned* imgHeight,
			const char* fileName) {
			unsigned int error_ = lodepng_decode32_file(imgData, imgWidth, imgHeight, fileName);
			return error_;
		}

		inline static const char *LodePNGErrorText(unsigned int errorCode) {
			const char *err_info = lodepng_error_text(errorCode);
			return err_info;
		}

		//
		inline static unsigned char *STBILoad(const char* fileName, int *width, int *height,
			int *channelCount, int reqChannelCount) {
			return stbi_load(fileName, width, height, channelCount, reqChannelCount);
		}

	};
}
