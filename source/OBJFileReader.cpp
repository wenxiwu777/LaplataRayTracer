#include "Common.h"
#include "Utility.h"
#include "OBJFileReader.h"

namespace LaplataRayTracer {

    //
    OBJFileReader::OBJFileReader() {
        mpReaderSink = nullptr;
    }

    OBJFileReader::~OBJFileReader() {

    }

    //
    int OBJFileReader::LoadMeshFromFile(const char *fileName, MeshDesc& mesh) {

        int ret = 0;

        FILE *fp = nullptr;
#if defined(PLATFORM_MACOSX) || defined(PLATFORM_LINUX)
        fp = fopen(fileName, "r");
#else
        fopen_s(&fp, fileName, "rb");
#endif

        if (!fp) {
            ret = -1;
            return ret;
        }

        clear_mesh(mesh);

        bool first_face = true;
        unsigned long face_count = 0;
        char line[512];
        while (!feof(fp)) {
            clear_line(line, sizeof(line));

            vector<string> line_parts;
            fgets(line, sizeof(line), fp);
            RTString::Split(line, " ", line_parts);
            if (line[0] == 'v' && line[1] == 'n') { // reach to the normal vector section
                if (line_parts.size() != 4) {
                    ret = -2;
                    break;
                }

                float nx = atof(line_parts[1].c_str());
                float ny = atof(line_parts[2].c_str());
                float nz = atof(line_parts[3].c_str());
                mesh.mesh_normal.push_back(Vec3f(nx, ny, nz));

            } else if (line[0] == 'v' && line[1] == 't') { // reach to the texture section
                if (line_parts.size() != 3) {
                    ret = -3;
                    break;
                }

                float u = atof(line_parts[1].c_str());
                float v = atof(line_parts[2].c_str());
                mesh.mesh_texU.push_back(u);
                mesh.mesh_texV.push_back(v);

            } else if (line[0] == 'v') { // reach to the vertex section
                if (line_parts.size() != 4) {
                    ret = -4;
                    break;
                }

                float x = atof(line_parts[1].c_str());
                float y = atof(line_parts[2].c_str());
                float z = atof(line_parts[3].c_str());
                mesh.mesh_vertices.push_back(Vec3f(x, y, z));
                if (mpReaderSink != nullptr) {
                    mpReaderSink->OnReadVertexRecord(x, y, z);
                }

            } else if (line[0] == 'f') { // reach to the face section
                if (line_parts.size() != 4) {
                    ret = -5;
                    break;
                }

                string ss0 = line_parts[1];
                string ss1 = line_parts[2];
                string ss2 = line_parts[3];
                vector<string> sub_line_parts;
                int index0, index1, index2;
                RTString::Split(ss0, "/", sub_line_parts);
//                if (sub_line_parts.size() != 3) {
//                    ret = -6;
//                    break;
//                }
                index0 = atoi(sub_line_parts[0].c_str());

                sub_line_parts.clear();
                RTString::Split(ss1, "/", sub_line_parts);
//                if (sub_line_parts.size() != 3) {
//                    ret = -6;
//                    break;
//                }
                index1 = atoi(sub_line_parts[0].c_str());

                sub_line_parts.clear();
                RTString::Split(ss2, "/", sub_line_parts);
//                if (sub_line_parts.size() != 3) {
//                    ret = -6;
//                    break;
//                }
                index2 = atoi(sub_line_parts[0].c_str());

                if (first_face) {
                    mesh.mesh_vertex_faces.reserve(mesh.mesh_vertices.size());
                    first_face = false;
#ifdef PLATFORM_WIN
					for (int i = 0; i < mesh.mesh_vertices.size(); ++i) {
						mesh.mesh_vertex_faces.push_back(vector<int>());
					}
#endif // PLATFORM_WIN
                }

                index0 -= 1;
                index1 -= 1;
                index2 -= 1;

                mesh.mesh_vertex_faces[index0].push_back(face_count);
                mesh.mesh_vertex_faces[index1].push_back(face_count);
                mesh.mesh_vertex_faces[index2].push_back(face_count);

                if (mpReaderSink != nullptr) {
                    mpReaderSink->OnReadFaceRecord(index0, index1, index2);
                } else {
                    TriFace face = {index0, index1, index2};
                    mesh.mesh_face_datas.push_back(face);
                }

                ++face_count;

            }
//            else { // reach to unkown type
//                if (strlen(line) > 0) { // the blank line is ok, so we'd skip it.
//                    ret = -7;
//                    break;
//                }
//            }
        }

        //
        if (ret < 0) {
            clear_mesh(mesh);
        } else {
            if (mesh.mesh_texU.size() > 0 && mesh.mesh_texV.size() > 0) {
                mesh.mesh_support_uv = 1;
            }
            mesh.mesh_vertex_count = mesh.mesh_vertices.size();
            mesh.mesh_face_count = mesh.mesh_face_datas.size();
        }

        fclose(fp);
        fp = nullptr;
        return ret;
    }

    void OBJFileReader::SetReadingSink(IMeshFileReaderSink *pSink) {
        mpReaderSink = pSink;
    }

    //
    void OBJFileReader::clear_line(char *line, int len) {
        for (int i = 0; i < len; ++i) {
            line[i] = 0;
        }
    }

    void OBJFileReader::clear_mesh(MeshDesc& mesh) {
        mesh.mesh_vertices.clear();
        mesh.mesh_texU.clear();
        mesh.mesh_texV.clear();
        mesh.mesh_normal.clear();
        mesh.mesh_vertex_faces.clear();
        mesh.mesh_face_datas.clear();

    }

}
