#pragma once

namespace LaplataRayTracer
{
	typedef struct ImageTextureDesc_t {

		ImageTextureDesc_t() {
			width = 0;
			height = 0;
			texDataRAW = nullptr;
		}

		int  width;
		int  height;
		std::vector<Color3f> texData;
		unsigned char *texDataRAW;

	} ImageTextureDesc;
}
