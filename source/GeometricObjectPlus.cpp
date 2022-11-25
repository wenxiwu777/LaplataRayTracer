#include "ShadeObject.h"

namespace LaplataRayTracer {

	//
	// SolidCylinder
	void SolidCylinder::SetMaterial(Material *bodyMaterail, bool autoDelete0,
		Material *topMaterial, bool autoDelete1,
		Material *bottomMaterial, bool autoDelete2) {

		if (bodyMaterail != nullptr && topMaterial != nullptr && bottomMaterial != nullptr) {
			MaterialObject *bodyPart = new MaterialObject(mvecObjects[SolidCylinder::BODY], bodyMaterail, true, autoDelete0);
			SetObject(SolidCylinder::BODY, bodyPart);

			MaterialObject *topPart = new MaterialObject(mvecObjects[SolidCylinder::TOP], topMaterial, true, autoDelete1);
			SetObject(SolidCylinder::TOP, topPart);

			MaterialObject *bottomPart = new MaterialObject(mvecObjects[SolidCylinder::BOTTOM], bottomMaterial, true, autoDelete2);
			SetObject(SolidCylinder::BOTTOM, bottomPart);

			mMaterial = true;
		}
	}

	void SolidCylinder::SetPartMaterial(int partNo, Material *material, bool autoDelete) {
		MaterialObject *partObj = new MaterialObject(mvecObjects[partNo], material, true, autoDelete);
		SetObject(partNo, partObj);

		mMaterial = true;
	}

	//
	// FlatRimmedBowl
	void FlatRimmedBowl::SetMaterial(Material *innerMaterail, bool autoDelete0,
		Material *outerMaterial, bool autoDelete1,
		Material *topMaterial, bool autoDelete2) {

		if (innerMaterail != nullptr && outerMaterial != nullptr && topMaterial != nullptr) {
			MaterialObject *innerPart = new MaterialObject(mvecObjects[FlatRimmedBowl::INNER], innerMaterail, true, autoDelete0);
			SetObject(FlatRimmedBowl::INNER, innerPart);

			MaterialObject *outerPart = new MaterialObject(mvecObjects[FlatRimmedBowl::OUTER], outerMaterial, true, autoDelete1);
			SetObject(FlatRimmedBowl::OUTER, outerPart);

			MaterialObject *topPart = new MaterialObject(mvecObjects[FlatRimmedBowl::TOP], topMaterial, true, autoDelete2);
			SetObject(FlatRimmedBowl::TOP, topPart);

			mMaterial = true;
		}

	}

	void FlatRimmedBowl::SetPartMaterial(int partNo, Material *material, bool autoDelete) {
		MaterialObject *partObj = new MaterialObject(mvecObjects[partNo], material, true, autoDelete);
		SetObject(partNo, partObj);

		mMaterial = true;
	}

	//
	// ThickRing
	void ThickRing::SetMaterial(Material *innerMaterial, bool autoDelete0,
		Material *outerMaterial, bool autoDelete1,
		Material *topMaterial, bool autoDelete2,
		Material *bottomMaterial, bool autoDelete3) {

		MaterialObject *innerPart = new MaterialObject(mvecObjects[ThickRing::INNER], innerMaterial, true, autoDelete0);
		SetObject(ThickRing::INNER, innerPart);

		MaterialObject *outerPart = new MaterialObject(mvecObjects[ThickRing::OUTER], outerMaterial, true, autoDelete1);
		SetObject(ThickRing::OUTER, outerPart);

		MaterialObject *topPart = new MaterialObject(mvecObjects[ThickRing::TOP], topMaterial, true, autoDelete2);
		SetObject(ThickRing::TOP, topPart);

		MaterialObject *bottomPart = new MaterialObject(mvecObjects[ThickRing::BOTTOM], bottomMaterial, true, autoDelete3);
		SetObject(ThickRing::BOTTOM, bottomPart);

	}

	void ThickRing::SetPartMaterial(int partNo, Material *material, bool autoDelete) {
		MaterialObject *partObj = new MaterialObject(mvecObjects[partNo], material, true, autoDelete);
		SetObject(partNo, partObj);

		mMaterial = true;
	}

	//
	void SolidCone::SetMaterial(Material *bodyMaterial, bool autoDelete0,
		Material *bottomMaterial, bool autoDelete1) {

		MaterialObject *bodyPart = new MaterialObject(mvecObjects[SolidCone::BODY], bodyMaterial, true, autoDelete0);
		SetObject(SolidCone::BODY, bodyPart);

		MaterialObject *bottomPart = new MaterialObject(mvecObjects[SolidCone::BOTTOM], bottomMaterial, true, autoDelete1);
		SetObject(SolidCone::BOTTOM, bottomPart);

	}

	void SolidCone::SetPartMaterial(int partNo, Material *material, bool autoDelete) {
		MaterialObject *partObj = new MaterialObject(mvecObjects[partNo], material, true, autoDelete);
		SetObject(partNo, partObj);

		mMaterial = true;
	}

	// FrustumCone
	void FrustumCone::SetMaterial(Material *bodyMaterial, bool autoDelete0,
		Material *topMaterial, bool autoDelete1,
		Material *bottomMaterial, bool autoDelete2) {

		MaterialObject *bodyPart = new MaterialObject(mvecObjects[FrustumCone::BODY], bodyMaterial, true, autoDelete0);
		SetObject(FrustumCone::BODY, bodyPart);

		MaterialObject *topPart = new MaterialObject(mvecObjects[FrustumCone::TOP], topMaterial, true, autoDelete1);
		SetObject(FrustumCone::TOP, topPart);

		MaterialObject *bottomPart = new MaterialObject(mvecObjects[FrustumCone::BOTTOM], bottomMaterial, true, autoDelete2);
		SetObject(FrustumCone::BOTTOM, bottomPart);
	}

	void FrustumCone::SetPartMaterial(int partNo, Material *material, bool autoDelete) {
		MaterialObject *partObj = new MaterialObject(mvecObjects[partNo], material, true, autoDelete);
		SetObject(partNo, partObj);

		mMaterial = true;
	}

	// ThickTrimedCone
	void ThickTrimedCone::SetMaterial(Material *innerMaterial, bool autoDelete0,
		Material *outerMaterial, bool autoDelete1,
		Material *topMaterial, bool autoDelete2,
		Material *bottomMaterial, bool autoDelete3) {

		MaterialObject *innerPart = new MaterialObject(mvecObjects[ThickTrimedCone::INNER], innerMaterial, true, autoDelete0);
		SetObject(ThickTrimedCone::INNER, innerPart);

		MaterialObject *outerPart = new MaterialObject(mvecObjects[ThickTrimedCone::OUTER], outerMaterial, true, autoDelete1);
		SetObject(ThickTrimedCone::OUTER, outerPart);

		MaterialObject *topPart = new MaterialObject(mvecObjects[ThickTrimedCone::TOP], topMaterial, true, autoDelete2);
		SetObject(ThickTrimedCone::TOP, topPart);

		MaterialObject *bottomPart = new MaterialObject(mvecObjects[ThickTrimedCone::BOTTOM], bottomMaterial, true, autoDelete3);
		SetObject(ThickTrimedCone::BOTTOM, bottomPart);

	}

	void ThickTrimedCone::SetPartMaterial(int partNo, Material *material, bool autoDelete) {
		MaterialObject *partObj = new MaterialObject(mvecObjects[partNo], material, true, autoDelete);
		SetObject(partNo, partObj);

		mMaterial = true;
	}

}
