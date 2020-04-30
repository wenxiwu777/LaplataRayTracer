#pragma once

namespace LaplataRayTracer
{
	class ICloneable
	{
	public:
		virtual ~ICloneable() { }

	public:
		virtual void *Clone() = 0;

	};
}
