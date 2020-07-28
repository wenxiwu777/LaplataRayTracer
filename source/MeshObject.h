#pragma once

#include "Common.h"
#include "Vec3.h"
#include "IGeometricAcceleration.h"
#include "IMeshFileReader.h"
#include "GeometricObject.h"
#include "MeshDesc.h"

namespace LaplataRayTracer
{
	class Material;

	enum EMeshType {
		FLAT_SHADING = 0,
		SMOOTH_SHADING,
		FLAT_UV_SHADING,
		SMOOTH_UV_SHADING,
	};

	enum EModelType {
	    MODEL_PLY = 0,
	    MODEL_OBJ,
	};

	//-----------------------------------------------------------------
	// RegularGridMeshObject impl IGeometricAcceleration, IMeshFileReaderSink
	// BVHMeshObject impl IGeometricAcceleration, IMeshFileReaderSink
	// most of the .ply or .obj file parser codes are alread in seperated classes.
	//-----------------------------------------------------------------

	//
	class RegularGridMeshObject : public CompoundObject, public IGeometricAcceleration, public IMeshFileReaderSink {
	public:
		RegularGridMeshObject();
		RegularGridMeshObject(RegularGridMeshObject& other);
		RegularGridMeshObject& operator=(RegularGridMeshObject& other);
		virtual ~RegularGridMeshObject();

	public:
		virtual void *Clone();

	public:
		virtual bool HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec);
		virtual bool IntersectP(Ray const& inRay, float& tvalue) const;

	public:
		virtual float Area() const;
		virtual bool GetBoundingBox(float t0, float t1, AABB& bounding);
		virtual void Update(float t);
		virtual bool IsCompound() const;
		virtual float PDFValue(Vec3f const& o, Vec3f const& v) const;

	public:
		// From IAccelerationGeometric
		virtual void BuildupAccelerationStructure();

	public:
		// From IMeshFileReaderSink
		virtual void OnReadVertexRecord(float& x, float& y, float& z);
		virtual void OnReadFaceRecord(int& index0, int& index1, int& index2);

	public:
		bool LoadFromFile(const char *meshFileName, const EMeshType meshType,
                             EModelType modelType = EModelType::MODEL_PLY, bool isBin = false);
		bool LoadFromMeshDesc(const int meshResID, const EMeshType meshType);

		void SetMaterial(Material *material);
		Material *GetMaterial();
		void SetSubObjectMaterial(const int index, Material *material, bool autoDelete);
		Material *GetSubObjectMaterial(const int index);

		void ReverseMeshNormals();

		void EnableAcceleration(bool enable);

    public:
        // tessellate geometry functions
        void TessellateFlatShpere(Vec3f const& pos, int hNum, int vNum);
        void TessellateFlatRotaionalSweeping(Vec3f const& pos, int hNum, int vNum, const float controlPoints[6][2]);

	public:
		static float KEpsilon() { return 0.0001f; }

	private:
		Vec3f find_min_bounds();
		Vec3f find_max_bounds();

		void update_mesh_normals(MeshDesc *meshDesc);

		void release_mesh_cell_objects();

	private:
		vector<GeometricObject * >	mvecCells; // just the references from its base objects, so we don't release them.
		AABB		mBounding;
		EMeshType	mMeshType;
		int			mnMeshResID;
		MeshDesc *  mpMeshDesc;
		int			mnX;
		int			mnY;
		int			mnZ;
		bool		mbReverseNormal;
		bool		mbAutoReleaseMesh;
		bool		mbEnableAcceleration;
		Material *  mpAllMaterial;

	};
}
