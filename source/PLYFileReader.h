#pragma once

#include "IMeshFileReaderSink.h"
#include "MeshDesc.h"

namespace LaplataRayTracer
{
	// We should introduce FileSystem to do the job, but now we just use FILE to handle it.
	class PLYFileReader {
	public:
		PLYFileReader();
		~PLYFileReader();

	public:
		int LoadMeshFromFile(const char *fileName, MeshDesc& mesh);
		void SetReadingSink(IMeshFileReaderSink *pSink);

	private:
		// NOTE: some of the 2nd parameters are unused.
		bool check_file_tag(FILE *fp, MeshDesc& mesh);
		bool read_vertex_count(FILE *fp, MeshDesc& mesh);
		bool check_vertex_xyz(FILE *fp, MeshDesc& mesh);
		bool check_vertex_uv(FILE *fp, MeshDesc& mesh);
		bool read_face_count(FILE *fp, MeshDesc& mesh);
		bool check_header_ending(FILE *fp, MeshDesc& mesh);
		bool read_vertex_data(FILE *fp, MeshDesc& mesh);
		bool read_face_data(FILE *fp, MeshDesc& mesh);

	private:
		bool is_ply_tag(char *line);
		bool is_element(char *line);
		bool is_property(char *line);
		bool is_x(char *line);
		bool is_y(char *line);
		bool is_z(char *line);
		bool is_u(char *line);
		bool is_v(char *line);
		bool is_header_ending(char *line);

	private:
		void clear_line(char *line, int len);

	private:
		IMeshFileReaderSink *		mpReaderSink;

	};

}
