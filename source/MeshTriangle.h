#pragma once

#include "Vec3.h"
#include "MeshDesc.h"
#include "GeometricObject.h"

namespace LaplataRayTracer
{
	//
	class MeshTriangle : public GeometricObject {
	public:
		MeshTriangle() {

		}

		MeshTriangle(int index0, int index1, int index2, MeshDesc *mesh)
		  : mnIndex0(index0), mnIndex1(index1), mnIndex2(index2), mColor(Color3f(1.0f, 1.0f, 1.0f)), mpMeshDesc(mesh)
		{
			this->ComputeNormal(false);
			this->ComputeArea();
		}

		virtual ~MeshTriangle() {

		}

	public:
		virtual void *Clone() {
			return (MeshTriangle *)(new MeshTriangle(*this));
		}

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec) {
			Vec3f& v0 = mpMeshDesc->mesh_vertices[mnIndex0];
			Vec3f& v1 = mpMeshDesc->mesh_vertices[mnIndex1];
			Vec3f& v2 = mpMeshDesc->mesh_vertices[mnIndex2];

			// I tried many times to abstract the following intersection code, because it is the same as the 
			// SimpleTriangle, but If I do that it will have slight performance issues, so I just keep doing
			// it so far
			// Just about 10min later, and now I made it.
			float beta, gamma;
			bool is_hit = SimpleTriangle::HitTestImpl(v0, v1, v2, mNormal, mColor, beta, gamma, inRay, tmin, tmax, rec);
			return is_hit;
		}

		virtual bool IntersectP(Ray const& inRay, float& tvalue) const {
			Vec3f& v0 = mpMeshDesc->mesh_vertices[mnIndex0];
			Vec3f& v1 = mpMeshDesc->mesh_vertices[mnIndex1];
			Vec3f& v2 = mpMeshDesc->mesh_vertices[mnIndex2];

			bool is_hit = SimpleTriangle::IntersectPImpl(v0, v1, v2, inRay, tvalue);
			return is_hit;
		}

	public:
		virtual float Area() const
		{
			return mfArea;
		}

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding) {
			Vec3f& v0 = mpMeshDesc->mesh_vertices[mnIndex0];
			Vec3f& v1 = mpMeshDesc->mesh_vertices[mnIndex1];
			Vec3f& v2 = mpMeshDesc->mesh_vertices[mnIndex2];

			SimpleTriangle::GetBoundingBoxImpl(v0, v1, v2, bounding);

			return true;
		}

		virtual Vec3f GetNormal(const HitRecord& rec) const
		{
			return mNormal;
		}

		virtual Vec3f RandomSamplePoint() const
		{
            Vec3f& v0 = mpMeshDesc->mesh_vertices[mnIndex0];
            Vec3f& v1 = mpMeshDesc->mesh_vertices[mnIndex1];
            Vec3f& v2 = mpMeshDesc->mesh_vertices[mnIndex2];

            Vec3f pt = SimpleTriangle::RandomSamplePointImpl(v0, v1, v2);
            return pt;
		}

		virtual Vec3f RandomSampleDirection(Vec3f const& v) const
		{
            Vec3f pt = RandomSamplePoint();
            return (pt - v);
		}

		virtual void Update(float t) {
			this->ComputeNormal(false);
			this->ComputeArea();

		}

		virtual bool IsCompound() const
		{
			return false;
		}

		virtual float PDFValue(Vec3f const& o, Vec3f const& v) const {
            return (1.0f / mfArea);
		}
		
	public:
		inline void SetColor(Color3f const& c) { mColor = c; }
		inline Color3f GetColor() const { return mColor; }

		inline void SetIndices(int index0, int index1, int index2) {
			mnIndex0 = index0;
			mnIndex1 = index1;
			mnIndex2 = index2;
		}
		inline void GetIndices(int& index0, int& index1, int index2) {
			index0 = mnIndex0;
			index1 = mnIndex1;
			index2 = mnIndex2;
		}
		inline void SetMeshDesc(MeshDesc *mesh) {
			mpMeshDesc = mesh;
		}
		// not allowed to directly access the underlying mesh pointer, so we don't support GetMeshDesc function.

	public:
		inline Vec3f GetNormal() const { return mNormal; }

	public:
		inline void ComputeNormal(bool reverse) {
			Vec3f& v0 = mpMeshDesc->mesh_vertices[mnIndex0];
			Vec3f& v1 = mpMeshDesc->mesh_vertices[mnIndex1];
			Vec3f& v2 = mpMeshDesc->mesh_vertices[mnIndex2];

			SimpleTriangle::ComputeNormalImpl(v0, v1, v2, mNormal);

			if (reverse) {
				mNormal = -mNormal;
			}
		}

		inline void ComputeArea() {
			Vec3f& v0 = mpMeshDesc->mesh_vertices[mnIndex0];
			Vec3f& v1 = mpMeshDesc->mesh_vertices[mnIndex1];
			Vec3f& v2 = mpMeshDesc->mesh_vertices[mnIndex2];

			SimpleTriangle::ComputeAreaImpl(v0, v1, v2, mfArea);
		}

	public:
		inline float Interpolate_TexU(const float beta, const float gamma) {
			float inter_u = (1 - beta - gamma) * mpMeshDesc->mesh_texU[mnIndex0] +
				beta * mpMeshDesc->mesh_texU[mnIndex1] + gamma * mpMeshDesc->mesh_texU[mnIndex2];
			
			return inter_u;
		}

		inline float Interpolate_TexV(const float beta, const float gamma) {
			float inter_v = (1 - beta - gamma) * mpMeshDesc->mesh_texV[mnIndex0] +
				beta * mpMeshDesc->mesh_texV[mnIndex1] + gamma * mpMeshDesc->mesh_texV[mnIndex2];

			return inter_v;
		}

	public:
		inline static float KEpsilon() { return 0.001f; }

	protected:
		int mnIndex0;
		int mnIndex1;
		int mnIndex2;
		Color3f mColor;
		MeshDesc *		mpMeshDesc;
		Vec3f mNormal;
		float mfArea;

	};

	//
	class FlatShadingMeshTriangle : public MeshTriangle {
	public:
		FlatShadingMeshTriangle() {

		}

		FlatShadingMeshTriangle(int index0, int index1, int index2, MeshDesc *mesh) 
		 : MeshTriangle(index0, index1, index2, mesh) {

		}

		virtual ~FlatShadingMeshTriangle() {

		}

	public:
		virtual void *Clone() {
			return (FlatShadingMeshTriangle *)(new FlatShadingMeshTriangle(*this));
		}

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec) {
			return MeshTriangle::HitTest(inRay, tmin, tmax, rec);
		}

	public:
		virtual float Area() const
		{
			return MeshTriangle::Area();
		}

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding) {
			return MeshTriangle::GetBoundingBox(t0, t1, bounding);
		}

		virtual void Update(float t) {
			return MeshTriangle::Update(t);
		}

	};

	//
	class SmoothShadingMeshTriangle : public MeshTriangle {
	public:
		SmoothShadingMeshTriangle() {

		}

		SmoothShadingMeshTriangle(int index0, int index1, int index2, MeshDesc *mesh)
		 : MeshTriangle(index0, index1, index2, mesh) {

		}

		virtual ~SmoothShadingMeshTriangle() {

		}

	public:
		virtual void *Clone() {
			return (SmoothShadingMeshTriangle *)(new SmoothShadingMeshTriangle(*this));
		}

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec) {
			Vec3f& v0 = mpMeshDesc->mesh_vertices[mnIndex0];
			Vec3f& v1 = mpMeshDesc->mesh_vertices[mnIndex1];
			Vec3f& v2 = mpMeshDesc->mesh_vertices[mnIndex2];

			float beta, gamma;
			bool is_hit = SimpleTriangle::HitTestImpl(v0, v1, v2, mNormal, mColor, beta, gamma, inRay, tmin, tmax, rec);
			if (is_hit) {
				rec.n = this->Interpolate_Normal(beta, gamma);
			}

			return is_hit;
		}

	public:
		virtual float Area() const
		{
			return MeshTriangle::Area();
		}

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding) {
			return MeshTriangle::GetBoundingBox(t0, t1, bounding);
		}

		virtual void Update(float t) {
			MeshTriangle::Update(t);
		}

	public:
		inline Vec3f Interpolate_Normal(const float beta, const float gamma) {
			return ((1 - beta - gamma) * mpMeshDesc->mesh_normal[mnIndex0] +
				beta * mpMeshDesc->mesh_normal[mnIndex1] + gamma * mpMeshDesc->mesh_normal[mnIndex2]);
		}

	};

	//
	class FlatShadingUVMeshTriangle : public FlatShadingMeshTriangle {
	public:
		FlatShadingUVMeshTriangle() {

		}

		FlatShadingUVMeshTriangle(int index0, int index1, int index2, MeshDesc *mesh) 
		 : FlatShadingMeshTriangle(index0, index1, index2, mesh) {

		}

		virtual ~FlatShadingUVMeshTriangle() {

		}

	public:
		virtual void *Clone() {
			return (FlatShadingUVMeshTriangle *)(new FlatShadingUVMeshTriangle(*this));
		}

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec) {
			Vec3f& v0 = mpMeshDesc->mesh_vertices[mnIndex0];
			Vec3f& v1 = mpMeshDesc->mesh_vertices[mnIndex1];
			Vec3f& v2 = mpMeshDesc->mesh_vertices[mnIndex2];

			float beta, gamma;
			bool is_hit = SimpleTriangle::HitTestImpl(v0, v1, v2, mNormal, mColor, beta, gamma, inRay, tmin, tmax, rec);
			if (is_hit) {
				rec.u = this->Interpolate_TexU(beta, gamma);
				rec.v = this->Interpolate_TexV(beta, gamma);
			}

			return is_hit;
		}

	public:
		virtual float Area() const
		{
			return MeshTriangle::Area();
		}

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding) {
			return MeshTriangle::GetBoundingBox(t0, t1, bounding);
		}

		virtual void Update(float t) {
			MeshTriangle::Update(t);
		}

	};

	//
	class SmoothShadingUVMeshTriangle : public SmoothShadingMeshTriangle {
	public:
		SmoothShadingUVMeshTriangle() {

		}

		SmoothShadingUVMeshTriangle(int index0, int index1, int index2, MeshDesc *mesh)
			: SmoothShadingMeshTriangle(index0, index1, index2, mesh) {

		}

		virtual ~SmoothShadingUVMeshTriangle() {

		}

	public:
		virtual void *Clone() {
			return (SmoothShadingUVMeshTriangle *)(new SmoothShadingUVMeshTriangle(*this));
		}

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec) {
			Vec3f& v0 = mpMeshDesc->mesh_vertices[mnIndex0];
			Vec3f& v1 = mpMeshDesc->mesh_vertices[mnIndex1];
			Vec3f& v2 = mpMeshDesc->mesh_vertices[mnIndex2];

			float beta, gamma;
			bool is_hit = SimpleTriangle::HitTestImpl(v0, v1, v2, mNormal, mColor, beta, gamma, inRay, tmin, tmax, rec);
			if (is_hit) {
				rec.n = SmoothShadingMeshTriangle::Interpolate_Normal(beta, gamma);
				rec.u = MeshTriangle::Interpolate_TexU(beta, gamma);
				rec.v = MeshTriangle::Interpolate_TexV(beta, gamma);
			}

			return is_hit;
		}

	public:
		virtual float Area() const
		{
			return MeshTriangle::Area();
		}

		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding) {
			return MeshTriangle::GetBoundingBox(t0, t1, bounding);
		}

		virtual void Update(float t) {
			MeshTriangle::Update(t);
		}

	};
}
