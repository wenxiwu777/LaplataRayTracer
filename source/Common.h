#pragma once

#include <stdlib.h>

#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <iostream>

//#define PLATFORM_WIN
#define PLATFORM_MACOSX

//#define TEXTURE_STREAM_RAW

//#define MICROFACET_SMITH_GGX
#define MICROFACET_COOK

#define PI_CONST		3.14159265f
#define INV_PI_CONST	0.31830989f
#define TWO_PI_CONST	6.2831853f
#define INV_TWO_PI_CONST 0.159155f
#define ANG2RAD(x)		(x*PI_CONST/180.0f)
#define PI_ON_180		0.01745f
#define PI_UNDER_180	57.29578f		// 180_OVER_PI
#define PI_HALF         1.571f
#define PI_OVER_4       0.7854f

using std::vector;
using std::map;
using std::string;

//
namespace LaplataRayTracer
{
	enum EPlaneOritention
	{
		PLANE_XOZ = 0,
		PALNE_XOY,
		PLANE_YOZ,
	};

}
