#pragma once

namespace LaplataRayTracer
{
	class IGeometricAcceleration {
	public:
		virtual ~IGeometricAcceleration() { }

	public:
		virtual void BuildupAccelerationStructure() = 0; // Like Regular Grid/BVH BOX/KD TREE/... ...

	};

}
