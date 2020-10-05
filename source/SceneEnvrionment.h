#pragma once

#include "Vec3.h"
#include "ImageIO.h"
#include "Utility.h"

namespace LaplataRayTracer {

//
// Background for scene.
class WorldEnvironment
{
public:
    WorldEnvironment() { }
    virtual ~WorldEnvironment() { }

public:
    virtual Color3f Shade(Ray const& ray) = 0;

};

class EmptyEnv : public WorldEnvironment
{
public:
    EmptyEnv(Color3f const& col) {
        mColor = col;
    }
    virtual ~EmptyEnv() { }

public:
    virtual Color3f Shade(Ray const& ray) override {
        return mColor;
    }

private:
    Color3f mColor;

};

class SkylineEnv : public WorldEnvironment
{
public:
    SkylineEnv() { }
    virtual ~SkylineEnv() { }

public:
    virtual Color3f Shade(Ray const& ray) override {
        float v = MakeUnit(ray.D()).Y();
        float t = 0.5f * (v + 1.0f);
        return (1.0f - t) * Color3f(1.0f, 1.0f, 1.0f) + t * Color3f(0.5f, 0.7f, 1.0f);
    }
};

class HDREvn : public WorldEnvironment
{
public:
    HDREvn(const char *fileName) : mHDRI(nullptr) {
        mHDRI = ImageIO::STBILoadHDR(fileName, &mHDRIW, &mHDRIH, &mHDRIC, 0);
    }
    virtual ~HDREvn() {
        if (mHDRI != nullptr) { delete mHDRI; }
    }

public:
    virtual Color3f Shade(Ray const& ray) override {
        Vec3f dir = MakeUnit(ray.D());
        float theta1 = std::atan2(dir.Z(), dir.X())+PI_CONST;
        float theta2 = std::atan(dir.Y()/(std::sqrt(dir.X()*dir.X()+dir.Z()*dir.Z())))+PI_HALF;
        int offset = 0;
        int u = ((int)(theta1/(TWO_PI_CONST)*mHDRIW)+offset)%mHDRIW;
        int v = mHDRIH-(int)(theta2/(PI_CONST)*mHDRIH);
        int addr = mHDRIC*u+mHDRIC*mHDRIW*v;
        float r = RTMath::Clamp(mHDRI[addr]*2, 0.0f, 1.0f);
        float g = RTMath::Clamp(mHDRI[addr+1]*2, 0.0f, 1.0f);
        float b = RTMath::Clamp(mHDRI[addr+2]*2, 0.0f, 1.0f);
//        std::cout << c.R() << " " << c.G() << " " << c.B() << std::endl;
        return Color3f(r, g, b);
    }

private:
    int mHDRIW;
    int mHDRIH;
    int mHDRIC;
    float *mHDRI;

};

}

