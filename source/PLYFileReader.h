#pragma once

#include "IMeshFileReader.h"
#include "MeshDesc.h"

namespace LaplataRayTracer
{
	// We should introduce FileSystem to do the job, but now we just use FILE to handle it.
	class PLYFileReader {
	public:
		PLYFileReader();
		~PLYFileReader();

	public:
		int LoadMeshFromFile(const char *fileName, MeshDesc& mesh, bool bin = false);
		void SetReadingSink(IMeshFileReaderSink *pSink);

	private:
		// NOTE: some of the 2nd parameters are unused.
		bool check_file_tag(FILE *fp, MeshDesc& mesh);
		bool read_vertex_count(FILE *fp, MeshDesc& mesh);
		bool check_vertex_xyz(FILE *fp, MeshDesc& mesh);
		bool check_vertex_property_nx_ny_nz_uv(FILE *fp, MeshDesc& mesh);
		bool read_face_count(FILE *fp, MeshDesc& mesh);
		bool check_header_ending(FILE *fp, MeshDesc& mesh);
		bool read_vertex_data(FILE *fp, MeshDesc& mesh);
		bool read_face_data(FILE *fp, MeshDesc& mesh);

		bool read_ascii_vertex_data(FILE *fp, MeshDesc& mesh);
		bool read_bin_vertex_data(FILE *fp, MeshDesc& mesh);
		bool read_ascii_face_data(FILE *fp, MeshDesc& mesh);
		bool read_bin_face_data(FILE *fp, MeshDesc& mesh);

	private:
		bool is_ply_tag(char *line);
		bool is_element(char *line);
		bool is_property(char *line);
		bool is_x(char *line);
		bool is_y(char *line);
		bool is_z(char *line);
		bool is_nx(char *line);
		bool is_ny(char *line);
		bool is_nz(char *line);
		bool is_u(char *line);
		bool is_v(char *line);
		bool is_header_ending(char *line);

	private:
		void clear_line(char *line, int len);

	private:
		IMeshFileReaderSink *		mpReaderSink;
		bool mBin;
		bool mNormal;

	};

}
