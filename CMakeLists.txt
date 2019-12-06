cmake_minimum_required(VERSION 3.13)
project(SI3D_TP)
set(CMAKE_DIR "CMake")
set(CMAKE_TUTO_DIR ${CMAKE_DIR}/tutos)


include(${CMAKE_DIR}/cmakeConfiguration.cmake)

include_directories(${PROJECT_SOURCE_DIR}/gKit)

# Create the list of src files for gkit
# Not reccomended practice by cmake developer because you must reload cmake when new files added
file(GLOB_RECURSE GKIT_SRC_FILES
        "${PROJECT_SOURCE_DIR}/gKit/*")

include(${CMAKE_DIR}/addDependencies.cmake)

add_executable(imageViewer.exe
        ${GKIT_SRC_FILES}
        src/image_viewer.cpp)
target_link_libraries(imageViewer.exe
        ${SDL2_LIBRARIES}
        ${SDL2_IMG_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${OPENGL_LIBRARIES})

add_executable(shader_kit.exe
        ${GKIT_SRC_FILES}
        src/shader_kit.cpp)
target_link_libraries(shader_kit.exe
        ${SDL2_LIBRARIES}
        ${SDL2_IMG_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${OPENGL_LIBRARIES})

add_executable(RobotGL.exe
        ${GKIT_SRC_FILES}
        src/RobotGL/robotGL.cpp)
target_link_libraries(RobotGL.exe
        ${SDL2_LIBRARIES}
        ${SDL2_IMG_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${OPENGL_LIBRARIES})

add_executable(RayTraceCPU.exe
        ${GKIT_SRC_FILES}
        src/RayTrace/CPU/mainCPU.cpp
        src/RayTrace/Utils/Utils.cpp
        src/RayTrace/Utils/Utils.h
        src/RayTrace/CPU/RayTraceImageProcessing.cpp
        src/RayTrace/CPU/RayTraceImageProcessing.h)
target_link_libraries(RayTraceCPU.exe
        ${SDL2_LIBRARIES}
        ${SDL2_IMG_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${OPENGL_LIBRARIES})

add_executable(RayTraceGPU.exe
        ${GKIT_SRC_FILES}
        src/RayTrace/GPU/mainGPU.cpp
        src/RayTrace/Utils/Utils.cpp
        src/RayTrace/Utils/Utils.h
        src/RayTrace/CPU/RayTraceImageProcessing.cpp
        src/RayTrace/CPU/RayTraceImageProcessing.h
        src/RayTrace/Utils/OpenGLUtils.h src/RayTrace/GPU/ComputeShaderTest.cpp)
target_link_libraries(RayTraceGPU.exe
        ${SDL2_LIBRARIES}
        ${SDL2_IMG_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${OPENGL_LIBRARIES})

add_executable(TestOpenGL.exe
        ${GKIT_SRC_FILES}
        src/TestOpenGL.cpp)
target_link_libraries(TestOpenGL.exe
        ${SDL2_LIBRARIES}
        ${SDL2_IMG_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${OPENGL_LIBRARIES})

add_executable(ComputeShaderTest.exe
        ${GKIT_SRC_FILES}
        src/RayTrace/GPU/ComputeShaderTest.cpp)
target_link_libraries(ComputeShaderTest.exe
        ${SDL2_LIBRARIES}
        ${SDL2_IMG_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${OPENGL_LIBRARIES})

add_executable(BezierSurface.exe
        ${GKIT_SRC_FILES}
        src/SurfaceBezier/Modelisation_TP3.cpp
        src/SurfaceBezier/surface_bezier.h
        src/SurfaceBezier/structNE.h)
target_link_libraries(BezierSurface.exe
        ${SDL2_LIBRARIES}
        ${SDL2_IMG_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${OPENGL_LIBRARIES})