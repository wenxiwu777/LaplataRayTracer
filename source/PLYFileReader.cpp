#include <stdio.h>
#include <stdlib.h>

#include "Common.h"
#include "Utility.h"
#include "PLYFileReader.h"

namespace LaplataRayTracer
{
	//
	PLYFileReader::PLYFileReader() 
		: mpReaderSink(nullptr) {
        mNormal = false;
	}

	PLYFileReader::~PLYFileReader() {

	}

	//
	int PLYFileReader::LoadMeshFromFile(const char *fileName, MeshDesc& mesh, bool bin) {
		mBin = bin;

		FILE *fp = nullptr;
#ifdef PLATFORM_MACOSX
		fp = fopen(fileName, "r");
#else
		fopen_s(&fp, fileName, "rb");
#endif // PLATFORM_MACOSX
		if (fp == nullptr) {
			return -1;
		}

		fseek(fp, 0, SEEK_SET);

		if (!feof(fp)) {

			bool succ = check_file_tag(fp, mesh);
			if (!succ) {
				fclose(fp);
				return -2;
			}

			succ = read_vertex_count(fp, mesh);
			if (!succ) {
				fclose(fp);
				return -3;
			}

            succ = check_vertex_xyz(fp, mesh);
            if (!succ) {
                fclose(fp);
                return -4;
            }

            int ch = fgetc(fp);
            ungetc(ch, fp);
            if ((char)ch == 'p') {

                succ = check_vertex_property_nx_ny_nz_uv(fp, mesh);
                if (!succ) {
                    fclose(fp);
                    return -5;
                }
            }

			succ = read_face_count(fp, mesh);
			if (!succ) {
				fclose(fp);
				return -6;
			}

			succ = check_header_ending(fp, mesh);
			if (!succ) {
				fclose(fp);
				return -7;
			}

			succ = read_vertex_data(fp, mesh);
			if (!succ) {
				fclose(fp);
				return -8;
			}

			succ = read_face_data(fp, mesh);
			if (!succ) {
				fclose(fp);
				return -9;
			}

			fclose(fp);
			return 0;
		}

		fclose(fp);
		return -10;
	}

	void PLYFileReader::SetReadingSink(IMeshFileReaderSink *pSink) {
		mpReaderSink = pSink;
	}

	//
	bool PLYFileReader::check_file_tag(FILE *fp, MeshDesc& mesh) {
		char line[128] = { 0 };
		fgets(line, sizeof(line), fp);

		if (!is_ply_tag(line)) {
			return false;
		}

		return true;
	}

	bool PLYFileReader::read_vertex_count(FILE *fp, MeshDesc& mesh) {

		bool succ = false;
		while (!feof(fp)) {
			char line[128] = { 0 };
			fgets(line, sizeof(line), fp);

			if (is_element(line)) {
				std::string str_element = line;
				std::string str_element_type = str_element.substr(8, 6);
				if (str_element_type == "vertex") {
					std::string str_element_count = str_element.substr(14, str_element.length());
					mesh.mesh_vertex_count = atoi(str_element_count.c_str());

					succ = true;
					break;
				}
			}
		}
		
		return succ;
	}

	bool PLYFileReader::check_vertex_xyz(FILE *fp, MeshDesc& mesh) {
		bool succ = false;
		char line[128] = { 0 };
		while (!feof(fp)) {
			fgets(line, sizeof(line), fp);

			if (is_property(line)) {
				succ = true;
				break;
			}

			clear_line(line, sizeof(line));
		}

		if (succ) {
			succ = false;
			if (is_x(line)) {
				clear_line(line, sizeof(line));
				fgets(line, sizeof(line), fp);
				if (is_y(line)) {
					clear_line(line, sizeof(line));
					fgets(line, sizeof(line), fp);
					if (is_z(line)) {
						clear_line(line, sizeof(line));
						succ = true;
					}
				}
			}
		}

		return succ;
	}

    bool PLYFileReader::check_vertex_property_nx_ny_nz_uv(FILE *fp, MeshDesc& mesh) {
        char line[128] = { 0 };
        clear_line(line, sizeof(line));
        fgets(line, sizeof(line), fp);

		bool has_following_prop = false;
        if (is_property(line) && is_nx(line)) {
            clear_line(line, sizeof(line));
            fgets(line, sizeof(line), fp);
            if (is_property(line) && is_ny(line)) {
                clear_line(line, sizeof(line));
                fgets(line, sizeof(line), fp);
                if (is_property(line) && is_nz(line)) {
                    clear_line(line, sizeof(line));
                    mNormal = true;
					int ch = fgetc(fp);
					ungetc(ch, fp);
					if ((char)ch == 'p') {
						has_following_prop = true;
					}
                }
            }
        }

		if (has_following_prop) {
			clear_line(line, sizeof(line));
			fgets(line, sizeof(line), fp);
		}

		if (is_property(line) && is_u(line)) {
//			if (!has_following_prop) {
				clear_line(line, sizeof(line));
				fgets(line, sizeof(line), fp);
//			}
			if (is_property(line) && is_v(line)) {
				clear_line(line, sizeof(line));
				mesh.mesh_support_uv = 1;
				has_following_prop = false; // reset tag var.

				int ch = fgetc(fp);
				ungetc(ch, fp);
				if ((char)ch == 'p') {
					has_following_prop = true;
				}

				if (has_following_prop) {
					clear_line(line, sizeof(line));
					fgets(line, sizeof(line), fp);

					if (is_property(line) && is_nx(line)) {
						clear_line(line, sizeof(line));
						fgets(line, sizeof(line), fp);
						if (is_property(line) && is_ny(line)) {
							clear_line(line, sizeof(line));
							fgets(line, sizeof(line), fp);
							if (is_property(line) && is_nz(line)) {
								clear_line(line, sizeof(line));
								mNormal = true;
							}
						}
					}
				}
			}
		}

        return true;
	}

	bool PLYFileReader::read_face_count(FILE *fp, MeshDesc& mesh) {
		bool succ = false;

		char line[128] = { 0 };
		while (!feof(fp)) {

			fgets(line, sizeof(line), fp);
			if (is_element(line)) {
				std::string str_element = line;
				std::string str_element_type = str_element.substr(8, 4);
				if (str_element_type == "face") {
					std::string str_element_count = str_element.substr(13, str_element.length());
					mesh.mesh_face_count = atoi(str_element_count.c_str());

					succ = true;
					break;
				}
			}

			clear_line(line, sizeof(line));
		}

		return succ;
	}

	bool PLYFileReader::check_header_ending(FILE *fp, MeshDesc& mesh) {
		bool succ = false;

		char line[128] = { 0 };
		while (!feof(fp)) {

			fgets(line, sizeof(line), fp);
			if (is_header_ending(line)) {
				succ = true;
				break;
			}

			clear_line(line, sizeof(line));
		}

		return succ;
	}

	bool PLYFileReader::read_vertex_data(FILE *fp, MeshDesc& mesh) {
		mesh.mesh_vertices.clear();
//		mesh.mesh_vertex_faces.reserve(mesh.mesh_vertex_count);
		if (mesh.mesh_support_uv)
		{
			mesh.mesh_texU.reserve(mesh.mesh_vertex_count);
			mesh.mesh_texV.reserve(mesh.mesh_vertex_count);
		}

		bool succ = true;
		if (mBin) {
			succ = read_bin_vertex_data(fp, mesh);
		}
		else {
			succ = read_ascii_vertex_data(fp, mesh);
		}

		return succ;
	}

	bool PLYFileReader::read_face_data(FILE *fp, MeshDesc& mesh) {
		mesh.mesh_vertex_faces.clear();
		mesh.mesh_vertex_faces.reserve(mesh.mesh_vertex_count);
		MeshDesc::FaceListPerVertex face_list;
		for (int i = 0; i < mesh.mesh_vertex_count; ++i) {
			mesh.mesh_vertex_faces.push_back(face_list);
		}

		bool succ = true;
		if (mBin) {
			succ = read_bin_face_data(fp, mesh);
		}
		else {
			succ = read_ascii_face_data(fp, mesh);
		}

		return succ;
	}

	bool PLYFileReader::read_ascii_vertex_data(FILE *fp, MeshDesc& mesh) {
		char line[128] = { 0 };
		for (int i = 0; i < mesh.mesh_vertex_count; ++i) {
			fgets(line, sizeof(line), fp);
			std::string str_line = line;
			std::vector<std::string> section;
			RTString::Split(str_line, " ", section);
			float x = atof(section[0].c_str());
			float y = atof(section[1].c_str());
			float z = atof(section[2].c_str());
			mesh.mesh_vertices.push_back(Vec3f(x, y, z));
			if (mNormal) {
				float nx = atof((section[3].c_str()));
				float ny = atof((section[4].c_str()));
				float nz = atof((section[5].c_str()));
				Vec3f norm(nx, ny, nz);
				mesh.mesh_normal.push_back(norm);
			}
			if (mesh.mesh_support_uv && !mNormal) {
				float u = atof((section[3].c_str()));
				float v = atof((section[4].c_str()));
				mesh.mesh_texU.push_back(static_cast<float>(u));
				mesh.mesh_texV.push_back(static_cast<float>(v));
			}
			if (mesh.mesh_support_uv && mNormal) {
				float u = atof((section[6].c_str()));
				float v = atof((section[7].c_str()));
				mesh.mesh_texU.push_back(static_cast<float>(u));
				mesh.mesh_texV.push_back(static_cast<float>(v));
			}
			
			if (mpReaderSink) {
				mpReaderSink->OnReadVertexRecord(x, y, z);
			}

			clear_line(line, sizeof(line));
		}

		return true;
	}

	bool PLYFileReader::read_bin_vertex_data(FILE *fp, MeshDesc& mesh) {
		for (int i = 0; i < mesh.mesh_vertex_count; ++i) {
			float x;
			float y;
			float z;
			fread((void *)&x, sizeof(float), 1U, fp);
			fread((void *)&y, sizeof(float), 1U, fp);
			fread((void *)&z, sizeof(float), 1U, fp);
			mesh.mesh_vertices.push_back(Vec3f(x, y, z));

			if (mNormal) {
                float nx;
                float ny;
                float nz;
                fread((void *)&nx, sizeof(float), 1U, fp);
                fread((void *)&ny, sizeof(float), 1U, fp);
                fread((void *)&nz, sizeof(float), 1U, fp);

                Vec3f norm(nx, ny, nz);
                mesh.mesh_normal.push_back(norm);
			}

			if (mesh.mesh_support_uv) {
				float u;
				float v;
				fread((void *)&u, sizeof(float), 1U, fp);
				fread((void *)&v, sizeof(float), 1U, fp);
				mesh.mesh_texU.push_back(static_cast<float>(u));
				mesh.mesh_texV.push_back(static_cast<float>(v));
			}

			if (mpReaderSink) {
				mpReaderSink->OnReadVertexRecord(x, y, z);
			}
		}

		return true;
	}

	bool PLYFileReader::read_ascii_face_data(FILE *fp, MeshDesc& mesh) {
		bool succ = true;
		char line[128] = { 0 };
		for (int i = 0; i < mesh.mesh_face_count; ++i) {
			fgets(line, sizeof(line), fp);
			string str_line = line;
			vector<string> face_line_data;
			RTString::Split(str_line, " ", face_line_data);
			int vertex_count_per_face = atoi(face_line_data[0].c_str());
			if (vertex_count_per_face != 3) {
				succ = false;
				break;
			}

			int vertex_index0 = atoi(face_line_data[1].c_str());
			int vertex_index1 = atoi(face_line_data[2].c_str());
			int vertex_index2 = atoi(face_line_data[3].c_str());
			if (mpReaderSink) {
				mpReaderSink->OnReadFaceRecord(vertex_index0, vertex_index1, vertex_index2);
			}
			else {
				TriFace one_face = { 0 };
				one_face.index0 = vertex_index0;
				one_face.index1 = vertex_index1;
				one_face.index2 = vertex_index2;
				mesh.mesh_face_datas.push_back(one_face);
			}

			mesh.mesh_vertex_faces[vertex_index0].push_back(i);
			mesh.mesh_vertex_faces[vertex_index1].push_back(i);
			mesh.mesh_vertex_faces[vertex_index2].push_back(i);

			clear_line(line, sizeof(line));
		}

		return succ;
	}

	bool PLYFileReader::read_bin_face_data(FILE *fp, MeshDesc& mesh) {

		for (int i = 0; i < mesh.mesh_face_count; ++i) {
			unsigned char cc;
			int idx0;
			int idx1;
			int idx2;
			fread((void *)&cc, sizeof(unsigned char), 1U, fp);
			fread((void *)&idx0, sizeof(int), 1U, fp);
			fread((void *)&idx1, sizeof(int), 1U, fp);
			fread((void *)&idx2, sizeof(int), 1U, fp);
			if (mpReaderSink) {
				mpReaderSink->OnReadFaceRecord(idx0, idx1, idx2);
			}
			else {
				TriFace one_face = { 0 };
				one_face.index0 = idx0;
				one_face.index1 = idx1;
				one_face.index2 = idx2;
				mesh.mesh_face_datas.push_back(one_face);
			}

			mesh.mesh_vertex_faces[idx0].push_back(i);
			mesh.mesh_vertex_faces[idx1].push_back(i);
			mesh.mesh_vertex_faces[idx2].push_back(i);
		}

		return true;
	}

	//
	bool PLYFileReader::is_ply_tag(char *line) {
		return (line[0] == 'p' && line[1] == 'l' && line[2] == 'y');
	}

	bool PLYFileReader::is_element(char *line) {
		return (line[0] == 'e' && line[1] == 'l' && line[2] == 'e'
			&& line[3] == 'm' && line[4] == 'e' && line[5] == 'n'
			&& line[6] == 't');
	}

	bool PLYFileReader::is_property(char *line) {
		return (line[0] == 'p' && line[1] == 'r' && line[2] == 'o'
			&& line[3] == 'p' && line[4] == 'e' && line[5] == 'r'
			&& line[6] == 't' && line[7] == 'y');
	}

	bool PLYFileReader::is_x(char *line) {
		return (line[15] == 'x');
	}

	bool PLYFileReader::is_y(char *line) {
		return (line[15] == 'y');
	}

	bool PLYFileReader::is_z(char *line) {
		return (line[15] == 'z');
	}

    bool PLYFileReader::is_nx(char *line) {
        return (line[15] == 'n' && line[16] == 'x');
	}

    bool PLYFileReader::is_ny(char *line) {
        return (line[15] == 'n' && line[16] == 'y');
	}

	bool PLYFileReader::is_nz(char *line) {
        return (line[15] == 'n' && line[16] == 'z');
	}

	bool PLYFileReader::is_u(char *line) {
		return (line[15] == 'u');
	}

	bool PLYFileReader::is_v(char *line) {
		return (line[15] == 'v');
	}

	// end_header
	bool PLYFileReader::is_header_ending(char *line) {
		return (line[0] == 'e' && line[1] == 'n' && line[2] == 'd'
			&& line[3] == '_' && line[4] == 'h' && line[5] == 'e'
			&& line[6] == 'a' && line[7] == 'd' && line[8] == 'e'
			&& line[9] == 'r');
	}

	//
	void PLYFileReader::clear_line(char *line, int len) {
		for (int i = 0;i < len;++i) {
			line[i] = 0;
		}
	}
}
