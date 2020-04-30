#pragma once

namespace LaplataRayTracer
{
	// Callback sink interface to handle the events during the reading process.
	class IMeshFileReaderSink {
	public:
		virtual ~IMeshFileReaderSink() { }

	public:
		virtual void OnReadVertexRecord(float& x, float& y, float& z) = 0;
		virtual void OnReadFaceRecord(int& index0, int& index1, int& index2) = 0;

	};

}
