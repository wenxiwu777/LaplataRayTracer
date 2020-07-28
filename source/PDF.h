#pragma once

#include "Vec3.h"
#include "Transform.h"
#include "Sampling.h"

namespace LaplataRayTracer {

	//
	class PDF {
	public:
		virtual ~PDF() { }

	public:
		virtual float Value(Vec3f const& vec_direction) const = 0;
		virtual Vec3f Generate(void) const = 0;

	};

	//
	class CosinePDF : public PDF {
	public:
		CosinePDF();
		CosinePDF(Vec3f& w);
		virtual ~CosinePDF();

	public:
		virtual float Value(Vec3f const& vec_direction) const;
		virtual Vec3f Generate(void) const;

	public:
		void SetW(Vec3f& w);

	private:
        ONB		m_onb_coord;


	};

	//
	class GeometricObject;

	class LightPDF : public PDF {
	public:
		LightPDF(GeometricObject *hitable_list, Vec3f const& origin);
		virtual ~LightPDF();

	public:
		virtual float Value(Vec3f const& vec_direction) const;
		virtual Vec3f Generate(void) const;

	private:
		GeometricObject *mpHitables;
		Vec3f mOrigin;

	};

	//
	class MixturePDF : public PDF {
	public:
		MixturePDF(PDF *pdf0, PDF *pdf1);
		virtual ~MixturePDF();

	public:
		virtual float Value(Vec3f const& vec_direction) const;
		virtual Vec3f Generate(void) const;

	private:
		PDF *mPDF[2];

	};
}
