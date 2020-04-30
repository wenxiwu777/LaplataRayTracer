#pragma once

#include "Common.h"
#include "MeshDesc.h"
#include "ImageTextureDesc.h"

namespace LaplataRayTracer
{
	class ResourcePool {
	public:
		static const int INVALID_RES_ID = -1;

	public:
		typedef int	RESID;

	public:
		~ResourcePool() { release(); }

	public:
		static ResourcePool *Instance() {
			if (mpThis == nullptr) {
				mpThis = new ResourcePool;
			}

			return mpThis;
		}

		static void Release() {
			if (mpThis != nullptr) {
				delete mpThis;
				mpThis = nullptr;
			}
		}

	public:
		//
		RESID AllocMesh();
		void FreeMesh(const RESID resID);
		MeshDesc *QueryMesh(const RESID resID);

		//
		RESID AllocTexture();
		void FreeTexture(const RESID resID);
		ImageTextureDesc *QueryTexture(const RESID resID);

	private:
		ResourcePool() { mnResAllocater = 0; }

		void release();

	private:
		static int mnResAllocater;		// currently, it's thread-unsafe, be aware of that.
		static ResourcePool *mpThis;

		map<RESID, MeshDesc * >		mMapMesh;
		map<RESID, ImageTextureDesc * >	mMapTexture;

	};
	
}
