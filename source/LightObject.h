#pragma once

#include "Texture.h"
#include "ShadeObject.h"

namespace LaplataRayTracer
{
	class AreaLightObject : public MaterialObject
	{
	public:
		AreaLightObject(GeometricObject *shape, Texture *tex_light_on, Texture *tex_light_off)
			: MaterialObject(shape, new EmissiveMaterial(tex_light_on)), mpMatTurnOff(new EmissiveMaterial(tex_light_off))
		{

		}

		AreaLightObject(AreaLightObject const& other)
			: MaterialObject(other)
		{
			copy_constructor(other);
		}

		AreaLightObject& operator=(AreaLightObject const& other)
		{
			if (this == &other) { return *this; }

			MaterialObject::operator=(other);
			copy_constructor(other);

			return *this;
		}

		virtual ~AreaLightObject()
		{
			release_material();
		}

	public:
		virtual void *Clone()
		{
			return (AreaLightObject *)(new AreaLightObject(
				(GeometricObject *)GetProxyObject()->Clone(),
				(Texture *)((EmissiveMaterial *)GetMaterial())->GetTexture()->Clone(),
				(Texture *)(mpMatTurnOff->GetTexture()->Clone())));
		}

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec)
		{
			if (MaterialObject::HitTest(inRay, tmin, tmax, rec))
			{
				if (!mbON) { rec.pMaterial = mpMatTurnOff; }

				return true;
			}

			return false;
		}

	public:
		inline void TurnON() { mbON = true; }
		inline void TurnOFF() { mbON = false; }
		inline bool IsTurnON() const { return mbON; }

	private:
		inline void release_material()
		{
			if (mpMatTurnOff != nullptr)
			{
				delete mpMatTurnOff;
				mpMatTurnOff = nullptr;
			}
		}

		inline void copy_constructor(AreaLightObject const& other)
		{
			mbON = other.mbON;

			if (mbMaterialAutoDelete)
			{
				release_material();
				mpMatTurnOff = (EmissiveMaterial *)other.mpMatTurnOff->Clone();
			}
			else
			{
				mpMatTurnOff = other.mpMatTurnOff;
			}
		}

	private:
		bool	mbON;

		EmissiveMaterial *	mpMatTurnOff;
	};

}
