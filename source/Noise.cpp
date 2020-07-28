#include "Noise.h"
#include "Utility.h"
#include "Point2.h"
#include "Random.h"
#include "Sampling.h"

namespace  LaplataRayTracer {

    //
    // LatticeNoise
    const unsigned char LatticeNoise::mPermTable[kTableSize] = {
            225,155,210,108,175,199,221,144,203,116,70,213,69,158,33,252,
            5,82,173,133,222,139,174,27,9,71,90,246,75,130,91,191,
            169,138,2,151,194,235,81,7,25,113,228,159,205,253,134,142,
            248,65,224,217,22,121,229,63,89,103,96,104,156,17,201,129,
            36,8,165,110,237,117,231,56,132,211,152,20,181,111,239,218,
            170,163,51,172,157,47,80,212,176,250,87,49,99,242,136,189,
            162,115,44,43,124,94,150,16,141,247,32,10,198,223,255,72,
            53,131,84,57,220,197,58,50,208,11,241,28,3,192,62,202,
            18,215,153,24,76,41,15,179,39,46,55,6,128,167,23,188,
            106,34,187,140,164,73,112,182,244,195,227,13,35,77,196,185,
            26,200,226,119,31,123,168,125,249,68,183,230,177,135,160,180,
            12,1,243,148,102,166,38,238,251,37,240,126,64,74,161,40,
            184,149,171,178,101,66,29,59,146,61,254,107,42,86,154,4,
            236,232,120,21,233,209,45,98,193,114,78,19,206,14,118,127,
            48,79,147,85,30,207,219,54,88,234,190,122,95,67,143,109,
            137,214,145,93,92,100,245,0,216,186,60,83,105,97,204,52
    };

    //
    LatticeNoise::LatticeNoise()
    : mOctavesNum(1), mGainFactor(0.5f), mLacunarityFactor(2.0f),
      mfbmMin(0.0f), mfbmMax(1.0f) {
        init_value_table(kSeedValue);
        init_vector_table(kSeedValue);
        compute_fbm_bounds();

    }

    LatticeNoise::LatticeNoise(int octaves, float gain, float lacunarity, float minValue, float maxValue)
    : mOctavesNum(octaves), mGainFactor(gain), mLacunarityFactor(lacunarity),
      mfbmMin(minValue), mfbmMax(maxValue) {
        init_value_table(kSeedValue);
        init_vector_table(kSeedValue);
        compute_fbm_bounds();

    }

    LatticeNoise::LatticeNoise(const LatticeNoise& rhs)
    : mOctavesNum(rhs.mOctavesNum), mGainFactor(rhs.mGainFactor), mLacunarityFactor(rhs.mLacunarityFactor),
      mfbmMin(rhs.mfbmMin), mfbmMax(rhs.mfbmMax) {
        init_value_table(kSeedValue);
        init_vector_table(kSeedValue);
        compute_fbm_bounds();
    }

    LatticeNoise& LatticeNoise::operator=(const LatticeNoise& rhs) {
        if (this == &rhs) {
            return *this;
        }

        this->mOctavesNum = rhs.mOctavesNum;
        this->mGainFactor = rhs.mGainFactor;
        this->mLacunarityFactor = rhs.mLacunarityFactor;
        this->mfbmMin = rhs.mfbmMin;
        this->mfbmMax = rhs.mfbmMax;
        // mValueTable and mVectorTable dont' have to assign to lhs, because before now, the two are already created.
        // and also, the random values within the two arrays surely can be different.

        return *this;
    }

    LatticeNoise::~LatticeNoise() {

    }

    //
    void LatticeNoise::SetOctavesNum(int octavesNum) {
        mOctavesNum = octavesNum;
    }

    void LatticeNoise::SetGainFactor(float gain) {
        mGainFactor = gain;
    }

    void LatticeNoise::SetLacunarityFactor(float lacunarity) {
        mLacunarityFactor = lacunarity;
    }

    void LatticeNoise::SetMinMaxValue(float minValue, float maxValue) {
        mfbmMin = minValue;
        mfbmMax = maxValue;
    }

    //
    float LatticeNoise::CalcValueFractalSum(Vec3f const& p) const {
        float amplitude = 1.0f;
        float frequency = 1.0f;
        float value_fractal_sum = 0.0f;

        for (int i = 0; i < mOctavesNum; ++i) {
            Vec3f fp = frequency * p;
            value_fractal_sum += amplitude * GenValueNoise(fp);
            amplitude *= 0.5f;
            frequency *= 2.0f;
        }

        value_fractal_sum = (value_fractal_sum - mfbmMin) / (mfbmMax - mfbmMin);

        return value_fractal_sum;
    }

    Vec3f LatticeNoise::CalcVectorFractalSum(Vec3f const& p) const {
        float amplitude = 1.0f;
        float frequency = 1.0f;
        Vec3f vector_fractal_sum;

        for (int i = 0; i < mOctavesNum; ++i) {
            Vec3f fp = frequency * p;
            vector_fractal_sum += amplitude * GenVectorNoise(fp);
            amplitude *= 0.5f;
            frequency *= 2.0f;
        }

        return vector_fractal_sum;
    }

    float LatticeNoise::CalcValueTurbulence(Vec3f const& p) const {
        float amplitude = 1.0f;
        float frequency = 1.0f;
        float turbulence_sum = 0.0f;

        for (int i = 0 ; i < mOctavesNum; ++i) {
            Vec3f fp = frequency * p;
            turbulence_sum += amplitude * std::fabs(GenValueNoise(fp));
            amplitude *= 0.5f;
            frequency *= 2.0f;
        }

        turbulence_sum /= mfbmMax;

        return turbulence_sum;
    }

    float LatticeNoise::CalcValue_fbm(Vec3f const& p) const {
        float amplitude = 1.0f;
        float frequency = 1.0f;
        float value_fbm_sum = 0.0f;

        for (int i = 0; i < mOctavesNum; ++i) {
            Vec3f fp = frequency * p;
            value_fbm_sum += amplitude * GenValueNoise(fp);
            amplitude *= mGainFactor;
            frequency *= mLacunarityFactor;
        }

        value_fbm_sum = (value_fbm_sum - mfbmMin) / (mfbmMax - mfbmMin);

        return value_fbm_sum;
    }

    Vec3f LatticeNoise::CalcVector_fbm(Vec3f const& p) const {
        float amplitude = 1.0f;
        float frequency = 1.0f;
        Vec3f vector_fbm_sum;

        for (int i = 0; i < mOctavesNum; ++i) {
            Vec3f fp = frequency * p;
            vector_fbm_sum += amplitude * GenVectorNoise(fp);
            amplitude *= mGainFactor;
            frequency *= mLacunarityFactor;
        }

        return vector_fbm_sum;
    }

    //
    void LatticeNoise::init_value_table(int seed) {
        Random::SetSeed(seed);
        for (int i = 0; i < kTableSize; ++i) {
            mValueTable[i] = 1.0f - Random::RandFloat() * 2.0f;
        }
    }

    void LatticeNoise::init_vector_table(int seed) {
        float r1, r2;
        float x, y, z;
        float r, phi;

        Random::SetSeed(seed);
        MultiJitteredSampler *multiJitteredSampler = new MultiJitteredSampler(256, 1);

        for (int i = 0; i < kTableSize; ++i) {
            Point2f sample2f = multiJitteredSampler->SampeFromOneSet();
            r1 	= sample2f.X();
            r2 	= sample2f.Y();
            z 	= 1.0f - 2.0f * r1;
            r 	= std::sqrt(1.0f - z * z);
            phi = TWO_PI_CONST * r2;
            x 	= r * std::cos(phi);
            y 	= r * std::sin(phi);
            Vec3f pt(x, y, z);
            pt.MakeUnit();
            mVectorTable[i] = pt;
        }
    }

    void LatticeNoise::compute_fbm_bounds() {
        if (mGainFactor == 1.0f)
            mfbmMax = mOctavesNum;
        else
            mfbmMax = (1.0f - std::pow(mGainFactor, mOctavesNum)) / (1.0f - mGainFactor);

        mfbmMin = -mfbmMax;
    }

    //
    // LinearNoise
    LinearNoise::LinearNoise()
    : LatticeNoise() {

    }

    LinearNoise::LinearNoise(int octaves, float gain, float lacunarity, float minValue, float maxValue)
    : LatticeNoise(octaves, gain, lacunarity, minValue, maxValue) {

    }

    LinearNoise::LinearNoise(const LinearNoise& rhs)
    : LatticeNoise(rhs) {

    }

    LinearNoise& LinearNoise::operator=(const LinearNoise& rhs) {
        if (this == &rhs) {
            return *this;
        }

        LatticeNoise::operator=(rhs);

        return *this;
    }

    LinearNoise::~LinearNoise() {

    }

    //
    void *LinearNoise::Clone() {
        return (void *)(new LinearNoise(*this));
    }

    //
    float LinearNoise::GenValueNoise(Vec3f const& p) const {
        int ix, iy, iz;
        float fx, fy, fz;
        float d[2][2][2];
        float x0, x1, x2, x3, y0, y1, z0;

        ix = FLOOR(p.X());
        fx = p.X() - ix;

        iy = FLOOR(p.Y());
        fy = p.Y() - iy;

        iz = FLOOR(p.Z());
        fz = p.Z() - iz;

        for (int k = 0; k <= 1; ++k)
            for (int j = 0; j <= 1; ++j)
                for (int i = 0; i <= 1; ++i)
                    d[k][j][i] = mValueTable[INDEX(ix + i, iy + j, iz + k)];

        x0 = RTMath::Lerp(d[0][0][0], d[0][0][1], fx);
        x1 = RTMath::Lerp(d[0][1][0], d[0][1][1], fx);
        x2 = RTMath::Lerp(d[1][0][0], d[1][0][1], fx);
        x3 = RTMath::Lerp(d[1][1][0], d[1][1][1], fx);
        y0 = RTMath::Lerp(x0, x1, fy);
        y1 = RTMath::Lerp(x2, x3, fy);
        z0 = RTMath::Lerp(y0, y1, fz);

        return z0;
    }

    Vec3f LinearNoise::GenVectorNoise(Vec3f const& p) const {
        int ix, iy, iz;
        float fx, fy, fz;
        Vec3f d[2][2][2];
        Vec3f x0, x1, x2, x3, y0, y1, z0;

        ix = FLOOR(p.X());
        fx = p.X() - ix;

        iy = FLOOR(p.Y());
        fy = p.Y() - iy;

        iz = FLOOR(p.Z());
        fz = p.Z() - iz;

        for (int k = 0; k <= 1; ++k)
            for (int j = 0; j <= 1; ++j)
                for (int i = 0; i <= 1; ++i)
                    d[k][j][i] = mVectorTable[INDEX(ix + i, iy + j, iz + k)];

        x0 = RTMath::Lerp(d[0][0][0], d[0][0][1], fx);
        x1 = RTMath::Lerp(d[0][1][0], d[0][1][1], fx);
        x2 = RTMath::Lerp(d[1][0][0], d[1][0][1], fx);
        x3 = RTMath::Lerp(d[1][1][0], d[1][1][1], fx);
        y0 = RTMath::Lerp(x0, x1, fy);
        y1 = RTMath::Lerp(x2, x3, fy);
        z0 = RTMath::Lerp(y0, y1, fz);

        return z0;
    }

    //
    // CubicNoise
    CubicNoise::CubicNoise()
            : LatticeNoise() {

    }

    CubicNoise::CubicNoise(int octaves, float gain, float lacunarity, float minValue, float maxValue)
            : LatticeNoise(octaves, gain, lacunarity, minValue, maxValue) {

    }

    CubicNoise::CubicNoise(const CubicNoise& rhs)
    : LatticeNoise(rhs) {

    }

    CubicNoise& CubicNoise::operator=(const CubicNoise& rhs) {
        if (this == &rhs) {
            return *this;
        }

        LatticeNoise::operator=(rhs);

        return *this;
    }

    CubicNoise::~CubicNoise() {

    }

    //
    void *CubicNoise::Clone() {
        return (void *)(new CubicNoise(*this));
    }

    //
    float CubicNoise::GenValueNoise(Vec3f const& p) const {
        int ix, iy, iz;
        float fx, fy, fz;
        float xknots[4], yknots[4], zknots[4];

        ix = FLOOR(p.X());
        fx = p.X() - ix;

        iy = FLOOR(p.Y());
        fy = p.Y() - iy;

        iz = FLOOR(p.Z());
        fz = p.Z() - iz;

        for (int k = -1; k <= 2; ++k) {
            for (int j = -1; j <= 2; ++j) {
                for (int i = -1; i <= 2; ++i) {
                    xknots[i+1] = mValueTable[INDEX(ix + i, iy + j, iz + k)];
                }
                yknots[j+1] = four_knot_spline(fx, xknots);
            }
            zknots[k+1] = four_knot_spline(fy, yknots);
        }

        float z = four_knot_spline(fz, zknots);
        return (RTMath::Clamp(z, -1.0f, 1.0f));
    }

    Vec3f CubicNoise::GenVectorNoise(Vec3f const& p) const {
        int ix, iy, iz;
        float fx, fy, fz;
        Vec3f xknots[4], yknots[4], zknots[4];

        ix = FLOOR(p.X());
        fx = p.X() - ix;

        iy = FLOOR(p.Y());
        fy = p.Y() - iy;

        iz = FLOOR(p.Z());
        fz = p.Z() - iz;

        for (int k = -1; k <= 2; ++k) {
            for (int j = -1; j <= 2; ++j) {
                for (int i = -1; i <= 2; ++i) {
                    xknots[i+1] = mVectorTable[INDEX(ix + i, iy + j, iz + k)];
                }
                yknots[j+1] = four_knot_spline(fx, xknots);
            }
            zknots[k+1] = four_knot_spline(fy, yknots);
        }

        Vec3f z = four_knot_spline(fz, zknots);
        return z;
    }

}

