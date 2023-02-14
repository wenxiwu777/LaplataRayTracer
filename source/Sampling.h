#pragma once

#include <vector>

#include "ICloneable.h"
#include "Point2.h"
#include "Vec3.h"
#include "Random.h"

using std::vector;

namespace LaplataRayTracer
{
	//
	class SamplerBase : public ICloneable {
	public:
		SamplerBase(void);
		SamplerBase(const int num);
		SamplerBase(const int num, const int numSet);
		SamplerBase(const SamplerBase& s);
		SamplerBase& operator= (const SamplerBase& rhs);

		virtual ~SamplerBase(void);

	public:
		virtual void* Clone(void) = 0;

	public:
		virtual void GenerateSamples(void) = 0;

	public:
		void SetSetCount(const int np);
		int GetSampleCount(void) const;

		void ShuffleXCoordinates(void);
		void ShuffleYCoordinates(void);
		void SetupShuffleIndices(void);

		void MapSamplesToUnitDisk(void);
		void MapSamplesToHemiShpere(const float exp);
		void MapSamplesToShpere(void);

		Point2f	SampleFromUnitSquare(void);
		Point2f SampleFromUnitDisk(void);
		Point3f SampleFromHemishpere(void);
		Point3f SampleFromShpere(void);

		Point2f SampeFromOneSet(void);

	public:
		inline static Point2f SampleInUnitDisk() {
			Point2f pt;

			while (1) {
				pt.x = Random::frand48();
				pt.y = Random::frand48();
				if (pt.SqaureLength() <= 1.0f) {
					break;
				}
			}

			return pt;
		}

		inline static Vec3f SampleInUnitSphere() {
			Vec3f vec;

			while (1) {
				vec.Set(2 * Random::frand48() - 1, 2 * Random::frand48() - 1, 2 * Random::frand48() - 1);
				if (vec.SquareLength() <= 1.0f) {
					break;
				}
			}

			return vec;
		}

		inline static Vec3f SampleUponUnitShpere() {
			Vec3f vec;

			while (1) {
				vec.Set(2 * Random::frand48() - 1, 2 * Random::frand48() - 1, 2 * Random::frand48() - 1);
				if (vec.SquareLength() <= 1.0f) {
					break;
				}
			}

			vec.MakeUnit();
			return vec;
		}

		inline static Vec3f RandomConsineDirection() {
			float r1 = Random::drand48();
			float r2 = Random::drand48();
			float z = std::sqrt(1.0f - r2);
            float phi = 2.0f * PI_CONST * r1;
			float x = std::cos(phi) * 2.0f * std::sqrt(r2);
			float y = std::sin(phi) * 2.0f * std::sqrt(r2);
			return Vec3f(x, y, z);
		}

		inline static Vec3f RandomToShpere(float radius, float squd_dist) {
			float r1 = Random::drand48();
			float r2 = Random::drand48();
			float z = 1.0f + r2 * (std::sqrt(1.0f - radius * radius / squd_dist) - 1.0f);
			float phi = 2.0f * PI_CONST * r1;
			float x = std::cos(phi) * std::sqrt(1.0f - z * z);
			float y = std::sin(phi) * std::sqrt(1.0f - z * z);
			return Vec3f(x, y, z);
		}
        
        inline static Point2f URand2() {
            return Point2f(Random::frand48(), Random::frand48());
        }

        inline static Point2f UniformSampleTriangle(const Point2f& urand2) {
            float su0 = std::sqrt(urand2[0]);
            return Point2f(1 - su0, urand2[1] * su0);
        }
        
        inline static Vec3f UniformSampleCone(const Point2f& urand2, float cosThetaMax) {
            float cosTheta = ((float)1.0f - urand2[0]) + urand2[0] * cosThetaMax;
            float sinTheta = std::sqrt((float)1.0f - cosTheta * cosTheta);
            float phi = urand2[1] * 2.0f * PI_CONST;
            return Vec3f(std::cos(phi) * sinTheta, std::sin(phi) * sinTheta, cosTheta);
        }
        
        inline static Point2f UniformSampleDisk(const Point2f& urand2) {
            float r = std::sqrt(urand2[0]);
            float theta = 2.0f * PI_CONST *urand2[1];
            return Point2f(r * std::cos(theta), r * std::sin(theta));
        }
        
        inline static Point2f ConcentricSampleDisk(const Point2f& urand2) {
            Point2f temp = 2.0f * urand2;
            Point2f uOffset = temp - Point2f(1.0f, 1.0f);
            
            if (uOffset.X() == 0 && uOffset.Y() == 0)
                return Point2f(0.0f, 0.0f);
            
            float theta, r;
            if (std::abs(uOffset.X()) > std::abs(uOffset.Y())) {
                r = uOffset.X();
                theta = PI_OVER_4 * (uOffset.Y() / uOffset.X());
            }
            else {
                r = uOffset.Y();
                theta = PI_HALF - PI_OVER_4 * (uOffset.X() / uOffset.Y());
            }
            return Point2f(std::cos(theta), std::sin(theta)) * r;
        }
        
        inline static Vec3f WeightedSampleHemishpere(const Point2f& urand2) {
            Point2f sample = SamplerBase::UniformSampleDisk(urand2);
            float z = std::sqrt(std::max(0.0f, 1.0f - sample[0] * sample[0] - sample[1] * sample[1]));
            return Vec3f(sample[0], sample[1], z);
        }
        
	protected:
		int 					mnSampleCount;
		int 					mnSets;
		vector<Point2f>			mvecSamples_UnitSquare;
		vector<int>				mvecShuffledIndices;
		vector<Point2f>			mvecSamples_UnitDisk;
		vector<Point3f> 		mvecSamples_HemiShpere;
		vector<Point3f> 		mvecSamples_Shpere;
		unsigned long 			mlCount;
		int 					mnJump;

	};

	//
	class RegularSampler : public SamplerBase {
	public:
		RegularSampler(void);
		RegularSampler(const int num);
		RegularSampler(const RegularSampler& rhs);

		RegularSampler& operator= (const RegularSampler& rhs);
		virtual ~RegularSampler(void);

	public:
		virtual void* Clone(void);

	public:
		virtual void GenerateSamples(void);

	};

	//
	class PureRandomSampler : public SamplerBase {
	public:
		PureRandomSampler(void);
		PureRandomSampler(const int num);
		PureRandomSampler(const PureRandomSampler& rhs);
		PureRandomSampler& operator=(const PureRandomSampler& rhs);
		virtual ~PureRandomSampler(void);
	
	public:
		virtual void* Clone(void);

	public:
		virtual void GenerateSamples(void);

	};

	//
	class JitteredSampler : public SamplerBase {
	public:
		JitteredSampler(void);
		JitteredSampler(const int num);
		JitteredSampler(const int num, const int ns);
		JitteredSampler(const JitteredSampler& rhs);
		JitteredSampler& operator= (const JitteredSampler& rhs);
		virtual ~JitteredSampler(void);

	public:
		virtual void* Clone(void);

	public:
		virtual void GenerateSamples(void);
	};

	//
	class NRooksSampler : public SamplerBase {
	public:
		NRooksSampler(void);
		NRooksSampler(const int num);
		NRooksSampler(const int num, const int ns);
		NRooksSampler(const NRooksSampler& rhs);
		NRooksSampler& operator= (const NRooksSampler& rhs);
		virtual ~NRooksSampler(void);

	public:
		virtual void* Clone(void);

	public:
		virtual void GenerateSamples(void);
	};

	//
	class MultiJitteredSampler : public SamplerBase {
	public:
		MultiJitteredSampler(void);
		MultiJitteredSampler(const int num);
		MultiJitteredSampler(const int num, const int ns);
		MultiJitteredSampler(const MultiJitteredSampler& rhs);
		MultiJitteredSampler& operator= (const MultiJitteredSampler& rhs);
		virtual ~MultiJitteredSampler(void);

	public:
		virtual void* Clone(void);

	public:
		virtual void GenerateSamples(void);
	
	};

	//
	// *** My favorite one :>) ***
	class HammersleySmapler : public SamplerBase {
	public:
		HammersleySmapler(void);
		HammersleySmapler(const int num);
		HammersleySmapler(const HammersleySmapler& rhs);
		HammersleySmapler& operator= (const HammersleySmapler& rhs);
		virtual ~HammersleySmapler(void);

	public:
		virtual void* Clone(void);

	public:
		virtual void GenerateSamples(void);

	private:
		float phi(int j);

	};
}
