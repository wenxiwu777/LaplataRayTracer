#pragma once

#include "Common.h"
#include "ICloneable.h"
#include "Vec3.h"

namespace LaplataRayTracer {

#define PERM(x)          LatticeNoise::mPermTable[(x)&LatticeNoise::kTableMask]
#define INDEX(ix,iy,iz)  PERM((ix)+PERM((iy)+PERM(iz)))
#define FLOOR(x) 		 ((int)(x) - ((x) < 0 && (x) != (int) (x)))

    //
    class LatticeNoise : public ICloneable {
    public:
        static const int kTableSize = 256;
        static const int kTableMask = kTableSize - 1;
        static const int kSeedValue = 253;

    public:
        LatticeNoise();
        LatticeNoise(int octaves, float gain, float lacunarity, float minValue, float maxValue);
        LatticeNoise(const LatticeNoise& rhs);
        LatticeNoise& operator=(const LatticeNoise& rhs);
        virtual ~LatticeNoise();

    public:
        virtual void *Clone() = 0;

    public:
        virtual float GenValueNoise(Vec3f const& p) const = 0;
        virtual Vec3f GenVectorNoise(Vec3f const& p) const = 0;

    public:
        virtual float CalcValueFractalSum(Vec3f const& p) const;
        virtual Vec3f CalcVectorFractalSum(Vec3f const& p) const;

        virtual float CalcValueTurbulence(Vec3f const& p) const;

        virtual float CalcValue_fbm(Vec3f const& p) const;
        virtual Vec3f CalcVector_fbm(Vec3f const& p) const;

    public:
        void SetOctavesNum(int octavesNum);
        void SetGainFactor(float gain);
        void SetLacunarityFactor(float lacunarity);

        void SetMinMaxValue(float minValue, float maxValue);

    private:
        void init_value_table(int seed);
        void init_vector_table(int seed);

        void compute_fbm_bounds();

    protected:
        static const unsigned char mPermTable[kTableSize];

        float mValueTable[kTableSize];
        Vec3f mVectorTable[kTableSize];

        int mOctavesNum;
        float mGainFactor;
        float mLacunarityFactor;

    private:
        float mfbmMin;
        float mfbmMax;

    };

    //
    class LinearNoise : public LatticeNoise {
    public:
        LinearNoise();
        LinearNoise(int octaves, float gain, float lacunarity, float minValue, float maxValue);
        LinearNoise(const LinearNoise& rhs);
        LinearNoise& operator=(const LinearNoise& rhs);
        virtual ~LinearNoise();

    public:
        virtual void *Clone();

    public:
        virtual float GenValueNoise(Vec3f const& p) const;
        virtual Vec3f GenVectorNoise(Vec3f const& p) const;

    };

    //
    class CubicNoise : public LatticeNoise {
    public:
        CubicNoise();
        CubicNoise(int octaves, float gain, float lacunarity, float minValue, float maxValue);
        CubicNoise(const CubicNoise& rhs);
        CubicNoise& operator=(const CubicNoise& rhs);
        virtual ~CubicNoise();

    public:
        virtual void *Clone();

    public:
        virtual float GenValueNoise(Vec3f const& p) const;
        virtual Vec3f GenVectorNoise(Vec3f const& p) const;

    private:
        template<typename T>
        T four_knot_spline(const float x, const T knots[]) const;

    };

    template<typename T>
    T CubicNoise::four_knot_spline(const float x, const T knots[]) const {
        T c3 = -0.5f * knots[0] + 1.5f * knots[1] - 1.5f * knots[2] + 0.5f * knots[3];
        T c2 = knots[0] - 2.5f * knots[1] + 2.0f * knots[2] - 0.5f * knots[3];
        T c1 = 0.5f * (-knots[0] + knots [2]);
        T c0 = knots[1];

        return (T((c3*x + c2)*x + c1)*x + c0);
    }

}
