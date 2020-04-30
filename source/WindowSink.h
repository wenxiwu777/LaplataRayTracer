#pragma once

namespace LaplataRayTracer
{
	//
	class IRenderWindowSink
	{
	public:
		virtual ~IRenderWindowSink() { }

	public:
		virtual void OnNotifyRenderProgress(int row, int total_row) = 0;

	};

	//

}
