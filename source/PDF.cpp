#include "GeometricObject.h"
#include "PDF.h"

namespace LaplataRayTracer {

	////////////////
	CosinePDF::CosinePDF() {

	}

	CosinePDF::CosinePDF(Vec3f& w) {
		m_onb_coord.BuildFromW(w);
	}

	CosinePDF::~CosinePDF() {

	}

	//
	float CosinePDF::Value(Vec3f const& vec_direction) const {
		Vec3f vec_unit_direction = vec_direction;
		vec_unit_direction.MakeUnit();

		float cosine_ = Dot(vec_unit_direction, m_onb_coord.W());
		if (cosine_ > 0.0f) {
			float pdf_ = cosine_ / PI_CONST;
			return pdf_;
		}

		return 0.0f;
	}

	Vec3f CosinePDF::Generate(void) const {
        Vec3f random_consine_ = SamplerBase::RandomConsineDirection();
		return (m_onb_coord.Local2(random_consine_));
	}

	//
	void CosinePDF::SetW(Vec3f& w) {
		m_onb_coord.BuildFromW(w);
	}

	////////////////
	LightPDF::LightPDF(GeometricObject *hitable_list, Vec3f const& origin)
		: mpHitables(hitable_list), mOrigin(origin) {

	}

	LightPDF::~LightPDF() {

	}

	//
	float LightPDF::Value(Vec3f const& vec_direction) const {
		return (mpHitables->PDFValue(mOrigin, vec_direction));
	}

	Vec3f LightPDF::Generate(void) const {
		return (mpHitables->RandomSampleDirection(mOrigin));
	}

	////////////////
	MixturePDF::MixturePDF(PDF *pdf0, PDF *pdf1) {
		mPDF[0] = pdf0;
		mPDF[1] = pdf1;

	}

	MixturePDF::~MixturePDF() {

	}

	float MixturePDF::Value(Vec3f const& vec_direction) const {
		float pdf_value0 = mPDF[0]->Value(vec_direction);
		float pdf_value1 = mPDF[1]->Value(vec_direction);
		return (0.5f * pdf_value0 + 0.5f * pdf_value1);
	}

	Vec3f MixturePDF::Generate(void) const {
		if (Random::drand48() < 0.5f) {
			return mPDF[0]->Generate();      
		}
		else {
			return mPDF[1]->Generate();            
		}
	}

}
