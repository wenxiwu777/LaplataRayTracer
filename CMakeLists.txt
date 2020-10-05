cmake_minimum_required(VERSION 3.15)
project(laplataraytracer)

set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES
        ext/img/lodepng.h
        ext/img/lodepng.cpp
        ext/img/stb_image.h
        ext/img/tgaimage.h
        ext/img/tgaimage.cpp
        source/AABB.h
        source/BRDF.h
        source/BTDF.h
        source/Camera.h
        source/Common.h
        source/GeometricObject.h
        source/GeometricObjectPlus.h
        source/GeometricObjectPlus.cpp
        source/HitRecord.h
        source/ICloneable.h
        source/IGeometricAcceleration.h
        source/ImageIO.h
        source/ImageProc.h
        source/ImageTextureDesc.h
        source/ImageTextureMapping.h
        source/IMeshFileReader.h
        source/Instance.h
        source/Instance.cpp
        source/Light.h
        source/LightObject.h
        source/Material.h
        source/Matrix.h
        source/Matrix.cpp
        source/MeshDesc.h
        source/MeshDesc.cpp
        source/MeshObject.h
        source/MeshObject.cpp
        source/MeshTriangle.h
        source/MovingObject.h
        source/Noise.h
        source/Noise.cpp
        source/OBJFileReader.h
        source/OBJFileReader.cpp
        source/PDF.h
        source/PDF.cpp
        source/PLYFileReader.h
        source/PLYFileReader.cpp
        source/Point2.h
        source/Random.h
        source/Ray.h
        source/RayTracer.h
        source/Reflection.h
        source/Reflection.cpp
        source/ResourcePool.h
        source/ResourcePool.cpp
        source/Sampling.h
        source/Sampling.cpp
        source/SceneEnvrionment.h
        source/Scene.h
        source/ShadeObject.h
        source/Surface.h
        source/Texture.h
        source/Texture.cpp
        source/Transform.h
        source/Transform.cpp
        source/Utility.h
        source/Utility.cpp
        source/Vec3.h
        source/ViewPlane.h
        source/Volume.h
        source/WindowSink.h
        source/WorldObjects.h)

add_executable(laplataraytracer main.cpp ${SOURCE_FILES} ${SOURCES})
