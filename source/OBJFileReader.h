#ifndef LAPLATARAYTRACER_OBJFILEREADER_H
#define LAPLATARAYTRACER_OBJFILEREADER_H

#pragma once

#include "IMeshFileReader.h"
#include "MeshDesc.h"

namespace LaplataRayTracer {

    class OBJFileReader {
    public:
        OBJFileReader();
        ~OBJFileReader();

    public:
        int LoadMeshFromFile(const char *fileName, MeshDesc& mesh);
        void SetReadingSink(IMeshFileReaderSink *pSink);

    private:
        void clear_line(char *line, int len);
        void clear_mesh(MeshDesc& mesh);

    private:
        IMeshFileReaderSink *		mpReaderSink;

    };

}

#endif //LAPLATARAYTRACER_OBJFILEREADER_H
