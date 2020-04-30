#pragma once

#include <stdlib.h>
#include <time.h>
// Further support a variety of sampling patterns...

namespace LaplataRayTracer
{
	inline unsigned long long MNWZ() { return 0x100000000; }
	inline unsigned long long ANWZ() { return 0x5DEECE66D;  }
	inline unsigned long long CNWZ() { return 0xB16; }
	inline unsigned long long INFINITY2() { return 0xFFFFFFFFFFFFLL; } // conflict with something already exists 

	static unsigned long long seed = 1;

	const float 	invRAND_MAX = 1.0f / (float)RAND_MAX;

	class Random
	{
	public:
		Random() { }
		~Random() { }

	public:
		inline static void srand48(unsigned int i)
		{
			seed = (((long long int)i) << 16) | rand();
		}
		
		inline static double drand48()
		{
			seed = (ANWZ() * seed + CNWZ()) & INFINITY2();
			unsigned int x = seed >> 16;
			return ((double)x / (double)MNWZ());
		}

		inline static float frand48()
		{
			return (float)drand48();
		}

		inline static void SetSeed(unsigned int s)
		{
			srand(s);
		}

		inline static void SetSeedWithTime()
		{
			srand(time(0));
		}

		inline static int RandInt()
		{
			return rand();
		}

		inline static float RandFloat()
		{
			return (float)RandInt() * invRAND_MAX;
		}

		inline static float RandFloat(float low, float high)
		{
			return (RandFloat() * (high - low)) + low;
		}

		inline static int RandInt(int low, int high)
		{
			return ((int)(RandFloat(0, high - low + 1) + low));
		}
	};

}
