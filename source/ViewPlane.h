#pragma once

namespace LaplataRayTracer
{
	class ViewPlane
	{
	public:
		ViewPlane(int w = 400, int h = 400)
			: mw(w), mh(h)
		{

		}
		~ViewPlane()
		{

		}

	public:
		inline void SetViewSize(int w, int h)
		{
			mw = w;
			mh = h;
		}
		inline int Width() const { return mw; }
		inline int Height() const { return mh; }

	private:
		int mw;
		int mh;

	};
}
