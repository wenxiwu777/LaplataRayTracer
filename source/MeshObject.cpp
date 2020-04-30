#include "Utility.h"
#include "ResourcePool.h"
#include "PLYFileReader.h"
#include "ShadeObject.h"
#include "MeshTriangle.h"
#include "MeshObject.h"

namespace LaplataRayTracer
{
	//
	RegularGridMeshObject::RegularGridMeshObject() {
		mMeshType = FLAT_SHADING;
		mnX = 0;
		mnY = 0;
		mnZ = 0;
		mnMeshResID = ResourcePool::INVALID_RES_ID;
		mpMeshDesc = nullptr;
		mbReverseNormal = false;
		mbAutoReleaseMesh = false;
		mbEnableAcceleration = true;
		mpAllMaterial = nullptr;
	}

	RegularGridMeshObject::RegularGridMeshObject(RegularGridMeshObject& other) {
		
	}

	RegularGridMeshObject& RegularGridMeshObject::operator=(RegularGridMeshObject& other) {

		return *this;

	}

	RegularGridMeshObject::~RegularGridMeshObject() {
		//
		if (mbAutoReleaseMesh) {
			if (mnMeshResID != ResourcePool::INVALID_RES_ID) {
				ResourcePool::Instance()->FreeMesh(mnMeshResID);
			}
		}

		mpMeshDesc = nullptr;

		//
		CompoundObject::DeleteObjects();
		release_mehs_cell_objects();

		//
		if (mpAllMaterial != nullptr) {
			delete mpAllMaterial;
			mpAllMaterial = nullptr;
		}
	}

	//
	void *RegularGridMeshObject::Clone() {
		return (RegularGridMeshObject *)(new RegularGridMeshObject(*this));
	}

	//
	bool RegularGridMeshObject::HitTest(Ray const& inRay, const float& tmin, float& tmax, HitRecord& rec) {

		bool is_hit = false;

		if (mbEnableAcceleration) {
			//
			float tx_min, ty_min, tz_min, tx_max, ty_max, tz_max;
			float t0;
			is_hit = mBounding.HitTestImpl(inRay, tx_min, ty_min, tz_min, tx_max, ty_max, tz_max, t0);
			if (is_hit) {
				//
				int ix_, iy_, iz_;

				if (mBounding.IsInside(inRay.O())) {
					ix_ = ((inRay.O().X() - mBounding.mX0) / (mBounding.mX1 - mBounding.mX0)) * mnX;
					iy_ = ((inRay.O().Y() - mBounding.mY0) / (mBounding.mY1 - mBounding.mY0)) * mnY;
					iz_ = ((inRay.O().Z() - mBounding.mZ0) / (mBounding.mZ1 - mBounding.mZ0)) * mnZ;

				}
				else {
					Vec3f pt_ = inRay.O() + t0 * inRay.D();
					ix_ = ((pt_.X() - mBounding.mX0) / (mBounding.mX1 - mBounding.mX0)) * mnX;
					iy_ = ((pt_.Y() - mBounding.mY0) / (mBounding.mY1 - mBounding.mY0)) * mnY;
					iz_ = ((pt_.Z() - mBounding.mZ0) / (mBounding.mZ1 - mBounding.mZ0)) * mnZ;

				}

				ix_ = RTMath::Clamp(ix_, 0, mnX - 1);
				iy_ = RTMath::Clamp(iy_, 0, mnY - 1);
				iz_ = RTMath::Clamp(iz_, 0, mnZ - 1);

				//
				float dtx_ = (tx_max - tx_min) / mnX;
				float dty_ = (ty_max - ty_min) / mnY;
				float dtz_ = (tz_max - tz_min) / mnZ;

				float 	tx_next, ty_next, tz_next;
				int 	ix_step, iy_step, iz_step;
				int 	ix_stop, iy_stop, iz_stop;

				if (inRay.D().X() > 0) {
					tx_next = tx_min + (ix_ + 1) * dtx_;
					ix_step = +1;
					ix_stop = mnX;
				}
				else {
					tx_next = tx_min + (mnX - ix_) * dtx_;
					ix_step = -1;
					ix_stop = -1;
				}

				if (inRay.D().X() == 0.0f) {
					tx_next = FLT_MAX;
					ix_step = -1;
					ix_stop = -1;
				}

				if (inRay.D().Y() > 0) {
					ty_next = ty_min + (iy_ + 1) * dty_;
					iy_step = +1;
					iy_stop = mnY;
				}
				else {
					ty_next = ty_min + (mnY - iy_) * dty_;
					iy_step = -1;
					iy_stop = -1;
				}

				if (inRay.D().Y() == 0.0f) {
					ty_next = FLT_MAX;
					iy_step = -1;
					iy_stop = -1;
				}

				if (inRay.D().Z() > 0) {
					tz_next = tz_min + (iz_ + 1) * dtz_;
					iz_step = +1;
					iz_stop = mnZ;
				}
				else {
					tz_next = tz_min + (mnZ - iz_) * dtz_;
					iz_step = -1;
					iz_stop = -1;
				}

				if (inRay.D().Z() == 0.0f) {
					tz_next = FLT_MAX;
					iz_step = -1;
					iz_stop = -1;
				}

				//
				while (true) {
					GeometricObject *object_ptr = (GeometricObject *)(mvecCells[ix_ + mnX * iy_ + mnX * mnY * iz_]);

					if (tx_next < ty_next && tx_next < tz_next) {
						if (object_ptr && object_ptr->HitTest(inRay, tmin, tmax, rec) && tmax < tx_next) {
							if (object_ptr->IsCompound()) {
								CompoundObject *object_compound_ptr = (CompoundObject *)object_ptr;
								rec.pMaterial = ((MaterialObject *)(object_compound_ptr->At(object_compound_ptr->LastHit())))->GetMaterial();
							}
							else {
								rec.pMaterial = ((MaterialObject *)object_ptr)->GetMaterial();
							}
							return (true);
						}

						tx_next += dtx_;
						ix_ += ix_step;

						if (ix_ == ix_stop)
							return (false);
					}
					else {
						if (ty_next < tz_next) {
							if (object_ptr && object_ptr->HitTest(inRay, tmin, tmax, rec) && tmax < ty_next) {
								if (object_ptr->IsCompound()) {
									CompoundObject *object_compound_ptr = (CompoundObject *)object_ptr;
									rec.pMaterial = ((MaterialObject *)(object_compound_ptr->At(object_compound_ptr->LastHit())))->GetMaterial();
								}
								else {
									rec.pMaterial = ((MaterialObject *)object_ptr)->GetMaterial();
								}
								return (true);
							}

							ty_next += dty_;
							iy_ += iy_step;

							if (iy_ == iy_stop)
								return (false);
						}
						else {
							if (object_ptr && object_ptr->HitTest(inRay, tmin, tmax, rec) && tmax < tz_next) {
								if (object_ptr->IsCompound()) {
									CompoundObject *object_compound_ptr = (CompoundObject *)object_ptr;
									rec.pMaterial = ((MaterialObject *)(object_compound_ptr->At(object_compound_ptr->LastHit())))->GetMaterial();
								}
								else {
									rec.pMaterial = ((MaterialObject *)object_ptr)->GetMaterial();
								}
								return (true);
							}

							tz_next += dtz_;
							iz_ += iz_step;

							if (iz_ == iz_stop)
								return (false);
						}
					}
				}
			}
		}
		else {
			is_hit = CompoundObject::HitTest(inRay, tmin, tmax, rec);
			if (is_hit) {
			//	rec.pMaterial = mpAllMaterial;
				int hitIdx_ = this->LastHit();
				rec.pMaterial = ((MaterialObject *)this->At(hitIdx_))->GetMaterial();
			}
		}

		return is_hit;
	}
	
	bool RegularGridMeshObject::IntersectP(Ray const& inRay, float& tvalue) const
	{
		bool is_hit = false;

		if (mbEnableAcceleration) {
			//
			float tx_min, ty_min, tz_min, tx_max, ty_max, tz_max;
			float t0;
			is_hit = mBounding.HitTestImpl(inRay, tx_min, ty_min, tz_min, tx_max, ty_max, tz_max, t0);
			if (is_hit) {
				//
				int ix_, iy_, iz_;

				if (mBounding.IsInside(inRay.O())) {
					ix_ = ((inRay.O().X() - mBounding.mX0) / (mBounding.mX1 - mBounding.mX0)) * mnX;
					iy_ = ((inRay.O().Y() - mBounding.mY0) / (mBounding.mY1 - mBounding.mY0)) * mnY;
					iz_ = ((inRay.O().Z() - mBounding.mZ0) / (mBounding.mZ1 - mBounding.mZ0)) * mnZ;

				}
				else {
					Vec3f pt_ = inRay.O() + t0 * inRay.D();
					ix_ = ((pt_.X() - mBounding.mX0) / (mBounding.mX1 - mBounding.mX0)) * mnX;
					iy_ = ((pt_.Y() - mBounding.mY0) / (mBounding.mY1 - mBounding.mY0)) * mnY;
					iz_ = ((pt_.Z() - mBounding.mZ0) / (mBounding.mZ1 - mBounding.mZ0)) * mnZ;

				}

				ix_ = RTMath::Clamp(ix_, 0, mnX - 1);
				iy_ = RTMath::Clamp(iy_, 0, mnY - 1);
				iz_ = RTMath::Clamp(iz_, 0, mnZ - 1);

				//
				float dtx_ = (tx_max - tx_min) / mnX;
				float dty_ = (ty_max - ty_min) / mnY;
				float dtz_ = (tz_max - tz_min) / mnZ;

				float 	tx_next, ty_next, tz_next;
				int 	ix_step, iy_step, iz_step;
				int 	ix_stop, iy_stop, iz_stop;

				if (inRay.D().X() > 0) {
					tx_next = tx_min + (ix_ + 1) * dtx_;
					ix_step = +1;
					ix_stop = mnX;
				}
				else {
					tx_next = tx_min + (mnX - ix_) * dtx_;
					ix_step = -1;
					ix_stop = -1;
				}

				if (inRay.D().X() == 0.0f) {
					tx_next = FLT_MAX;
					ix_step = -1;
					ix_stop = -1;
				}

				if (inRay.D().Y() > 0) {
					ty_next = ty_min + (iy_ + 1) * dty_;
					iy_step = +1;
					iy_stop = mnY;
				}
				else {
					ty_next = ty_min + (mnY - iy_) * dty_;
					iy_step = -1;
					iy_stop = -1;
				}

				if (inRay.D().Y() == 0.0f) {
					ty_next = FLT_MAX;
					iy_step = -1;
					iy_stop = -1;
				}

				if (inRay.D().Z() > 0) {
					tz_next = tz_min + (iz_ + 1) * dtz_;
					iz_step = +1;
					iz_stop = mnZ;
				}
				else {
					tz_next = tz_min + (mnZ - iz_) * dtz_;
					iz_step = -1;
					iz_stop = -1;
				}

				if (inRay.D().Z() == 0.0f) {
					tz_next = FLT_MAX;
					iz_step = -1;
					iz_stop = -1;
				}

				//
				while (true) {
					GeometricObject *object_ptr = (GeometricObject *)(mvecCells[ix_ + mnX * iy_ + mnX * mnY * iz_]);

					if (tx_next < ty_next && tx_next < tz_next) {
						if (object_ptr && object_ptr->IntersectP(inRay, tvalue) && tvalue < tx_next) {
							return (true);
						}

						tx_next += dtx_;
						ix_ += ix_step;

						if (ix_ == ix_stop)
							return (false);
					}
					else {
						if (ty_next < tz_next) {
							if (object_ptr && object_ptr->IntersectP(inRay, tvalue) && tvalue < ty_next) {
								return (true);
							}

							ty_next += dty_;
							iy_ += iy_step;

							if (iy_ == iy_stop)
								return (false);
						}
						else {
							if (object_ptr && object_ptr->IntersectP(inRay, tvalue) && tvalue < tz_next) {
								return (true);
							}

							tz_next += dtz_;
							iz_ += iz_step;

							if (iz_ == iz_stop)
								return (false);
						}
					}
				}
			}
		}
		else {
			is_hit = CompoundObject::IntersectP(inRay, tvalue);
		}

		return is_hit;
	}
	
	//
	// From CompoundObject
	float RegularGridMeshObject::Area() const
	{
		return 0.0f;
	}

	bool RegularGridMeshObject::GetBoundingBox(float t0, float t1, AABB& bounding) {
		Vec3f pt_min = this->find_min_bounds();
		Vec3f pt_max = this->find_max_bounds();

		mBounding.mX0 = pt_min.X();
		mBounding.mY0 = pt_min.Y();
		mBounding.mZ0 = pt_min.Z();
		mBounding.mX1 = pt_max.X();
		mBounding.mY1 = pt_max.Y();
		mBounding.mZ1 = pt_max.Z();

		bounding = mBounding;

		return true;
	}

	void RegularGridMeshObject::Update(float t) {
		// to do nothing.

	}

	bool RegularGridMeshObject::IsCompound() const {
		return true;
	}

	float RegularGridMeshObject::PDFValue(Vec3f const& o, Vec3f const& v) const {
		return 0.0f;
	}

	//
	// From IAccelerationGeometric
	void RegularGridMeshObject::BuildupAccelerationStructure() {
		release_mehs_cell_objects();

		// find the boundary
		AABB bounding_box;
		if (!this->GetBoundingBox(0.0f, 0.0f, bounding_box)) {
			return;
		}

		// compute the scale and size information
		int obj_num = (int)mvecObjects.size();

		// get the length/width/height of the boundary
		float boundary_length = bounding_box.mX1 - bounding_box.mX0;
		float boundary_width = bounding_box.mZ1 - bounding_box.mZ0;
		float boundary_height = bounding_box.mY1 - bounding_box.mY0;

		// get the average cube cell length of each cell
		const float scale_factor = 2.0f;
		float cube_cell_length = std::pow((boundary_length * boundary_width * boundary_height) / obj_num, 0.333333f);

		// get the cell count in each three directions (length/width/height)
		mnX = (scale_factor * boundary_length) / cube_cell_length + 1;
		mnY = (scale_factor * boundary_width) / cube_cell_length + 1;
		mnZ = (scale_factor * boundary_height) / cube_cell_length + 1;

		// initialize the cells via total cell count
		unsigned long cell_count = mnX * mnY * mnZ;

		mvecCells.reserve(cell_count);

		vector<int> each_cell_obj_count;
		each_cell_obj_count.reserve(cell_count);

		for (int i = 0; i < cell_count; ++i) {
			mvecCells.push_back(nullptr);
			each_cell_obj_count.push_back(0);
		}
		
		// match each object in its base class to the cells in the boundary(each object may cover multiply cells).
		AABB curr_obj_bounding;
		int curr_obj_index;

		for (int i = 0; i < obj_num; ++i) {
			if (mvecObjects[i]->GetBoundingBox(0.0f, 0.0f, curr_obj_bounding)) {
				// compute the coverage cells in three directions.
				int min_x = ((curr_obj_bounding.mX0 - bounding_box.mX0) / (bounding_box.mX1 - bounding_box.mX0)) * mnX;
				int min_y = ((curr_obj_bounding.mY0 - bounding_box.mY0) / (bounding_box.mY1 - bounding_box.mY0)) * mnY;
				int min_z = ((curr_obj_bounding.mZ0 - bounding_box.mZ0) / (bounding_box.mZ1 - bounding_box.mZ0)) * mnZ;
				int max_x = ((curr_obj_bounding.mX1 - bounding_box.mX0) / (bounding_box.mX1 - bounding_box.mX0)) * mnX;
				int max_y = ((curr_obj_bounding.mY1 - bounding_box.mY0) / (bounding_box.mY1 - bounding_box.mY0)) * mnY;
				int max_z = ((curr_obj_bounding.mZ1 - bounding_box.mZ0) / (bounding_box.mZ1 - bounding_box.mZ0)) * mnZ;

				min_x = RTMath::Clamp(min_x, 0, mnX - 1);
				min_y = RTMath::Clamp(min_y, 0, mnY - 1);
				min_z = RTMath::Clamp(min_z, 0, mnZ - 1);
				max_x = RTMath::Clamp(max_x, 0, mnX - 1);
				max_y = RTMath::Clamp(max_y, 0, mnY - 1);
				max_z = RTMath::Clamp(max_z, 0, mnZ - 1);

				for (int iz = min_z; iz <= max_z; ++iz) {
					for (int jy = min_y; jy <= max_y; ++jy) {
						for (int kx = min_x; kx <= max_x; ++kx) {
							curr_obj_index = kx + mnX * jy + mnX * mnY * iz;
							
							if (each_cell_obj_count[curr_obj_index] == 0) {
								mvecCells[curr_obj_index] = mvecObjects[i];
								each_cell_obj_count[curr_obj_index] += 1;
							}
							else if (each_cell_obj_count[curr_obj_index] == 1) {
								CompoundObject *pCompoundObj = new CompoundObject;
								pCompoundObj->EnableAutoDelete(false);
								pCompoundObj->AddObject(mvecCells[curr_obj_index]);
								pCompoundObj->AddObject(mvecObjects[i]);
								mvecCells[curr_obj_index] = pCompoundObj;
								each_cell_obj_count[curr_obj_index] += 1;
							}
							else {
								((CompoundObject *)mvecCells[curr_obj_index])->AddObject(mvecObjects[i]);
								each_cell_obj_count[curr_obj_index] += 1;
							}
						}
					}
				}
			}
		}

		each_cell_obj_count.clear();
	}

	//
	// From IMeshFileReaderSink
	void RegularGridMeshObject::OnReadVertexRecord(float& x, float& y, float& z) {
		// to do nothing. We don't care about each vertex data, cause the mesh desc will hold all of them.
	}

	void RegularGridMeshObject::OnReadFaceRecord(int& index0, int& index1, int& index2) {
		if (mpMeshDesc == nullptr) {
			return;
		}

		MeshTriangle *mesh_triagnle = nullptr;
		if (mMeshType == EMeshType::FLAT_SHADING) {
			mesh_triagnle = new FlatShadingMeshTriangle(index0, index1, index2, mpMeshDesc);
		}
		else if (mMeshType == EMeshType::SMOOTH_SHADING) {
			mesh_triagnle = new SmoothShadingMeshTriangle(index0, index1, index2, mpMeshDesc);
		}
		else if (mMeshType == EMeshType::FLAT_UV_SHADING) {
			mesh_triagnle = new FlatShadingUVMeshTriangle(index0, index1, index2, mpMeshDesc);
		}
		else if (mMeshType == EMeshType::SMOOTH_UV_SHADING) {
			mesh_triagnle = new SmoothShadingUVMeshTriangle(index0, index1, index2, mpMeshDesc);
		}

		if (mbReverseNormal) {
			mesh_triagnle->ComputeNormal(true);
		}

		MaterialObject *material_triangle_object = new MaterialObject(mesh_triagnle, mpAllMaterial, true, false);
		mvecObjects.push_back(material_triangle_object);
	}

	//
	bool RegularGridMeshObject::LoadFromPLYFile(const char *meshFileName, const EMeshType meshType) {
		mbAutoReleaseMesh = true;
		mMeshType = meshType;

		mnMeshResID = ResourcePool::Instance()->AllocMesh();
		if (mnMeshResID == ResourcePool::INVALID_RES_ID) {
			return false;
		}
		MeshDesc *mesh_desc = ResourcePool::Instance()->QueryMesh(mnMeshResID);
		if (mesh_desc == nullptr) {
			return false;
		}

		DeleteObjects();
		release_mehs_cell_objects();

		mpMeshDesc = mesh_desc;
		mvecObjects.reserve(mesh_desc->mesh_face_count);

		PLYFileReader reader;
		reader.SetReadingSink(this);
		int succ = reader.LoadMeshFromFile(meshFileName, *mesh_desc);
		if (succ != 0) {
			DeleteObjects();
			release_mehs_cell_objects();
			return false;
		}

		if (mMeshType == SMOOTH_SHADING || mMeshType == SMOOTH_UV_SHADING) {
			update_mesh_normals(mesh_desc);
		}

		return true;
	}

	bool RegularGridMeshObject::LoadFromMeshDesc(const int meshResID, const EMeshType meshType) {
		if (meshResID == ResourcePool::INVALID_RES_ID) {
			return false;
		}

		mbAutoReleaseMesh = false;
		mMeshType = meshType;
		mnMeshResID = meshResID;

		MeshDesc *mesh_desc = ResourcePool::Instance()->QueryMesh(meshResID);
		if (mesh_desc == nullptr) {
			return false;
		}
		mpMeshDesc = mesh_desc;

		DeleteObjects();
		release_mehs_cell_objects();

		mvecObjects.reserve(mesh_desc->mesh_face_count);
		for (int i = 0; i < mesh_desc->mesh_face_count; ++i) {
			OnReadFaceRecord(mesh_desc->mesh_face_datas[i].index0, 
				mesh_desc->mesh_face_datas[i].index1, 
				mesh_desc->mesh_face_datas[i].index2);
		}

		if (mMeshType == SMOOTH_SHADING || mMeshType == SMOOTH_UV_SHADING) {
			update_mesh_normals(mesh_desc);
		}

		return true;
	}

	void RegularGridMeshObject::SetMaterial(Material *material) {
		if (mpAllMaterial != nullptr) {
			delete mpAllMaterial;
			mpAllMaterial = nullptr;
		}

		mpAllMaterial = material;
	}

	Material *RegularGridMeshObject::GetMaterial() {
		return mpAllMaterial;
	}

	void RegularGridMeshObject::SetSubObjectMaterial(const int index, Material *material, bool autoDelete) {
		((MaterialObject *)mvecObjects[index])->SetMaterial(material, autoDelete);
	}

	Material *RegularGridMeshObject::GetSubObjectMaterial(const int index) {
		return ((MaterialObject *)mvecObjects[index])->GetMaterial();
	}

	void RegularGridMeshObject::ReverseMeshNormals() {
		mbReverseNormal = !mbReverseNormal;
	}

	void RegularGridMeshObject::EnableAcceleration(bool enable) {
		mbEnableAcceleration = enable;
	}

	//
	Vec3f RegularGridMeshObject::find_min_bounds() {
		AABB obj_box;
		Vec3f pt_min(FLT_MAX);
		int num_objects = (int)mvecObjects.size();

		for (int i = 0; i < num_objects; ++i) {
			mvecObjects[i]->GetBoundingBox(0.0f, 0.0f, obj_box);

			if (obj_box.mX0 < pt_min.X()) {
				pt_min[0] = obj_box.mX0;
			}
			else if (obj_box.mY0 < pt_min.Y()) {
				pt_min[1] = obj_box.mY0;
			}
			else if (obj_box.mZ0 < pt_min.Z()) {
				pt_min[2] = obj_box.mZ0;
			}
		}

		pt_min[0] -= RegularGridMeshObject::KEpsilon();
		pt_min[1] -= RegularGridMeshObject::KEpsilon();
		pt_min[2] -= RegularGridMeshObject::KEpsilon();

		return pt_min;
	}

	Vec3f RegularGridMeshObject::find_max_bounds() {
		AABB obj_box;
		Vec3f pt_max(-FLT_MAX);
		int num_objects = (int)mvecObjects.size();

		for (int i = 0; i < num_objects; ++i) {
			mvecObjects[i]->GetBoundingBox(0.0f, 0.0f, obj_box);

			if (obj_box.mX1 > pt_max.X()) {
				pt_max[0] = obj_box.mX1;
			}
			else if (obj_box.mY1 > pt_max.Y()) {
				pt_max[1] = obj_box.mY1;
			}
			else if (obj_box.mZ1 > pt_max.Z()) {
				pt_max[2] = obj_box.mZ1;
			}
		}

		pt_max[0] += RegularGridMeshObject::KEpsilon();
		pt_max[1] += RegularGridMeshObject::KEpsilon();
		pt_max[2] += RegularGridMeshObject::KEpsilon();

		return pt_max;
	}

	void RegularGridMeshObject::update_mesh_normals(MeshDesc *meshDesc) {
		meshDesc->mesh_normal.clear();
		meshDesc->mesh_normal.reserve(meshDesc->mesh_vertex_count);
	
		for (int i = 0; i < meshDesc->mesh_vertex_count; ++i) { // how many vertex we have?
			Vec3f vertex_average_normal(0.0f);
			int face_count = (int)meshDesc->mesh_vertex_faces[i].size(); // how many faces per vertex?
			for (int j = 0; j < face_count; ++j) {
				// average each normal of the face related to the current vertex.
				MaterialObject *meshTriMaterialObj = (MaterialObject *)(mvecObjects[meshDesc->mesh_vertex_faces[i][j]]);
				const Vec3f face_normal = ((MeshTriangle *)meshTriMaterialObj->GetProxyObject())->GetNormal();
				vertex_average_normal += face_normal;
			}
			vertex_average_normal.MakeUnit();

			meshDesc->mesh_normal.push_back(vertex_average_normal);
		}
	}

	void RegularGridMeshObject::release_mehs_cell_objects() {
		for (int i = 0; i < mvecCells.size(); ++i) {
			if (mvecCells[i]->IsCompound()) {
				delete mvecCells[i];
			}
		}

		mvecCells.clear();
	}

}
