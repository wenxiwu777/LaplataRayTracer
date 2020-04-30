#include "ResourcePool.h"

namespace LaplataRayTracer
{
	//
	ResourcePool::RESID ResourcePool::mnResAllocater = 0;
	ResourcePool *ResourcePool::mpThis = nullptr;

	//
	ResourcePool::RESID ResourcePool::AllocMesh() {
		++mnResAllocater;

		MeshDesc *ptr_mesh = new MeshDesc;
		mMapMesh.insert(std::make_pair(mnResAllocater, ptr_mesh));

		return mnResAllocater;
	}

	void ResourcePool::FreeMesh(const RESID resID) {
		map<RESID, MeshDesc * >::iterator iterMesh = mMapMesh.find(resID);
		if (iterMesh != mMapMesh.end()) {
			MeshDesc *ptr_mesh = iterMesh->second;
			delete ptr_mesh;
			ptr_mesh = nullptr;

			mMapMesh.erase(iterMesh);
		}
	}

	MeshDesc *ResourcePool::QueryMesh(const RESID resID) {
		MeshDesc *ptr_mesh = nullptr;

		map<RESID, MeshDesc * >::iterator iterMesh = mMapMesh.find(resID);
		if (iterMesh != mMapMesh.end()) {
			ptr_mesh = iterMesh->second;
		}

		return ptr_mesh;
	}

	//
	ResourcePool::RESID ResourcePool::AllocTexture() {
		++mnResAllocater;

		ImageTextureDesc *ptr_texture = new ImageTextureDesc;
		mMapTexture.insert(std::make_pair(mnResAllocater, ptr_texture));

		return mnResAllocater;
	}

	void ResourcePool::FreeTexture(const RESID resID) {
		map<RESID, ImageTextureDesc * >::iterator iterTexture = mMapTexture.find(resID);
		if (iterTexture != mMapTexture.end()) {
			ImageTextureDesc *ptr_texture = iterTexture->second;
			ptr_texture->texData.clear();
			if (ptr_texture->texDataRAW) {
				delete[] ptr_texture->texDataRAW;
				ptr_texture->texDataRAW = nullptr;
			}
			delete ptr_texture;
			ptr_texture = nullptr;

			mMapTexture.erase(iterTexture);
		}
	}

	ImageTextureDesc *ResourcePool::QueryTexture(const RESID resID) {
		ImageTextureDesc *ptr_texture = nullptr;

		map<RESID, ImageTextureDesc * >::iterator iterTexture = mMapTexture.find(resID);
		if (iterTexture != mMapTexture.end()) {
			ptr_texture = iterTexture->second;
		}

		return ptr_texture;
	}

	//
	void ResourcePool::release() {
		//
		map<RESID, MeshDesc * >::iterator iterMesh = mMapMesh.begin();
		while (iterMesh != mMapMesh.end()) {
			MeshDesc *ptr_mesh = iterMesh->second;
			delete ptr_mesh;
			ptr_mesh = nullptr;

			++iterMesh;
		}

		mMapMesh.clear();

		//
		map<RESID, ImageTextureDesc * >::iterator iterTexture = mMapTexture.begin();
		while (iterTexture != mMapTexture.end()) {
			ImageTextureDesc *ptr_texture = iterTexture->second;
			ptr_texture->texData.clear();
			if (ptr_texture->texDataRAW) {
				delete[] ptr_texture->texDataRAW;
				ptr_texture->texDataRAW = nullptr;
			}
			delete ptr_texture;
			ptr_texture = nullptr;

			++iterTexture;
		}

		mMapTexture.clear();

		//
		mnResAllocater = 0;
	}
}
